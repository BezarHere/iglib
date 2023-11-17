#include "pch.h"
#include "_iglib_window.h"
#include "_iglib_canvas.h"
#include "internal.h"
#include "intrinsics.h"
#include "draw_internal.h"

FORCEINLINE [[nodiscard]] Vector2f to_clamped_space(const Vector2f &val, const Vector2f wf)
{
	(void)wf;
	return val;
	//return Vector2f((val.x * 2.0f / wf.x) - 1.0f, -((val.y * 2.0f / wf.y) - 1.0f));
}

FORCEINLINE [[nodiscard]] Vector2f to_clamped_size(const Vector2f &val, const Vector2f wf)
{
	(void)wf;
	return val;
	//return Vector2f((val.x * 2.0f / wf.x), -((val.y * 2.0f / wf.y)));
}


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

union vector2_qbuffer
{
	static constexpr size_t size = 4;
	static constexpr size_t fsize = size * 2;
	static constexpr size_t bits_count = size * sizeof(Vector2f) * 8;
	static_assert(bits_count == 256, "");
	
	vector2_qbuffer(const Vector2f &v0, const Vector2f &v1, const Vector2f &v2, const Vector2f &v3)
		: v{ v0, v1, v2, v3 }
	{
	}

	vector2_qbuffer()
		: v{}
	{}

	Vector2f v[ size ];
	float_t fv[ size * 2 ];
	float_t f;
};
static_assert(sizeof(vector2_qbuffer) == sizeof(__m256), "");

//__forceinline void to_clamped_space(Vector2f &val, const Window &wnd)
//{
//  const Vector2f wf{ wnd.get_size() };
//  val.x = (val.x * 2.0f / wf.x) - 1.0f;
//  val.y = -((val.y * 2.0f / wf.y) - 1.0f);
//}

//__forceinline void to_clamped_size(Vector2f &val, const Window &wnd)
//{
//  const Vector2f wf{ wnd.get_size() };
//  val.x = (val.x * 2.0f / wf.x);
//  val.y = -((val.y * 2.0f / wf.y));
//}

// Packed __m256 are structured as:
//    x0|x1|x2|x3|y0|y1|y2|y3
__forceinline __m256 pack_vector2_q(const float_t *const fv)
{
#if 0
	const float_t *src = &qbuffer.f;
	float dst[ vector2_qbuffer::fsize ];
	for (size_t i{}; i < vector2_qbuffer::fsize; i++)
	{
		if (i & 0x1) // y-axis (odd)
			dst[ (i >> 1) + 4 ] = src[ i ];
		else // x-axis (even)
			dst[ i >> 1 ] = src[ i ];
	}
	return _mm256_load_ps(dst);
#endif
	return _mm256_set_ps(fv[ 0 ], fv[ 2 ], fv[ 4 ], fv[ 6 ],
											 fv[ 1 ], fv[ 3 ], fv[ 5 ], fv[ 7 ]);
}

// Packed __m256 are structured as:
//    x0|x1|x2|x3|y0|y1|y2|y3
__forceinline void unpack_vector2_q(const __m256 mm256, float_t *const fv)
{
	fv[ 0 ] = mm256.m256_f32[ 0 ];
	fv[ 2 ] = mm256.m256_f32[ 2 ];
	fv[ 4 ] = mm256.m256_f32[ 4 ];
	fv[ 6 ] = mm256.m256_f32[ 6 ];
	fv[ 1 ] = mm256.m256_f32[ 1 ];
	fv[ 3 ] = mm256.m256_f32[ 3 ];
	fv[ 5 ] = mm256.m256_f32[ 5 ];
	fv[ 7 ] = mm256.m256_f32[ 7 ];
}

__forceinline [[nodiscard]] __m256 to_clamped_space(__m256 val, __m256 wnd_size)
{
	const __m256 original = val;

	// val *= 2.0f
	val = _mm256_mul_ps(val, two256);

	// val /= wnd_size
	val = _mm256_div_ps(val, wnd_size);

	// val -= 1.0f
	val = _mm256_sub_ps(val, one256);

	// val.y = -val.y
	{
		__m128 *ptr128_y = ((__m128 *) & val) + 1;
		// val_y = val.y
		__m128 val_y = *ptr128_y;

		// val_y = 0 - val_y # negation
		val_y = _mm_sub_ps(zero128, val_y);

		// val.y = val_y
		*ptr128_y = val_y;
	}
	return val;
}

__forceinline [[nodiscard]] void to_clamped_space(vector2_qbuffer &val, const Vector2f wf)
{
	// idk why flipping the x and y fixes some stretch proplems
	const __m256 wf256 = _mm256_set_ps(wf.y, wf.y, wf.y, wf.y, wf.x, wf.x, wf.x, wf.x);
	unpack_vector2_q(to_clamped_space(pack_vector2_q(val.fv), wf256), val.fv);
}


static ShaderInstance_t DefaultShader2D{};
static ShaderInstance_t DefaultShader3D{};

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

FORCEINLINE void generate_opengl_globals()
{
	static bool first = true;
	if (first) first = false;
	else return;

	DefaultShader2D = Shader::get_default(ShaderUsage::Usage2D);
	DefaultShader3D = Shader::get_default(ShaderUsage::Usage3D);

	DefaultCubeIndexBuffer.generate(sizeof(DefaultCubeIndexBuffer_Values) / sizeof(*DefaultCubeIndexBuffer_Values), DefaultCubeIndexBuffer_Values);
	DefaultLineBuffer.create(2u);
	DefaultCubeBuffer.create(36u);

	g_Quad2DBuffer.set_primitive(PrimitiveType::Quad);
	g_Triangle2DBuffer.set_primitive(PrimitiveType::Triangle);
	g_Line2DBuffer.set_primitive(PrimitiveType::Line);

	g_Quad2DBuffer.create(4);
	g_Triangle2DBuffer.create(3);
	g_Line2DBuffer.create(2);


	{
		g_Quad2DVertcies[ 0 ].uv = { 0.f, 0.f };
		g_Quad2DVertcies[ 1 ].uv = { 0.f, 1.f };
		g_Quad2DVertcies[ 2 ].uv = { 1.f, 1.f };
		g_Quad2DVertcies[ 3 ].uv = { 1.f, 0.f };
	}
}

namespace ig
{
	Canvas::Canvas(const Window &wnd)
		: m_wnd{ wnd }, m_shader{ 0 }, m_tex{ 0 }, m_trans2d{}, m_trans3d{}
	{
		generate_opengl_globals();
			
		this->unbind_shader();
	}

	Canvas::~Canvas()
	{
		glUseProgram(0);
	}

	void Canvas::quad(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, const Colorf &clr)
	{
		const Vector2f wf = m_wnd.get_size();
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
		quad(start, { start.x, end.y }, end, { end.x, start.y }, clr);
	}

	void Canvas::traingle(Vector2f p0, Vector2f p1, Vector2f p2, const Colorf &clr)
	{
		g_Triangle2DVertcies[ 0 ].pos = to_clamped_space(p0, m_wnd.get_size());
		g_Triangle2DVertcies[ 1 ].pos = to_clamped_space(p1, m_wnd.get_size());
		g_Triangle2DVertcies[ 2 ].pos = to_clamped_space(p2, m_wnd.get_size());

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
		start = to_clamped_space(start, m_wnd.get_size());
		end = to_clamped_space(end, m_wnd.get_size());

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
			v[ i ].pos += Vector3f{ 1.0f, 1.0f, 1.0f };
			v[ i ].pos *= 200.0f;
			//v[ i ].pos = v[ i ].pos.rotated(Vector3f{ 1.f, 0.f, 1.f }, Pi / 2.0f);
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

	void Canvas::draw(const Vertex2DBuffer &buf)
	{
		//if (m_shader && m_shader->get_usage() == ShaderUsage::Usage2D)
		if (m_shader)
		{
			update_shader_uniforms();
		}
		else
		{
			bind_shader(DefaultShader2D);
			update_shader_uniforms();
		}

		buf._bind_array_buffer();

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(Vertex2DBuffer::vertex_type), (const void *)offsetof(Vertex2DBuffer::vertex_type, pos));
		glVertexAttribPointer(1, 4, GL_FLOAT, 0, sizeof(Vertex2DBuffer::vertex_type), (const void *)offsetof(Vertex2DBuffer::vertex_type, clr));
		glVertexAttribPointer(2, 2, GL_FLOAT, 0, sizeof(Vertex2DBuffer::vertex_type), (const void *)offsetof(Vertex2DBuffer::vertex_type, uv));
		
		glDrawArrays(to_glprimitve(buf.get_primitive()), 0, (int)buf.get_size());
		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		

		if (!buf._unbind_array_buffer())
			raise("draw failed: unbind faild at vertex buffer becuse of possible race condition, unbinding the vertex 2d buffer mid process");
	}

	void Canvas::draw(const Vertex2DBuffer &buf, const IndexBuffer &indcies)
	{
		//if (m_shader && m_shader->get_usage() == ShaderUsage::Usage2D)
		if (m_shader)
		{
			update_shader_uniforms();
		}
		else
		{
			bind_shader(DefaultShader2D);
			update_shader_uniforms();
		}

		buf._bind_array_buffer();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indcies.get_id());

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(Vertex2DBuffer::vertex_type), (const void *)offsetof(Vertex2DBuffer::vertex_type, pos));
		glVertexAttribPointer(1, 4, GL_FLOAT, 0, sizeof(Vertex2DBuffer::vertex_type), (const void *)offsetof(Vertex2DBuffer::vertex_type, clr));
		glVertexAttribPointer(2, 2, GL_FLOAT, 0, sizeof(Vertex2DBuffer::vertex_type), (const void *)offsetof(Vertex2DBuffer::vertex_type, uv));

		glDrawElements(to_glprimitve(buf.get_primitive()), (int)buf.get_size(), GL_UNSIGNED_INT, nullptr);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
		if (!buf._unbind_array_buffer())
			raise("draw failed: unbind faild at vertex buffer becuse of possible race condition, unbinding the vertex 2d buffer mid process");
	}

	void Canvas::draw(const Vertex3DBuffer &buf)
	{
		//if (m_shader && m_shader->get_usage() == ShaderUsage::Usage3D)
		if (m_shader)
		{
			update_shader_uniforms();
		}
		else
		{
			bind_shader(DefaultShader3D);
			update_shader_uniforms();
		}

		buf._bind_array_buffer();

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(Vertex3DBuffer::vertex_type), (const void *)offsetof(Vertex3DBuffer::vertex_type, pos));
		glVertexAttribPointer(1, 4, GL_FLOAT, 0, sizeof(Vertex3DBuffer::vertex_type), (const void *)offsetof(Vertex3DBuffer::vertex_type, clr));
		glVertexAttribPointer(2, 2, GL_FLOAT, 0, sizeof(Vertex3DBuffer::vertex_type), (const void *)offsetof(Vertex3DBuffer::vertex_type, uv));
		glVertexAttribPointer(3, 3, GL_FLOAT, 0, sizeof(Vertex3DBuffer::vertex_type), (const void *)offsetof(Vertex3DBuffer::vertex_type, normal));

		glDrawArrays(to_glprimitve(buf.get_primitive()), 0, (int)buf.get_size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);

		if (!buf._unbind_array_buffer())
			raise("draw failed: unbind faild at vertex buffer becuse of possible race condition, unbinding the vertex 2d buffer mid process");
	}

	void Canvas::demo()
	{
		//glEnable(GL_TEXTURE_2D);

		////glClear(GL_COLOR_BUFFER_BIT);   // Clear the color buffer with current clearing color

		//GLbyte tex[ 64 * 64 ];
		//for (size_t x{}; x < 64; x++)
		//{
		//  for (size_t y{}; y < 64; y++)
		//  {
		//		tex[ x + (y * 64) ] = GLbyte(x + y);
		//  }
		//}

		//static GLfloat pixels[] =
		//{
		//		1, 0, 0,
		//		0, 1, 0,
		//		0, 0, 1,
		//		1, 1, 1
		//};

		//GLuint t;
		//glGenTextures(1, &t);
		//glBindTexture(GL_TEXTURE0, t);
		//glTexImage2D(GL_TEXTURE0, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
		//glTexParameteri(GL_TEXTURE0, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE0, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glBindTexture(GL_TEXTURE0, 0);


		//// Define shapes enclosed within a pair of glBegin and glEnd
		//glBindTexture(GL_TEXTURE0, t);

		constexpr Colorf White = { 1.f, 1.f, 1.f, 1.f };
		constexpr Colorf Red = { 1.f, 0.f, 0.f, 1.f };
		{
			Vertex2D vert[ 8 ]
			{
				{ Vector2f{-80.f, 80.f}, White, Vector2f{ 0.f, 0.f } },
				{ Vector2f{-80.f, -80.f}, White, Vector2f{ 0.f, 1.f } },
				{ Vector2f{80.f, -80.f}, White, Vector2f{ 1.f, 1.f } },
				{ Vector2f{80.f, 80.f}, White, Vector2f{ 1.f, 0.f } },
				{ Vector2f{80.f, 80.f}, White, Vector2f{ 0.f, 0.f } },
				{ Vector2f{180.f, 80.f}, White, Vector2f{ 1.f, 0.f } },
				{ Vector2f{180.f, 180.f}, White, Vector2f{ 1.f, 1.f } },
				{ Vector2f{80.f, 180.f}, White, Vector2f{ 0.f, 1.f } },
			};

			//GLuint buffer;
			//glGenBuffers(1, &buffer);
			//glBindBuffer(GL_ARRAY_BUFFER, buffer);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

			//
			//glEnableVertexAttribArray(0);
			//glEnableVertexAttribArray(1);
			//glEnableVertexAttribArray(2);
			//glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(Vertex2D), nullptr);
			//glVertexAttribPointer(1, 4, GL_FLOAT, 0, sizeof(Vertex2D), (const void *)sizeof(Vector2f));
			//glVertexAttribPointer(2, 2, GL_FLOAT, 0, sizeof(Vertex2D), (const void *)(sizeof(Vector2f) + sizeof(Colorf)));


			Vertex2DBuffer buff{ 8 };
			buff.update(vert);
			buff.set_primitive(PrimitiveType::Quad);


			this->draw(buff);

			//glDrawArrays(GL_QUADS, 0, 8);

			/*if (!buffer._unbind_array_buffer())
				raise("draw failed: unbind faild at vertex buffer becuse of possible race condition, unbinding the vertex 2d buffer mid process");*/
			//glDeleteBuffers(1, &buffer);
		}
		
		
		{
			Vertex3D vert[ 6 ]
			{
				{ Vector3f{-1.f, -1.f, -1.f}, White, Vector2f{ 0.f, 0.f } },
				{ Vector3f{-1.f, 1.f, 1.f}, White, Vector2f{ 0.f, 1.f } },
				{ Vector3f{1.f, -1.f, -1.f}, White, Vector2f{ 1.f, 1.f } },
				{ Vector3f{-1.f, -1.f, -1.f}, White, Vector2f{ 1.f, 0.f } },
				{ Vector3f{-1.f, -1.f, -1.f}, White, Vector2f{ 0.f, 0.f } },
				{ Vector3f{-1.f, -1.f, -1.f}, White, Vector2f{ 1.f, 0.f } },
			};

			for (size_t i = 0; i < sizeof(vert) / sizeof(*vert); i++)
			{
				vert[ i ].pos *= 10.f;
				vert[ i ].pos += Vector3f{20.f, 20.f, 0.f};
			}



			//GLuint buffer;
			//glGenBuffers(1, &buffer);
			//glBindBuffer(GL_ARRAY_BUFFER, buffer);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

			//
			//glEnableVertexAttribArray(0);
			//glEnableVertexAttribArray(1);
			//glEnableVertexAttribArray(2);
			//glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(Vertex2D), nullptr);
			//glVertexAttribPointer(1, 4, GL_FLOAT, 0, sizeof(Vertex2D), (const void *)sizeof(Vector2f));
			//glVertexAttribPointer(2, 2, GL_FLOAT, 0, sizeof(Vertex2D), (const void *)(sizeof(Vector2f) + sizeof(Colorf)));


			Vertex3DBuffer buff{ 6 };
			buff.update(vert);
			buff.set_primitive(PrimitiveType::Triangle);


			this->draw(buff);

			//glDrawArrays(GL_QUADS, 0, 8);

			/*if (!buffer._unbind_array_buffer())
				raise("draw failed: unbind faild at vertex buffer becuse of possible race condition, unbinding the vertex 2d buffer mid process");*/
			//glDeleteBuffers(1, &buffer);
		}

		{
			Vertex3D vert[ 4 ]
			{
				{ Vector3f{-1.f, -1.f, -1.f}, White, Vector2f{ 0.f, 0.f } },
				{ Vector3f{-1.f, 1.f, 1.f}, White, Vector2f{ 0.f, 1.f } },
				{ Vector3f{1.f, -1.f, -1.f}, White, Vector2f{ 1.f, 0.f } },
				{ Vector3f{-1.f, -1.f, -1.f}, White, Vector2f{ 1.f, 1.f } },
			};

			for (size_t i = 0; i < sizeof(vert) / sizeof(*vert); i++)
			{
				vert[ i ].pos *= 10.f;
				vert[ i ].pos += Vector3f{ 20.f, 20.f, 0.f };
			}



			//GLuint buffer;
			//glGenBuffers(1, &buffer);
			//glBindBuffer(GL_ARRAY_BUFFER, buffer);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

			//
			//glEnableVertexAttribArray(0);
			//glEnableVertexAttribArray(1);
			//glEnableVertexAttribArray(2);
			//glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(Vertex2D), nullptr);
			//glVertexAttribPointer(1, 4, GL_FLOAT, 0, sizeof(Vertex2D), (const void *)sizeof(Vector2f));
			//glVertexAttribPointer(2, 2, GL_FLOAT, 0, sizeof(Vertex2D), (const void *)(sizeof(Vector2f) + sizeof(Colorf)));


			Vertex3DBuffer buff{ 4 };
			buff.update(vert);
			buff.set_primitive(PrimitiveType::Quad);


			this->draw(buff);

			//glDrawArrays(GL_QUADS, 0, 8);

			/*if (!buffer._unbind_array_buffer())
				raise("draw failed: unbind faild at vertex buffer becuse of possible race condition, unbinding the vertex 2d buffer mid process");*/
				//glDeleteBuffers(1, &buffer);
		}

		glFlush();  // Render now
	}

	void Canvas::bind_shader(const ShaderInstance_t &shader)
	{
		if (!shader || m_shader.get() == shader.get())
			return;
		glUseProgram(shader->get_id());

		//update_shader_uniforms();

		m_shader = shader;
	}

	void Canvas::unbind_shader()
	{
		glUseProgram(DefaultShader2D->get_id());
		m_shader = DefaultShader2D;
	}

	ShaderId_t Canvas::get_shader_id() const noexcept
	{
		return m_shader->get_id();
	}

	void Canvas::update_shader_uniforms()
	{
		if (!m_shader)
			return;

		glActiveTexture(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_tex);

		glUniform2f(glGetUniformLocation(m_shader->get_id(), "_screensize"), (float)m_wnd.get_width(), (float)m_wnd.get_height());
		//glUniform1f(location++, m_wnd.get_shader_time());

		if (m_shader->get_usage() == ShaderUsage::Usage2D)
		{
			glUniformMatrix2fv(glGetUniformLocation(m_shader->get_id(), "_trans"), 1, GL_FALSE, m_trans2d.get_data().f);
			glUniform2f(glGetUniformLocation(m_shader->get_id(), "_offset"), m_trans2d.get_data().origin.x, m_trans2d.get_data().origin.y);
		}
		else if (m_shader->get_usage() == ShaderUsage::Usage3D)
		{
			//glUniformMatrix3fv(glGetUniformLocation(m_shader->get_id(), "_trans"), 1, GL_FALSE, m_trans3d.get_data().f);
			glUniform3f(glGetUniformLocation(m_shader->get_id(), "_offset"), m_trans3d.get_data().origin.x, m_trans3d.get_data().origin.y, m_trans3d.get_data().origin.z);
			//glUniform3f(glGetUniformLocation(m_shader->get_id(), "_offset"), 0.f, 0.f, 0.f);
			constexpr float Identy[]
			{
				1.f, 0.f, 0.f,
				0.f, 1.f, 0.f,
				0.f, 0.f, 1.f,
			};
			glUniformMatrix3fv(glGetUniformLocation(m_shader->get_id(), "_trans"), 1, GL_TRUE, m_trans3d.get_data().f);
		}

	}

	const Window &Canvas::get_window() const
	{
		return m_wnd;
	}

	void Canvas::set_texture(const Texture &tex)
	{
		m_tex = tex.get_handle();
	}

	void ig::Canvas::set_texture(const TextureId_t tex)
	{
		m_tex = tex;
	}

	TextureId_t ig::Canvas::get_texture() const noexcept
	{
		return m_tex;
	}

}

