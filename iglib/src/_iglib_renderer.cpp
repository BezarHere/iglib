#include "pch.h"
#include "_iglib_renderer.h"
#include "draw_internal.h"
#include "internal.h"

enum ScreenTextureBuffers
{
	STF_Color = 0,
	STF_Overbright = 1,
};

static Vertex2 g_ScreenQuadVertcies[ 4 ]
{
	{ {0.f, 0.f}, {1.f, 1.f, 1.f, 1.f}, {0.f, 1.f} }, // topleft
	{ {0.f, 1.f}, {1.f, 1.f, 1.f, 1.f}, {0.f, 0.f} }, // bottomleft
	{ {1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}, {1.f, 0.f} }, // bottomright
	{ {1.f, 0.f}, {1.f, 1.f, 1.f, 1.f}, {1.f, 1.f} } // topright
};

static Vertex2Buffer g_ScreenQuadBuffer{};
static std::shared_ptr<Shader> g_ScreenShader{};
static std::shared_ptr<Shader> g_DefaultShaders[ (int)ShaderUsage::_Max ]{};
static std::shared_ptr<const Texture> g_PlankTexture;
static const Renderer *g_BoundRenderer = nullptr;
static struct RendererOpenglPersistenceState
{
	Vector2i size;
	bool postprocessing;
} g_RendererGlState{};

// will haunt me later
static const Canvas *g_ActiveCanvas;

static FORCEINLINE void init_globals() {
	static bool s__has_ran = false;
	if (s__has_ran)return; s__has_ran = true;

	constexpr char PostProcessing_Vert[] =
		"void main() { gl_Position = vec4(to_native_space(pos), 0.0, 1.0); UV = texcoord; }";

	constexpr char PostProcessing_Frag[] =
		"void main() {"
		" if(UV.x < 0.5){ Color = vec4(pow(texture(uTex0, UV).rgb, vec3(1.34)), 1.0); } else { Color = vec4(pow(texture(uTex1, UV).rgb, vec3(1.34)), 1.0); }"
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


	// generating plank texture
	{
		constexpr byte plank_white_data[ 2 * 2 * 4 ]
		{
			255, 255, 255, 255,
			255, 255, 255, 255,
			255, 255, 255, 255,
			255, 255, 255, 255,
		};
		g_PlankTexture.reset( new Texture( Image( plank_white_data, { 2, 2 }, ColorFormat::RGBA ) ) );
	}
}

namespace ig
{
	static constexpr GLuint RenderTextureTypeLow = GL_RGB16F;
	static constexpr GLuint RenderTextureType = GL_RGB16F;
	static constexpr GLuint RenderTextureTypeHDR = GL_RGB;
	static constexpr GLuint RenderDepthStencilMode = GL_DEPTH24_STENCIL8;

	struct Renderer::RenderBuffersState::Regenerator
	{
		FORCEINLINE static void cleanup( Renderer::RenderBuffersState &buffer_state ) {
			glDeleteFramebuffers( 1, &buffer_state.framebuffer_object );
			glDeleteRenderbuffers( 1, &buffer_state.renderbuffer_object );
			glDeleteTextures( 2, buffer_state.color_buffers.data() );

			buffer_state.framebuffer_object = NULL;
			buffer_state.renderbuffer_object = NULL;
			buffer_state.color_buffers = { 0 };
		}

		FORCEINLINE static void regenerate( Renderer::RenderBuffersState &buffer_state, Vector2i size, const RenderEnvironment &env ) {
			cleanup( buffer_state );
			buffer_state.color_buffer_size = size;

			glGenFramebuffers( 1, &buffer_state.framebuffer_object );

			WARN( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ); // <- BUGBUG

			REPORT_V( buffer_state.framebuffer_object == NULL, cleanup( buffer_state ) );

			glGenTextures( (GLsizei)buffer_state.color_buffers.size(), buffer_state.color_buffers.data() );

			for (size_t i = 0; i < buffer_state.color_buffers.size(); i++)
			{
				if (buffer_state.color_buffers[ i ] == NULL)
				{
					glDeleteTextures( (GLsizei)buffer_state.color_buffers.size(), buffer_state.color_buffers.data() );
					glDeleteFramebuffers( 1, &buffer_state.framebuffer_object );
					REPORT_V( buffer_state.color_buffers[ i ] == NULL, cleanup( buffer_state ) ); // <- what value is 'i'
				}

				glBindTexture( GL_TEXTURE_2D, buffer_state.color_buffers[ i ] );
				switch (ScreenTextureBuffers( i ))
				{
				case STF_Color:
					{
						glTexImage2D( GL_TEXTURE_2D,
													0,
													env.hdr ? RenderTextureTypeHDR : RenderTextureType,
													size.x,
													size.y,
													0,
													GL_RGB,
													GL_UNSIGNED_BYTE,
													nullptr );
					}
					break;
				case STF_Overbright:
					{
						glTexImage2D( GL_TEXTURE_2D,
													0,
													RenderTextureTypeLow,
													size.x,
													size.y,
													0,
													GL_RGB,
													GL_UNSIGNED_BYTE,
													nullptr );
					}
					break;
				default:
					break;
				}

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			}

			glGenRenderbuffers( 1, &buffer_state.renderbuffer_object );

			if (buffer_state.renderbuffer_object == NULL)
			{
				glDeleteFramebuffers( 1, &buffer_state.framebuffer_object );
				glDeleteTextures( (GLsizei)buffer_state.color_buffers.size(), buffer_state.color_buffers.data() );
				REPORT_V( buffer_state.renderbuffer_object == NULL, cleanup( buffer_state ) );
			}


			glBindRenderbuffer( GL_RENDERBUFFER, buffer_state.renderbuffer_object );
			glRenderbufferStorage( GL_RENDERBUFFER, RenderDepthStencilMode, size.x, size.y );



			glBindFramebuffer( GL_FRAMEBUFFER, buffer_state.framebuffer_object );
			for (size_t i = 0; i < buffer_state.color_buffers.size(); i++)
			{
				glFramebufferTexture2D( GL_FRAMEBUFFER, (GLsizei)(GL_COLOR_ATTACHMENT0 + i), GL_TEXTURE_2D, buffer_state.color_buffers[ i ], 0 );
			}
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer_state.renderbuffer_object );

			{
				constexpr size_t color_buffers_count = sizeof( buffer_state.color_buffers ) / sizeof( buffer_state.color_buffers[ 0 ] );
				constexpr GLuint draw_buffers_attachments[ 10 ] = {
					GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4,
					GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
				};

				glDrawBuffers( color_buffers_count, draw_buffers_attachments );
			}

			WARN( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ); // <- BUGBUG

			glBindFramebuffer( GL_FRAMEBUFFER, NULL );
			glBindRenderbuffer( GL_RENDERBUFFER, NULL );
			glBindTexture( GL_TEXTURE_2D, NULL );
		}
	};

	Renderer::RenderBuffersState::RenderBuffersState()
		: color_buffers{ NULL }, framebuffer_object{ NULL },
		renderbuffer_object{ NULL }, color_buffer_size{ -1, -1 }

	{
	}

	Renderer::RenderBuffersState::~RenderBuffersState() {
		Regenerator::cleanup( *this );
	}

	Renderer::Renderer( const Window &window, RenderCallback render_callback )
		: m_window{ window }, m_active_canvas{},
		m_callback{ render_callback } {
		init_globals();
	}

	void Renderer::clear() {
		push_to_draw_pipline( (WindowHandle_t)m_window.m_hdl );
		glClearColor( m_background_clr.r, m_background_clr.g, m_background_clr.b, m_background_clr.a );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		pop_draw_pipline();
	}

	bool Renderer::draw() {
		if (g_BoundRenderer)
		{
			bite::raise( "Drawing to Renderer while there is a renderer already bound!" );
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

		const bool postprocessing = m_environment.enabled_postprocessing;
		if (postprocessing && sz != m_buffers_state.color_buffer_size)
		{
			RenderBuffersState::Regenerator::regenerate( m_buffers_state, sz, m_environment );
		}

		GLuint vao;
		glGenVertexArrays( 1, &vao );
		glBindVertexArray( vao );


		glEnable( GL_SCISSOR_TEST );
		glEnable( GL_DEPTH_CLAMP );

		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glScissor( 0, 0, sz.x, sz.y );
		//glCullFace(GL_FRONT);


		if (postprocessing)
		{
			glBindRenderbuffer( GL_RENDERBUFFER, m_buffers_state.renderbuffer_object );
			glBindFramebuffer( GL_FRAMEBUFFER, m_buffers_state.framebuffer_object );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			glClearColor( m_background_clr.r, m_background_clr.g, m_background_clr.b, m_background_clr.a );
		}

		g_RendererGlState.size = sz;
		g_RendererGlState.postprocessing = postprocessing;

		m_active_canvas.m_renderer = this;
		set_draw_type( DrawType::Drawing2D );
		bind_default_shader( ShaderUsage::Usage2D );
		if (m_callback)
			m_callback( *this );
		else
			bite::warn( "Renderer has no render callback" );


		glBindRenderbuffer( GL_RENDERBUFFER, NULL );
		glBindFramebuffer( GL_FRAMEBUFFER, NULL );

		glDisable( GL_DEPTH_TEST );
		glDisable( GL_CULL_FACE );
		glDisable( GL_SCISSOR_TEST );
		glDisable( GL_BLEND );
		glDisable( GL_DEPTH_CLAMP );

		// why? to remove all the transforms and camera cfg
		m_active_canvas = Canvas( this );

		if (g_RendererGlState.postprocessing)
		{
			g_ScreenQuadVertcies[ 1 ].pos.y = (float)g_RendererGlState.size.y;
			g_ScreenQuadVertcies[ 2 ].pos = Vector2f( g_RendererGlState.size );
			g_ScreenQuadVertcies[ 3 ].pos.x = (float)g_RendererGlState.size.x;
			g_ScreenQuadBuffer.update( g_ScreenQuadVertcies );

			set_draw_type( DrawType::Drawing2D );
			bind_shader( g_ScreenShader.get() );
			bind_texture( m_buffers_state.color_buffers[ STF_Color ], TextureSlot::Slot0 );
			bind_texture( m_buffers_state.color_buffers[ STF_Overbright ], TextureSlot::Slot1 );
			m_active_canvas.draw( g_ScreenQuadBuffer );


		}

		glBindVertexArray( NULL );
		glDeleteVertexArrays( 1, &vao );

		bind_texture( 0 );
		pop_draw_pipline();
		glfwSwapBuffers( (GLFWwindow *)m_window.m_hdl );

		m_active_canvas.m_renderer = nullptr; // <- clear out the renderer so later draw calls fail until the next draw op
		g_BoundRenderer = nullptr;
		return true;
	}

	void Renderer::set_callback( RenderCallback callback ) {
		m_callback = callback;
	}

	RenderCallback Renderer::get_callback() const {
		return m_callback;
	}

	Canvas &Renderer::get_canvas() {
		return m_active_canvas;
	}

	bool Renderer::is_active() const {
		return g_BoundRenderer == this;
	}

	const Window &Renderer::get_window() const {
		return m_window;
	}

	void Renderer::set_draw_type( const DrawType type ) {
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

	void Renderer::bind_shader( const Shader *shader ) {
		if (shader == nullptr)
		{
			bite::warn( "trying to bind a NULL shader to canvas!" );
			return;
		}

		shader->bind();
		m_state.bound_shader = shader;
		try_update_shader_state();
	}

	void Renderer::bind_default_shader( ShaderUsage usage ) {
		bind_shader( g_DefaultShaders[ (int)usage ].get() );
	}

	void Renderer::unbind_shader() {
		g_DefaultShaders[ (int)m_state.shading_usage ]->bind();
		m_state.bound_shader = g_DefaultShaders[ (int)m_state.shading_usage ].get();
		try_update_shader_state();
	}

	void Renderer::try_update_shader_state() {
		if (!m_state.bound_shader)
			return;

		const int atc = m_state.active_textures_count;
		for (int i = 0; i < atc; i++)
		{
			Texture::activate_slot( static_cast<TextureSlot>(i) );
			const auto hdl = m_state.textures[ i ] ? m_state.textures[ i ] : g_PlankTexture->get_handle();

			// find something better
			glBindTexture( GL_TEXTURE_2D, hdl );
		}

		glUniform2f( glGetUniformLocation( m_state.bound_shader->get_name(), "_screensize" ), (float)m_window.width(), (float)m_window.height() );

		if (m_state.bound_shader->get_usage() == ShaderUsage::Usage2D)
		{
			glUniformMatrix2fv( glGetUniformLocation( m_state.bound_shader->get_name(), "_trans" ), 1, GL_FALSE, m_active_canvas.m_transform2d.get_data().f );
			glUniform2f( glGetUniformLocation( m_state.bound_shader->get_name(), "_offset" ), m_active_canvas.m_transform2d.get_data().origin.x, m_active_canvas.m_transform2d.get_data().origin.y );
		}
		else if (m_state.bound_shader->get_usage() == ShaderUsage::Usage3D)
		{
			const auto &trans3d = m_active_canvas.transform3d().get_data();
			glUniform3f( glGetUniformLocation( m_state.bound_shader->get_name(), "_offset" ), trans3d.origin.x, trans3d.origin.y, trans3d.origin.z );
			glUniformMatrix3fv( glGetUniformLocation( m_state.bound_shader->get_name(), "_trans" ), 1, GL_TRUE, trans3d.f );
			glUniformMatrix4fv( glGetUniformLocation( m_state.bound_shader->get_name(), "_proj" ), 1, GL_TRUE, m_active_canvas.m_cam_cache.m_elements.data() );
			glUniformMatrix3fv( glGetUniformLocation( m_state.bound_shader->get_name(), "_view_transform" ), 1, GL_TRUE, m_active_canvas.m_camera.transform.get_data().f );
			glUniform3fv( glGetUniformLocation( m_state.bound_shader->get_name(), "_view_position" ), 1, m_active_canvas.m_camera.transform.get_data().f + 9 );
		}

	}

	void Renderer::set_cullwinding( const CullWinding winding ) {
		glFrontFace( static_cast<int>(winding) );
	}

	void Renderer::set_cullface( const CullFace face ) {
		glCullFace( static_cast<int>(face) );
	}

	void Renderer::enable_feature( const Feature feature ) {
		glEnable( static_cast<int>(feature) );
	}

	void Renderer::disable_feature( const Feature feature ) {
		glDisable( static_cast<int>(feature) );
	}

	bool Renderer::is_feature_enabled( const Feature feature ) const {
		return glIsEnabled( static_cast<int>(feature) );
	}

	void Renderer::set_depth_test_comparison( DepthTestComparison comparison ) {
		glDepthFunc( static_cast<GLenum>(comparison) );
	}

	DepthTestComparison Renderer::get_depth_test_comparison() const {
		DepthTestComparison dtc = DepthTestComparison::LessThen;
		glGetIntegerv( GL_DEPTH_FUNC, reinterpret_cast<GLint *>(&dtc) );
		return dtc;
	}

	ShaderName Renderer::get_shader_name() const noexcept {
		return m_state.bound_shader->get_name();
	}

	void Renderer::bind_texture( const TextureId tex, const TextureSlot slot ) {
		m_state.textures[ int( slot ) ] = tex;
		try_update_shader_state();
	}

	TextureId ig::Renderer::get_texture( const TextureSlot slot ) const noexcept {
		return m_state.textures[ int( slot ) ];
	}

	void Renderer::set_active_textures_count( int count ) {
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

		m_state.active_textures_count = count;
	}

	int Renderer::get_active_textures_count() const noexcept {
		return m_state.active_textures_count;
	}

	void Renderer::set_environment( const RenderEnvironment &env ) {
		m_environment = env;
		try_update_shader_state();
	}

	const RenderEnvironment &Renderer::get_environment() {
		return m_environment;
	}


	void Renderer::set_shader_uniform( int location, int value ) {
		glUniform1i( location, value );
	}

	void Renderer::set_shader_uniform( int location, float value ) {
		glUniform1f( location, value );
	}

	void Renderer::set_shader_uniform( int location, Vector2i value ) {
		glUniform2i( location, value.x, value.y );
	}

	void Renderer::set_shader_uniform( int location, Vector2f value ) {
		glUniform2f( location, value.x, value.y );
	}

	void Renderer::set_shader_uniform( int location, Vector3i value ) {
		glUniform3i( location, value.x, value.y, value.z );
	}

	void Renderer::set_shader_uniform( int location, Vector3f value ) {
		glUniform3f( location, value.x, value.y, value.z );
	}

	void Renderer::set_shader_uniform( int location, const Vector4i &value ) {
		glUniform4i( location, value.x, value.y, value.z, value.w );
	}

	void Renderer::set_shader_uniform( int location, const Vector4f &value ) {
		glUniform4f( location, value.x, value.y, value.z, value.w );
	}

	void Renderer::set_shader_uniform( int location, int count, const int *value ) {
		glUniform1iv( location, count, value );
	}

	void Renderer::set_shader_uniform( int location, int count, const float *value ) {
		glUniform1fv( location, count, value );
	}

	void Renderer::set_shader_uniform( int location, int count, const Vector2i *value ) {
		glUniform2iv( location, count, (const GLint *)value );
	}

	void Renderer::set_shader_uniform( int location, int count, const Vector2f *value ) {
		glUniform2fv( location, count, (const GLfloat *)value );
	}

	void Renderer::set_shader_uniform( int location, int count, const Vector3i *value ) {
		glUniform3iv( location, count, (const GLint *)value );
	}

	void Renderer::set_shader_uniform( int location, int count, const Vector3f *value ) {
		glUniform3fv( location, count, (const GLfloat *)value );
	}

	void Renderer::set_shader_uniform( int location, int count, const Vector4i *value ) {
		glUniform4iv( location, count, (const GLint *)value );
	}

	void Renderer::set_shader_uniform( int location, int count, const Vector4f *value ) {
		glUniform4fv( location, count, (const GLfloat *)value );
	}

	void Renderer::render( const PrimitiveType primitive, const int begin, const int count ) {
		glDrawArrays( to_glprimitve( primitive ), begin, count );
	}

	void Renderer::render( PrimitiveType primitive, int count, VertexIndexType vertex_index_type, unsigned index_offset ) {
		glDrawElements(
			to_glprimitve( primitive ),
			count,
			static_cast<GLenum>(vertex_index_type),
			(const void *)((size_t)index_offset)
		);
	}

}
