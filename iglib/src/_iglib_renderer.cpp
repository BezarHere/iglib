#include "pch.h"
#include "_iglib_renderer.h"
#include "internal.h"

static Vertex2D g_ScreenQuadVertcies[ 4 ]
{
	{ {0.f, 0.f}, {1.f, 1.f, 1.f, 1.f}, {0.f, 1.f} }, // topleft
	{ {0.f, 1.f}, {1.f, 1.f, 1.f, 1.f}, {0.f, 0.f} }, // bottomleft
	{ {1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}, {1.f, 0.f} }, // bottomright
	{ {1.f, 0.f}, {1.f, 1.f, 1.f, 1.f}, {1.f, 1.f} } // topright
};

static Vertex2DBuffer g_ScreenQuadBuffer{};
static ShaderInstance_t g_ScreenShader{};
static ShaderInstance_t g_DefaultShaders[ (int)ShaderUsage::_Max ]{};
static std::shared_ptr<const Texture> g_PlankTexture;
static const Renderer *g_BoundRenderer = nullptr;
static struct RendererOpenglPressitenceState
{
	Vector2i size;
	bool postprocessing;
} g_RendererGlState;


static FORCEINLINE void init_globals()
{
	static bool s__has_ran = false;
	if (s__has_ran)return; s__has_ran = true;

	static const std::string PostProcessing_Vert =
		"void main() { gl_Position = vec4(to_native_space(pos), 0.0, 1.0); UV = texcoord; }";

	static const std::string PostProcessing_Frag =
		"void main() {"
			"Color = vec4(texture(uTex0, UV).rgb, 1.0);"
		//"Color = vec4(UV, 1.0, 1.0);"
	"}";

	g_ScreenQuadBuffer.set_primitive( PrimitiveType::Quad );
	g_ScreenQuadBuffer.set_usage( BufferUsage::Dynamic );
	g_ScreenQuadBuffer.create( 4, nullptr );
	g_ScreenShader = Shader::compile(
		PostProcessing_Vert, PostProcessing_Frag, ShaderUsage::ScreenSpace
	);
	
	for (int i = 0; i < (int)ShaderUsage::_Max; i++)
	{
		g_DefaultShaders[ i ] = Shader::get_default( ShaderUsage( i ) );
	}


	// generating plank texutre
	{
		constexpr byte plank_white_data[ 2 * 2 * 4 ]
		{
			255, 255, 255, 255,
			255, 255, 255, 255,
			255, 255, 255, 255,
			255, 255, 255, 255,
		};
		g_PlankTexture.reset( new Texture( Image( plank_white_data, { 2, 2 }, ColorFormat::L ) ) );
	}
}

namespace ig
{
	static constexpr GLuint RenderTextureType = GL_RGB;
	static constexpr GLuint RenderTextureTypeHDR = GL_RGB16F;
	static constexpr GLuint RenderDepthStencilMode = GL_DEPTH24_STENCIL8;

	struct Renderer::RenderBuffersState::Regenarator
	{
		FORCEINLINE static void cleanup( Renderer::RenderBuffersState &buffer_state )
		{
			glDeleteFramebuffers( 1, &buffer_state.framebuffer_object );
			glDeleteRenderbuffers( 1, &buffer_state.renderbuffer_object );
			glDeleteTextures( 1, &buffer_state.colorbuffer_object );

			buffer_state.framebuffer_object = NULL;
			buffer_state.renderbuffer_object = NULL;
			buffer_state.colorbuffer_object = NULL;
		}

		FORCEINLINE static void regenerate( Renderer::RenderBuffersState &buffer_state, Vector2i size, const RenderEnviorment &env )
		{
			cleanup( buffer_state );

			glGenFramebuffers( 1, &buffer_state.framebuffer_object );

			REPORT_V( buffer_state.framebuffer_object == NULL, cleanup( buffer_state ) );

			glGenTextures( 1, &buffer_state.colorbuffer_object );

			if (buffer_state.colorbuffer_object == NULL)
			{
				glDeleteFramebuffers( 1, &buffer_state.framebuffer_object );
				REPORT_V( buffer_state.colorbuffer_object == NULL, cleanup( buffer_state ) );
			}

			glBindTexture( GL_TEXTURE_2D, buffer_state.colorbuffer_object );
			glTexImage2D( GL_TEXTURE_2D, 0, env.hdr ? RenderTextureTypeHDR : RenderTextureType, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );


			glGenRenderbuffers( 1, &buffer_state.renderbuffer_object );

			if (buffer_state.renderbuffer_object == NULL)
			{
				glDeleteFramebuffers( 1, &buffer_state.framebuffer_object );
				glDeleteTextures( 1, &buffer_state.colorbuffer_object );
				REPORT_V( buffer_state.renderbuffer_object == NULL, cleanup( buffer_state ) );
			}


			glBindRenderbuffer( GL_RENDERBUFFER, buffer_state.renderbuffer_object );
			glRenderbufferStorage( GL_RENDERBUFFER, RenderDepthStencilMode, size.x, size.y );



			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, buffer_state.framebuffer_object );
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer_state.colorbuffer_object, 0 );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer_state.renderbuffer_object );

			WARN( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ); // <- BUGBUG

			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, NULL );
			glBindRenderbuffer( GL_RENDERBUFFER, NULL );
			glBindTexture( GL_TEXTURE_2D, NULL );
		}
	};

	Renderer::RenderBuffersState::RenderBuffersState()
		: colorbuffer_object{ NULL }, framebuffer_object{ NULL },
			renderbuffer_object{ NULL }, colorbuffer_size{ -1, -1 }
			
	{
	}

	Renderer::RenderBuffersState::~RenderBuffersState()
	{
		Regenarator::cleanup( *this );
	}

	Renderer::Renderer( const Window &window, RenderCallback render_callback )
		: m_window{ window }, m_canvas{ *this },
			m_callback{ render_callback }
	{
		init_globals();
	}

	void Renderer::clear()
	{
		push_to_draw_pipline( (WindowHandle_t)m_window.m_hdl );
		glClearColor( m_background_clr.r, m_background_clr.g, m_background_clr.b, m_background_clr.a );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		pop_draw_pipline();
	}
	
	bool Renderer::draw()
	{
		if (g_BoundRenderer)
		{
			bite::raise( "Drawing to Renderer while there is a renderer already boud!" );
		}
		g_BoundRenderer = this;

		const Vector2i sz = m_window.size();

		// MINIMIZED, no drawing
		if (sz.area() == 0)
		{
			g_BoundRenderer = nullptr;
			return false;
		}

		push_to_draw_pipline( (WindowHandle_t)m_window.m_hdl );

		const bool postprocessing = m_enviorment.enabled_postprocessing;
		if (postprocessing && sz != m_buffers_state.colorbuffer_size)
		{
			RenderBuffersState::Regenarator::regenerate( m_buffers_state, sz, m_enviorment );
		}


		glEnable( GL_SCISSOR_TEST );
		glEnable( GL_BLEND );
		glEnable( GL_DEPTH_CLAMP );

		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glScissor( 0, 0, sz.x, sz.y );
		//glCullFace(GL_FRONT);

		
		if (postprocessing)
		{
			glBindRenderbuffer( GL_RENDERBUFFER, m_buffers_state.renderbuffer_object );
			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_buffers_state.renderbuffer_object );
			glClearColor( m_background_clr.r, m_background_clr.g, m_background_clr.b, m_background_clr.a );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		}

		g_RendererGlState.size = sz;
		g_RendererGlState.postprocessing = postprocessing;

		if (m_callback)
			m_callback( *this );
		else
			bite::warn( "Renderer has no render callback" );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, NULL );
		glBindRenderbuffer( GL_RENDERBUFFER, NULL );
		glDisable( GL_DEPTH_TEST );
		glDisable( GL_CULL_FACE );
		glDisable( GL_SCISSOR_TEST );
		glDisable( GL_BLEND );
		glDisable( GL_DEPTH_CLAMP );

		if (g_RendererGlState.postprocessing)
		{
			g_ScreenQuadVertcies[ 1 ].pos.y = (float)g_RendererGlState.size.y;
			g_ScreenQuadVertcies[ 2 ].pos = Vector2f( g_RendererGlState.size );
			g_ScreenQuadVertcies[ 3 ].pos.x = (float)g_RendererGlState.size.x;
			g_ScreenQuadBuffer.update( g_ScreenQuadVertcies );

			bind_shader( g_ScreenShader );
			bind_texture( m_buffers_state.colorbuffer_object );
			m_canvas.draw( g_ScreenQuadBuffer );
		}

		pop_draw_pipline();
		glfwSwapBuffers( (GLFWwindow *)m_window.m_hdl );
		g_BoundRenderer = nullptr;
	}

	void Renderer::set_callback( RenderCallback callback )
	{
		m_callback = callback;
	}

	RenderCallback Renderer::get_callback() const
	{
		return m_callback;
	}

	Canvas &Renderer::get_canvas()
	{
		return m_canvas;
	}

	bool Renderer::is_active() const
	{
		return g_BoundRenderer == this;
	}

	const Window &Renderer::get_window() const
	{
		return m_window;
	}

	void Renderer::set_draw_type( const DrawType type )
	{
		m_state.draw_type = type;

		switch (type)
		{
		case DrawType::Drawing2D:
			m_state.shading_usage = ShaderUsage::Usage2D;

			glDisable( GL_DEPTH_TEST );
			glDisable( GL_CULL_FACE );

			break;
		case DrawType::Drawing3D:
			m_state.shading_usage = ShaderUsage::Usage3D;
			glEnable( GL_DEPTH_TEST );
			glEnable( GL_CULL_FACE );

			glFrontFace( GL_CCW );
			glCullFace( GL_FRONT );

			break;
		case DrawType::DirectDrawing:
			m_state.shading_usage = ShaderUsage::ScreenSpace;

			glDisable( GL_DEPTH_TEST );
			glDisable( GL_CULL_FACE );
			break;
		case DrawType::Raw:
			m_state.shading_usage = ShaderUsage::ScreenSpace;

			glDisable( GL_DEPTH_TEST );
			glDisable( GL_CULL_FACE );
			break;
		default:
			break;
		}
	}

	void Renderer::bind_shader( const ShaderInstance_t &shader )
	{
		if (m_state.bound_shader.get() == shader.get())
			return;

		if (!shader)
		{
			bite::warn( "trying to bind a NULL shader to canvas!" );
			return;
		}

		glUseProgram( shader->get_id() );
		m_state.bound_shader = shader;
		try_update_shader_state();
	}

	void Renderer::bind_default_shader( ShaderUsage usage )
	{
		bind_shader( g_DefaultShaders[ (int)usage ] );
	}

	void Renderer::unbind_shader()
	{
		glUseProgram( g_DefaultShaders[ (int)m_state.shading_usage ]->get_id() );
		m_state.bound_shader = g_DefaultShaders[ (int)m_state.shading_usage ];
		try_update_shader_state();
	}

	void Renderer::try_update_shader_state()
	{
		if (!m_state.bound_shader)
			return;

		const int atc = m_state.active_textrues_count;
		for (int i = 0; i < atc; i++)
		{
			glActiveTexture( GL_TEXTURE0 + i );
			const auto hdl = m_state.textures[ i ] ? m_state.textures[ i ] : g_PlankTexture->get_handle();
			glBindTexture( GL_TEXTURE_2D, hdl );
		}

		glUniform2f( glGetUniformLocation( m_state.bound_shader->get_id(), "_screensize" ), (float)m_window.width(), (float)m_window.height() );

		if (m_state.bound_shader->get_usage() == ShaderUsage::Usage2D)
		{
			glUniformMatrix2fv( glGetUniformLocation( m_state.bound_shader->get_id(), "_trans" ), 1, GL_FALSE, m_canvas.m_transform2d.get_data().f );
			glUniform2f( glGetUniformLocation( m_state.bound_shader->get_id(), "_offset" ), m_canvas.m_transform2d.get_data().origin.x, m_canvas.m_transform2d.get_data().origin.y );
		}
		else if (m_state.bound_shader->get_usage() == ShaderUsage::Usage3D)
		{
			glUniform3f( glGetUniformLocation( m_state.bound_shader->get_id(), "_offset" ), m_canvas.m_transform3d.get_data().origin.x, m_canvas.m_transform3d.get_data().origin.y, m_canvas.m_transform3d.get_data().origin.z );
			glUniformMatrix3fv( glGetUniformLocation( m_state.bound_shader->get_id(), "_trans" ), 1, GL_TRUE, m_canvas.m_transform3d.get_data().f );
			glUniformMatrix4fv( glGetUniformLocation( m_state.bound_shader->get_id(), "_proj" ), 1, GL_TRUE, m_canvas.m_cam_cache.m_elements.data() );
			glUniformMatrix3fv( glGetUniformLocation( m_state.bound_shader->get_id(), "_view_transform" ), 1, GL_TRUE, m_canvas.m_camera.transform.get_data().f );
			glUniform3fv( glGetUniformLocation( m_state.bound_shader->get_id(), "_view_position" ), 1, m_canvas.m_camera.transform.get_data().f + 9 );
		}

	}

	void Renderer::set_cullwinding( CullWinding winding )
	{
		glFrontFace( int( winding ) );
	}

	void Renderer::set_cullface( CullFace face )
	{
		glCullFace( int( face ) );
	}

	void Renderer::enable_feature( Feature feature )
	{
		glEnable( int( feature ) );
	}

	void Renderer::disable_feature( Feature feature )
	{
		glDisable( int( feature ) );
	}

	ShaderId_t Renderer::get_shader_id() const noexcept
	{
		return m_state.bound_shader->get_id();
	}

	void Renderer::bind_texture( const TextureId_t tex, const TextureSlot slot )
	{
		m_state.textures[ int( slot ) ] = tex;
		try_update_shader_state();
	}

	TextureId_t ig::Renderer::get_texture( const TextureSlot slot ) const noexcept
	{
		return m_state.textures[ int( slot ) ];
	}

	void Renderer::set_active_textures_count( int count )
	{
		if (count < 0)
		{
			bite::warn( "can't set the active textures count to less then zero" );
			return;
		}

		if (count >= int( TextureSlot::_MAX ))
		{
			bite::warn( "can't set the active textures count to greater or equal to zero" );
			return;
		}

		m_state.active_textrues_count = count;
	}

	int Renderer::get_active_textures_count() const noexcept
	{
		return m_state.active_textrues_count;
	}

	void Renderer::set_enviorment( const RenderEnviorment &env )
	{
		m_enviorment = env;
		try_update_shader_state();
	}

	const RenderEnviorment &Renderer::get_enviorment()
	{
		return m_enviorment;
	}


	void Renderer::set_shader_uniform( int location, int value )
	{
		glUniform1i( location, value );
	}

	void Renderer::set_shader_uniform( int location, float value )
	{
		glUniform1f( location, value );
	}

	void Renderer::set_shader_uniform( int location, Vector2i value )
	{
		glUniform2i( location, value.x, value.y );
	}

	void Renderer::set_shader_uniform( int location, Vector2f value )
	{
		glUniform2f( location, value.x, value.y );
	}

	void Renderer::set_shader_uniform( int location, Vector3i value )
	{
		glUniform3i( location, value.x, value.y, value.z );
	}

	void Renderer::set_shader_uniform( int location, Vector3f value )
	{
		glUniform3f( location, value.x, value.y, value.z );
	}

	void Renderer::set_shader_uniform( int location, const Vector4i &value )
	{
		glUniform4i( location, value.x, value.y, value.z, value.w );
	}

	void Renderer::set_shader_uniform( int location, const Vector4f &value )
	{
		glUniform4f( location, value.x, value.y, value.z, value.w );
	}

	void Renderer::set_shader_uniform( int location, int count, const int *value )
	{
		glUniform1iv( location, count, value );
	}

	void Renderer::set_shader_uniform( int location, int count, const float *value )
	{
		glUniform1fv( location, count, value );
	}

	void Renderer::set_shader_uniform( int location, int count, const Vector2i *value )
	{
		glUniform2iv( location, count, (const GLint *)value );
	}

	void Renderer::set_shader_uniform( int location, int count, const Vector2f *value )
	{
		glUniform2fv( location, count, (const GLfloat *)value );
	}

	void Renderer::set_shader_uniform( int location, int count, const Vector3i *value )
	{
		glUniform3iv( location, count, (const GLint *)value );
	}

	void Renderer::set_shader_uniform( int location, int count, const Vector3f *value )
	{
		glUniform3fv( location, count, (const GLfloat *)value );
	}

	void Renderer::set_shader_uniform( int location, int count, const Vector4i *value )
	{
		glUniform4iv( location, count, (const GLint *)value );
	}

	void Renderer::set_shader_uniform( int location, int count, const Vector4f *value )
	{
		glUniform4fv( location, count, (const GLfloat *)value );
	}

}
