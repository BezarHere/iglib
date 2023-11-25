#include "pch.h"
#include "_iglib_window.h"
#include "_iglib_canvas.h"
#include "internal.h"
#include "intrinsics.h"
#include "draw_internal.h"

// all of the unsigned int's bits turned on except the last one
constexpr unsigned int UnsignedIntNonTrailingBitMask = (1u << (sizeof(unsigned int) * 8 - 1)) - 1;
constexpr unsigned int UnsignedIntLastBit = 1u << ((sizeof(unsigned int) * 8) - 1);
constexpr unsigned int DisabledTextureSlotMask = UnsignedIntLastBit;


// yeah!
#define to_clamped_space(v, w) v

typedef basic_heap_span<Vector2f> Vector2fSpan_t;
FORCEINLINE [[nodiscard]] const Vector2fSpan_t _generate_circle_frame(uint16_t res)
{
	const size_t vcount = (size_t)res;
	alignas(64) Vector2f *verts = new Vector2f[vcount];
	constexpr Vector2f Left = { 1.f, 0.f };
	float r = 0.f;
	const float step = Tau / (float)res;

	verts[ 0 ] = Left;

	for (size_t i = 1; i < vcount; i++)
	{
		r += step;
		verts[ i ] = Left.rotated(r);
		//std::cout << i << " stepped: " << r << " -> " << Left.rotated(r) << '\n';
	}
	return Vector2fSpan_t{ verts, vcount };
}

FORCEINLINE const Vector2fSpan_t &get_circle_frame(uint16_t res)
{
	static std::vector<Vector2fSpan_t> s_CircleFrames(1 << 16);
	static const Vector2fSpan_t VEmpty{};

	if (res < 3)
		return VEmpty;

	if (!s_CircleFrames[res].sz)
		s_CircleFrames[res] = _generate_circle_frame(res);
	return s_CircleFrames.at(res);
}
static ShaderInstance_t DefaultShaders[(int)ShaderUsage::_Max]{};

// a cube consists of two quads each one consisting of two tringles
static Vertex3DBuffer DefaultCubeBuffer{};
constexpr IndexBuffer::index_type DefaultCubeIndexBuffer_Values[]
{
	// front
	0, 1, 2,
	1, 2, 3,
	
	// right
	2, 3, 4,
	3, 4, 5,
	
	// back
	4, 5, 6,
	5, 6, 7,
	
	// left
	6, 7, 0,
	7, 0, 1,
	
	
	// top
	7, 5, 1,
	5, 1, 3,
	
	// bottom
	6, 7, 0,
	7, 0, 1,
};

static IndexBuffer DefaultCubeIndexBuffer{};
static Vertex3DBuffer DefaultLineBuffer{};

static Vertex2DBuffer g_Quad2DBuffer;
static Vertex2DBuffer g_Triangle2DBuffer;
static Vertex2DBuffer g_Line2DBuffer;

static Vertex2D g_Quad2DVertcies[ 4 ]{};
static Vertex2D g_Triangle2DVertcies[ 3 ]{};
static Vertex2D g_Line2DVertcies[ 2 ]{};

static const Canvas *g_CurrentCanvas;
static std::shared_ptr<const Texture> g_PlankTexture;

namespace ig { // def in _iglib_font.h
	extern void _font_init();
}

FORCEINLINE void generate_opengl_globals()
{
	static bool first = true;
	if (first) first = false;
	else return;

	for (int i = 0; i < (int)ShaderUsage::_Max; i++)
	{
		DefaultShaders[i] = Shader::get_default(ShaderUsage(i));
	}

	DefaultCubeIndexBuffer.generate(sizeof(DefaultCubeIndexBuffer_Values) / sizeof(*DefaultCubeIndexBuffer_Values), DefaultCubeIndexBuffer_Values);
	DefaultLineBuffer.create(2u);
	DefaultCubeBuffer.create(36u);

	g_Quad2DBuffer.set_primitive(PrimitiveType::Quad);
	g_Triangle2DBuffer.set_primitive(PrimitiveType::Triangle);
	g_Line2DBuffer.set_primitive(PrimitiveType::Line);
	DefaultCubeBuffer.set_primitive(PrimitiveType::Triangle);

	g_Quad2DBuffer.create(4);
	g_Triangle2DBuffer.create(3);
	g_Line2DBuffer.create(2);


	// uvs
	{
		g_Quad2DVertcies[ 0 ].uv = { 0.f, 0.f };
		g_Quad2DVertcies[ 1 ].uv = { 0.f, 1.f };
		g_Quad2DVertcies[ 2 ].uv = { 1.f, 1.f };
		g_Quad2DVertcies[ 3 ].uv = { 1.f, 0.f };
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
		g_PlankTexture.reset(new Texture(Image(plank_white_data, {2, 2}, ColorFormat::L)));
	}

	ig::_font_init();
}

namespace ig
{
	


	Canvas::Canvas(const Window &wnd)
		: m_wnd{ wnd }, m_shader{ 0 }, m_transform2d{}, m_transform3d{}
	{
		generate_opengl_globals();

		if (g_CurrentCanvas)
			bite::warn("More then on instances of ig::Canvas exists! you can't render on window/canvas while a canvas is alive!");
		g_CurrentCanvas = this;

		this->unbind_shader();
		update_camera();
		set_draw_type( m_draw_type );
	}

	Canvas::~Canvas()
	{
		glUseProgram(0);
		g_CurrentCanvas = nullptr;
	}

	void Canvas::set_draw_type( const DrawType type )
	{
		m_draw_type = type;

		switch (type)
		{
		case DrawType::Drawing2D:
			m_shading_usage = ShaderUsage::Usage2D;

			glDisable( GL_DEPTH_TEST );
			glDisable( GL_CULL_FACE );

			break;
		case DrawType::Drawing3D:
			m_shading_usage = ShaderUsage::Usage3D;
			glEnable( GL_DEPTH_TEST );
			glEnable( GL_CULL_FACE );

			glFrontFace( GL_CCW );
			glCullFace( GL_FRONT );

			break;
		case DrawType::DirectDrawing:
			m_shading_usage = ShaderUsage::ScreenSpace;

			glDisable( GL_DEPTH_TEST );
			glDisable( GL_CULL_FACE );
			break;
		case DrawType::Raw:
			m_shading_usage = ShaderUsage::ScreenSpace;

			glDisable( GL_DEPTH_TEST );
			glDisable( GL_CULL_FACE );
			break;
		default:
			break;
		}
	}

	void Canvas::quad(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, const Colorf &clr)
	{
		const Vector2f wf = m_wnd.size();
		g_Quad2DVertcies[ 0 ].pos = to_clamped_space(p0, wf);
		g_Quad2DVertcies[ 1 ].pos = to_clamped_space(p1, wf);
		g_Quad2DVertcies[ 2 ].pos = to_clamped_space(p2, wf);
		g_Quad2DVertcies[ 3 ].pos = to_clamped_space(p3, wf);

		g_Quad2DVertcies[ 0 ].clr = clr;
		g_Quad2DVertcies[ 1 ].clr = clr;
		g_Quad2DVertcies[ 2 ].clr = clr;
		g_Quad2DVertcies[ 3 ].clr = clr;

		g_Quad2DBuffer.update(g_Quad2DVertcies);
		draw(g_Quad2DBuffer);
	}

	void Canvas::rect(Vector2f start, Vector2f end, const Colorf &clr)
	{
		quad(start, { end.x, start.y }, end, { start.x, end.y }, clr);
	}

	void Canvas::traingle(Vector2f p0, Vector2f p1, Vector2f p2, const Colorf &clr)
	{
		g_Triangle2DVertcies[ 0 ].pos = to_clamped_space(p0, m_wnd.size());
		g_Triangle2DVertcies[ 1 ].pos = to_clamped_space(p1, m_wnd.size());
		g_Triangle2DVertcies[ 2 ].pos = to_clamped_space(p2, m_wnd.size());

		g_Triangle2DVertcies[ 0 ].clr = clr;
		g_Triangle2DVertcies[ 1 ].clr = clr;
		g_Triangle2DVertcies[ 2 ].clr = clr;

		g_Triangle2DBuffer.update(g_Triangle2DVertcies);
		draw(g_Triangle2DBuffer);
	}

	void Canvas::line(Vector2f start, Vector2f end, const Colorb clr)
	{
		line(start, end, 1.0f, clr);
	}

	void Canvas::line(Vector2f start, Vector2f end, float_t width, const Colorb clr)
	{
		start = to_clamped_space(start, m_wnd.size());
		end = to_clamped_space(end, m_wnd.size());

		glBegin(GL_LINES);
		glLineWidth(width);
		glColor4ub(clr.r, clr.g, clr.b, clr.a);
		glVertex2f(start.x, start.y);
		glVertex2f(end.x, end.y);
		glEnd();
	}

	void Canvas::line(Vector3f start, Vector3f end, const Colorf &clr)
	{
		static Vertex3D ff[ 2 ]{};

		ff[ 0 ].pos = start;
		ff[ 1 ].pos = end;
		ff[ 0 ].clr = clr;
		ff[ 1 ].clr = clr;

		DefaultLineBuffer.update(ff);
		draw(DefaultLineBuffer);
	}

	void Canvas::cube(Vector3f start, Vector3f end, const Colorf &clr)
	{
		static GLfloat g_vertex_buffer_data[] = {
			-1.0f,-1.0f,-1.0f, // triangle 1 : begin
			-1.0f,-1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f, // triangle 1 : end
			1.0f, 1.0f,-1.0f, // triangle 2 : begin
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f,-1.0f, // triangle 2 : end
			1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f,-1.0f,
			1.0f,-1.0f,-1.0f,
			1.0f, 1.0f,-1.0f,
			1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f,-1.0f,
			1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f,-1.0f, 1.0f,
			1.0f,-1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f,-1.0f,-1.0f,
			1.0f, 1.0f,-1.0f,
			1.0f,-1.0f,-1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f,-1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f,-1.0f,
			-1.0f, 1.0f,-1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f,-1.0f, 1.0f
		};

		const float Uvs[]
		{
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
		};

		Vertex3D v[ sizeof(g_vertex_buffer_data) / (sizeof(GLfloat) * 3) ];
		for (size_t i = 0; i < 36; i++)
		{
			v[ i ].pos.set(g_vertex_buffer_data[ i * 3 ], g_vertex_buffer_data[ (i * 3) + 1 ], g_vertex_buffer_data[ (i * 3) + 2 ]);
			v[ i ].pos = v[ i ].pos.rotated(Vector3f{ 0.f, 1.f, 0.f }, (Pi / 2.0f) + (float)glfwGetTime());
			//v[ i ].pos /= 0.25f;
			v[ i ].pos += start;
			v[ i ].clr = clr;
			v[ i ].uv.x = Uvs[ (i * 2) % 6 ];
			v[ i ].uv.y = Uvs[ ((i * 2) + 1) % 6 ];
		}
		DefaultCubeBuffer.update(v);

		draw(DefaultCubeBuffer);

		(void)end;
		(void)start;
	}

	void Canvas::draw(Vertex2D *vert, size_t count, PrimitiveType draw_type)
	{
		glBegin(to_glprimitve(draw_type));

		for (const Vertex2D &v : std::initializer_list<Vertex2D>(vert, vert + count))
			glVertex(v);

		glEnd();
	}

	void Canvas::circle(float radius, Vector2f center, const Colorf clr, const uint16_t res)
	{
		span<Vertex2D> verts( res );

		const auto &poly = get_circle_frame(res);
		for (size_t i = 0; i < poly.sz; i++)
		{
			verts[ i ].pos = Vector2f((poly.ptr.get()[ i ].x * radius) + center.x, (poly.ptr.get()[ i ].y * radius) + center.y);
			verts[ i ].clr = clr;
		}

		Vertex2DBuffer buff{};
		buff.set_primitive(PrimitiveType::TriangleFan);
		buff.create(res, verts.begin());
		draw(buff);
	}

	void Canvas::draw(const Vertex2DBuffer &buf, int start, int count)
	{
		update_shader_uniforms();

		buf._bind_array_buffer();

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(Vertex2DBuffer::vertex_type), (const void *)offsetof(Vertex2DBuffer::vertex_type, pos));
		glVertexAttribPointer(1, 4, GL_FLOAT, 0, sizeof(Vertex2DBuffer::vertex_type), (const void *)offsetof(Vertex2DBuffer::vertex_type, clr));
		glVertexAttribPointer(2, 2, GL_FLOAT, 0, sizeof(Vertex2DBuffer::vertex_type), (const void *)offsetof(Vertex2DBuffer::vertex_type, uv));

		glDrawArrays(to_glprimitve(buf.get_primitive()), start, count < 0 ? ((int)buf.size() - start) : count);
		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		if (!buf._unbind_array_buffer())
			raise("draw failed: unbind faild at vertex buffer becuse of possible race condition, unbinding the vertex 2d buffer mid process");
	}

	void Canvas::draw(const Vertex2DBuffer &buf, const IndexBuffer &indcies)
	{
		update_shader_uniforms();

		buf._bind_array_buffer();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indcies.get_id());

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(Vertex2DBuffer::vertex_type), (const void *)offsetof(Vertex2DBuffer::vertex_type, pos));
		glVertexAttribPointer(1, 4, GL_FLOAT, 0, sizeof(Vertex2DBuffer::vertex_type), (const void *)offsetof(Vertex2DBuffer::vertex_type, clr));
		glVertexAttribPointer(2, 2, GL_FLOAT, 0, sizeof(Vertex2DBuffer::vertex_type), (const void *)offsetof(Vertex2DBuffer::vertex_type, uv));

		glDrawElements(to_glprimitve(buf.get_primitive()), (int)buf.size(), GL_UNSIGNED_INT, nullptr);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
		if (!buf._unbind_array_buffer())
			raise("draw failed: unbind faild at vertex buffer becuse of possible race condition, unbinding the vertex 2d buffer mid process");
	}

	void Canvas::draw(const Vertex3DBuffer &buf, int start, int count)
	{
		update_shader_uniforms();

		buf._bind_array_buffer();

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(Vertex3DBuffer::vertex_type), (const void *)offsetof(Vertex3DBuffer::vertex_type, pos));
		glVertexAttribPointer(1, 4, GL_FLOAT, 0, sizeof(Vertex3DBuffer::vertex_type), (const void *)offsetof(Vertex3DBuffer::vertex_type, clr));
		glVertexAttribPointer(2, 2, GL_FLOAT, 0, sizeof(Vertex3DBuffer::vertex_type), (const void *)offsetof(Vertex3DBuffer::vertex_type, uv));
		glVertexAttribPointer(3, 3, GL_FLOAT, 0, sizeof(Vertex3DBuffer::vertex_type), (const void *)offsetof(Vertex3DBuffer::vertex_type, normal));

		glDrawArrays(to_glprimitve(buf.get_primitive()), start, count < 0 ? ((int)buf.size() - start) : count);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);

		if (!buf._unbind_array_buffer())
			raise("draw failed: unbind faild at vertex buffer becuse of possible race condition, unbinding the vertex 2d buffer mid process");
	}

	void Canvas::bind_shader(const ShaderInstance_t &shader)
	{
		if (m_shader.get() == shader.get())
			return;

		if (!shader)
		{
			bite::warn("trying to bind a NULL shader to canvas!");
			return;
		}

		glUseProgram(shader->get_id());
		m_shader = shader;
	}

	void Canvas::unbind_shader()
	{
		glUseProgram(DefaultShaders[ (int)m_shading_usage ]->get_id());
		m_shader = DefaultShaders[ (int)m_shading_usage ];
	}

	//void Canvas::set_shading_usage(const ShaderUsage usage)
	//{
	//	if (usage == ShaderUsage::_Max)
	//	{
	//		bite::warn("the shading usage ShaderUsage::_Max is not valid!");
	//		return;
	//	}
	//	m_shading_usage = usage;
	//}

	ShaderId_t Canvas::get_shader_id() const noexcept
	{
		return m_shader->get_id();
	}

	void Canvas::update_shader_uniforms()
	{
		if (!m_shader)
			return;

		const int atc = m_active_textrues_count;
		for (int i = 0; i < atc; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			const auto hdl = m_textures[ i ] ? m_textures[ i ] : g_PlankTexture->get_handle();
			glBindTexture(GL_TEXTURE_2D, hdl);
		}

		glUniform2f(glGetUniformLocation(m_shader->get_id(), "_screensize"), (float)m_wnd.width(), (float)m_wnd.height());

		if (m_shader->get_usage() == ShaderUsage::Usage2D)
		{
			glUniformMatrix2fv(glGetUniformLocation(m_shader->get_id(), "_trans"), 1, GL_FALSE, m_transform2d.get_data().f);
			glUniform2f(glGetUniformLocation(m_shader->get_id(), "_offset"), m_transform2d.get_data().origin.x, m_transform2d.get_data().origin.y);
		}
		else if (m_shader->get_usage() == ShaderUsage::Usage3D)
		{
			glUniform3f(glGetUniformLocation(m_shader->get_id(), "_offset"), m_transform3d.get_data().origin.x, m_transform3d.get_data().origin.y, m_transform3d.get_data().origin.z);
			glUniformMatrix3fv(glGetUniformLocation(m_shader->get_id(), "_trans"), 1, GL_TRUE, m_transform3d.get_data().f);
			glUniformMatrix4fv(glGetUniformLocation(m_shader->get_id(), "_proj"), 1, GL_TRUE, m_camera_cache.m_proj_matrix.m_elements.data());
			glUniformMatrix3fv(glGetUniformLocation(m_shader->get_id(), "_view_transform"), 1, GL_TRUE, m_camera.transform.get_data().f);
			glUniform3fv(glGetUniformLocation(m_shader->get_id(), "_view_position"), 1, m_camera.transform.get_data().f + 9);
		}

	}

	const Window &Canvas::get_window() const
	{
		return m_wnd;
	}


	void Canvas::set_texture(const TextureId_t tex, const TextureSlot slot)
	{
		m_textures[ int(slot) ] = tex;
	}

	TextureId_t ig::Canvas::get_texture(const TextureSlot slot) const noexcept
	{
		return m_textures[ int(slot) ];
	}

	void Canvas::set_active_textures_count(int count)
	{
		if (count < 0)
		{
			bite::warn("can't set the active textures count to less then zero");
			return;
		}

		if (count >= int(TextureSlot::_MAX))
		{
			bite::warn("can't set the active textures count to greater or equal to zero");
			return;
		}

		m_active_textrues_count = count;
	}

	int Canvas::get_active_textures_count() const noexcept
	{
		return m_active_textrues_count;
	}

	Camera &Canvas::camera()
	{
		return m_camera;
	}

	const Camera &Canvas::camera() const
	{
		return m_camera;
	}

	void Canvas::update_camera()
	{
		m_camera_cache.m_proj_matrix = m_camera.projection();
	}

	void Canvas::set_shader_uniform(int location, int value)
	{
		glUniform1i(location, value);
	}

	void Canvas::set_shader_uniform(int location, float value)
	{
		glUniform1f(location, value);
	}

	void Canvas::set_shader_uniform(int location, Vector2i value)
	{
		glUniform2i(location, value.x, value.y);
	}

	void Canvas::set_shader_uniform(int location, Vector2f value)
	{
		glUniform2f(location, value.x, value.y);
	}

	void Canvas::set_shader_uniform(int location, Vector3i value)
	{
		glUniform3i(location, value.x, value.y, value.z);
	}

	void Canvas::set_shader_uniform(int location, Vector3f value)
	{
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Canvas::set_shader_uniform(int location, const Vector4i &value)
	{
		glUniform4i(location, value.x, value.y, value.z, value.w);
	}

	void Canvas::set_shader_uniform(int location, const Vector4f &value)
	{
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void Canvas::set_shader_uniform(int location, int count, const int *value)
	{
		glUniform1iv(location, count, value);
	}

	void Canvas::set_shader_uniform(int location, int count, const float *value)
	{
		glUniform1fv(location, count, value);
	}

	void Canvas::set_shader_uniform(int location, int count, const Vector2i *value)
	{
		glUniform2iv(location, count, (const GLint *)value);
	}

	void Canvas::set_shader_uniform(int location, int count, const Vector2f *value)
	{
		glUniform2fv(location, count, (const GLfloat *)value);
	}

	void Canvas::set_shader_uniform(int location, int count, const Vector3i *value)
	{
		glUniform3iv(location, count, (const GLint *)value);
	}

	void Canvas::set_shader_uniform(int location, int count, const Vector3f * value)
	{
		glUniform3fv(location, count, (const GLfloat *)value);
	}

	void Canvas::set_shader_uniform(int location, int count, const Vector4i *value)
	{
		glUniform4iv(location, count, (const GLint *)value);
	}

	void Canvas::set_shader_uniform(int location, int count, const Vector4f *value)
	{
		glUniform4fv(location, count, (const GLfloat *)value);
	}

}

