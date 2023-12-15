#include "pch.h"
#include "_iglib_renderer.h"
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
static Vertex3DBuffer g_Quad3DBuffer;
static Vertex2DBuffer g_Triangle2DBuffer;
static Vertex2DBuffer g_Line2DBuffer;

static Vertex2D g_Quad2DVertcies[ 4 ]{};
static Vertex3D g_Quad3DVertcies[ 4 ]{};
static Vertex2D g_Triangle2DVertcies[ 3 ]{};
static Vertex2D g_Line2DVertcies[ 2 ]{};

static const Canvas *g_CurrentCanvas;


FORCEINLINE void try_generate_opengl_globals()
{
	static bool first = true;
	if (first) first = false;
	else return;

	DefaultCubeIndexBuffer.generate(sizeof(DefaultCubeIndexBuffer_Values) / sizeof(*DefaultCubeIndexBuffer_Values), DefaultCubeIndexBuffer_Values);
	DefaultLineBuffer.create(2u);
	DefaultCubeBuffer.create(36u);

	g_Quad2DBuffer.set_primitive(PrimitiveType::Quad);
	g_Quad3DBuffer.set_primitive(PrimitiveType::Quad);
	g_Triangle2DBuffer.set_primitive(PrimitiveType::Triangle);
	g_Line2DBuffer.set_primitive(PrimitiveType::Line);
	DefaultCubeBuffer.set_primitive(PrimitiveType::Triangle);

	g_Quad2DBuffer.create(4);
	g_Quad3DBuffer.create(4);
	g_Triangle2DBuffer.create(3);
	g_Line2DBuffer.create(2);


	// uvs
	{
		g_Quad2DVertcies[ 0 ].uv = { 0.f, 0.f };
		g_Quad2DVertcies[ 1 ].uv = { 0.f, 1.f };
		g_Quad2DVertcies[ 2 ].uv = { 1.f, 1.f };
		g_Quad2DVertcies[ 3 ].uv = { 1.f, 0.f };
	}

}

namespace ig
{
	


	Canvas::Canvas(const Renderer &renderer)
		: m_renderer{ renderer }, m_transform2d{}, m_transform3d{}
	{
		try_generate_opengl_globals();
		update_camera();
	}

	Canvas::Canvas( Canvas &&move ) noexcept
		: m_renderer{ move.m_renderer }, m_transform2d{ move.m_transform2d }, m_transform3d{ move.m_transform3d }, m_camera{ move.m_camera }
	{
		update_camera();
	}

	Canvas::~Canvas()
	{
	}

	void Canvas::quad(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, const Colorf &clr)
	{
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

	void Canvas::quad( Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, const Colorf &clr )
	{
		g_Quad3DVertcies[ 0 ].pos = p0;
		g_Quad3DVertcies[ 1 ].pos = p1;
		g_Quad3DVertcies[ 2 ].pos = p2;
		g_Quad3DVertcies[ 3 ].pos = p3;

		g_Quad3DVertcies[ 0 ].clr = clr;
		g_Quad3DVertcies[ 1 ].clr = clr;
		g_Quad3DVertcies[ 2 ].clr = clr;
		g_Quad3DVertcies[ 3 ].clr = clr;

		g_Quad3DBuffer.update( g_Quad3DVertcies );
		draw( g_Quad3DBuffer );
	}

	void Canvas::quad( const Vector3f p[], const Colorf &clr )
	{
		g_Quad3DVertcies[ 0 ].pos = p[0];
		g_Quad3DVertcies[ 1 ].pos = p[1];
		g_Quad3DVertcies[ 2 ].pos = p[2];
		g_Quad3DVertcies[ 3 ].pos = p[3];

		g_Quad3DVertcies[ 0 ].clr = clr;
		g_Quad3DVertcies[ 1 ].clr = clr;
		g_Quad3DVertcies[ 2 ].clr = clr;
		g_Quad3DVertcies[ 3 ].clr = clr;

		g_Quad3DBuffer.update( g_Quad3DVertcies );
		draw( g_Quad3DBuffer );
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

	void Canvas::plane( Vector3f center, Vector2f extent, const Colorf &clr )
	{
		quad({ center.x + extent.x, center.y + extent.y, center.z },
			   { center.x + extent.x, center.y - extent.y, center.z },
			   { center.x - extent.x, center.y - extent.y, center.z },
			   { center.x - extent.x, center.y + extent.y, center.z },
				 clr );
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

	void Canvas::cube(Vector3f center, Vector3f extent, const Colorf &clr)
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
			v[ i ].pos *= extent;
			v[ i ].pos += center;
			v[ i ].clr = clr;
			v[ i ].uv.x = Uvs[ (i * 2) % 12 ];
			v[ i ].uv.y = Uvs[ ((i * 2) + 1) % 12 ];
		}
		DefaultCubeBuffer.update(v);

		draw(DefaultCubeBuffer);

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

	const Renderer &Canvas::get_renderer() const
	{
		return m_renderer;
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
		m_cam_cache = m_camera.projection(m_renderer.get_window().width() / float( m_renderer.get_window().height()));
	}

}

