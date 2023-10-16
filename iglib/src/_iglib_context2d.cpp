#include "pch.h"
#include "_iglib_window.h"
#include "_iglib_context2d.h"
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

	if (res < 3)
		return Vector2fSpan_t{};

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

namespace ig
{
	Context2D::Context2D(const Window &wnd)
		: m_wnd{ wnd }
	{
	}

	void Context2D::quad(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, const Colorb clr)
	{
		const Vector2f wf = m_wnd.get_size();
		p0 = to_clamped_space(p0, wf);
		p1 = to_clamped_space(p1, wf);
		p2 = to_clamped_space(p2, wf);
		p3 = to_clamped_space(p3, wf);

		glBegin(GL_QUADS);
		glColor4ub(clr.r, clr.g, clr.b, clr.a);
		glVertex2f(p0.x, p0.y);
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
		glVertex2f(p3.x, p3.y);
		glEnd();

		//vector2_qbuffer vq{};
		//to_clamped_space(vq, m_wnd.get_size());

		//glBegin(GL_QUADS);
		//glColor4ub(clr.r, clr.g, clr.b, clr.a);
		//glVertex2f(vq.v[ 0 ].x, vq.v[ 0 ].y);
		//glVertex2f(vq.v[ 1 ].x, vq.v[ 1 ].y);
		//glVertex2f(vq.v[ 2 ].x, vq.v[ 2 ].y);
		//glVertex2f(vq.v[ 3 ].x, vq.v[ 3 ].y);
		//glEnd();
	}

	void Context2D::rect(Vector2f start, Vector2f end, const Colorb clr)
	{
		start = to_clamped_space(start, m_wnd.get_size());
		end = to_clamped_space(end, m_wnd.get_size());

		glColor4ub(clr.r, clr.g, clr.b, clr.a);
		glRectf(start.x, start.y, end.x, end.y);
	}

	void Context2D::traingle(Vector2f p0, Vector2f p1, Vector2f p2, const Colorb clr)
	{
		p0 = to_clamped_space(p0, m_wnd.get_size());
		p1 = to_clamped_space(p1, m_wnd.get_size());
		p2 = to_clamped_space(p2, m_wnd.get_size());

		glBegin(GL_TRIANGLES);
		glColor4b(clr.r, clr.g, clr.b, clr.a);
		glVertex2f(p0.x, p0.y);
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
		glEnd();
	}

	void Context2D::line(Vector2f start, Vector2f end, const Colorb clr)
	{
		line(start, end, 1.0f, clr);
	}

	void Context2D::line(Vector2f start, Vector2f end, float_t width, const Colorb clr)
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

	void Context2D::traingle_strips(const vector2f_buffer_view_t points, const Colorb clr)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3(clr);

		for (const Vector2f &v : points)
			glVertex2f(v.x, v.y);

		glEnd();
	}

	void Context2D::vertecies(Vertex *vert, size_t count, VertexDrawType draw_type)
	{
		glBegin(to_gldraw_type(draw_type));

		for (const Vertex &v : std::initializer_list<Vertex>(vert, vert + count))
			glVertex(v);

		glEnd();
	}

	void Context2D::circle(float radius, Vector2f center, const Colorb clr, const uint16_t res)
	{
		glBegin(GL_TRIANGLE_FAN);
		glColor3(clr);

		for (const auto &v : get_circle_frame(res))
			glVertex2f((v.x * radius) + center.x, (v.y * radius) + center.y);

		glEnd();
	}

	void Context2D::demo()
	{
		glEnable(GL_TEXTURE_2D);

		//glClear(GL_COLOR_BUFFER_BIT);   // Clear the color buffer with current clearing color

		GLbyte tex[ 64 * 64 ];
		for (size_t x{}; x < 64; x++)
		{
		  for (size_t y{}; y < 64; y++)
		  {
				tex[ x + (y * 64) ] = GLbyte(x + y);
		  }
		}

		static GLfloat pixels[] =
		{
				1, 0, 0,
				0, 1, 0,
				0, 0, 1,
				1, 1, 1
		};

		GLuint t;
		glGenTextures(1, &t);
		glBindTexture(GL_TEXTURE0, t);
		glTexImage2D(GL_TEXTURE0, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
		glTexParameteri(GL_TEXTURE0, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE0, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE0, 0);


		// Define shapes enclosed within a pair of glBegin and glEnd
		glBindTexture(GL_TEXTURE0, t);
		glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
		glColor3f(1.0f, 1.0f, 1.0f);

		glTexCoord2f(0.0, 0.0);
		glVertex2f(0.0, 0.0);
		glTexCoord2f(1.0, 0.0);
		glVertex2f(100.0, 0.0);
		glTexCoord2f(1.0, 1.0);
		glVertex2f(100.0, 100.0);
		glTexCoord2f(0.0, 1.0);
		glVertex2f(0.0, 100.0);

		//glColor3f(1.0f, 0.0f, 0.0f); // Red
		//glTexCoord2f(0.0f, 0.0f);
		//glVertex2i(128 + 32, 32);     // Define vertices in counter-clockwise (CCW) order

		////glColor3f(0.0f, 1.0f, 0.0f); // green
		//glTexCoord2f(1.0f, 0.0f);
		//glVertex2i(32, 32);     //  so that the normal (front-face) is facing you
		////glColor3f(0.0f, 0.0f, 1.0f); // blue
		//glTexCoord2f(1.0f, 1.0f);
		//glVertex2i(32, 32 + 128);
		////glColor3f(0.5f, 0.5f, 0.5f); // gray
		//glTexCoord2f(0.0f, 1.0f);
		//glVertex2f( 32 + 128, 32 + 128);

		glDisable(GL_TEXTURE_2D);
		glEnd();

		//glColor3f(0.0f, 1.0f, 0.0f); // Green
		//glVertex2f(-0.7f, -0.6f);
		//glVertex2f(-0.1f, -0.6f);
		//glVertex2f(-0.1f, 0.0f);
		//glVertex2f(-0.7f, 0.0f);

		//glColor3f(0.2f, 0.2f, 0.2f); // Dark Gray
		//glVertex2f(-0.9f, -0.7f);
		//glColor3f(1.0f, 1.0f, 1.0f); // White
		//glVertex2f(-0.5f, -0.7f);
		//glColor3f(0.2f, 0.2f, 0.2f); // Dark Gray
		//glVertex2f(-0.5f, -0.3f);
		//glColor3f(1.0f, 1.0f, 1.0f); // White
		//glVertex2f(-0.9f, -0.3f);
		//glEnd();

		glBegin(GL_TRIANGLES);          // Each set of 3 vertices form a triangle
		glColor3f(0.0f, 0.0f, 1.0f); // Blue
		//glVertex2f(0.1f, -0.6f);
		//glVertex2f(0.7f, -0.6f);
		//glVertex2f(0.4f, -0.1f);
		//glVertex2i(64, 32);
		//glVertex2i(90, 32);
		//glVertex2i(70, 60);

		glVertex2i(64, 32);
		glVertex2i(70, 60);
		glVertex2i(90, 32);

		glVertex2i(64, -32);
		glVertex2i(90, -32);
		glVertex2i(70, -60);

		glVertex2i(64, -32);
		glVertex2i(70, -60);
		glVertex2i(90, -32);

		glVertex2i(-1, 0);
		glVertex2i(3, -1);
		glVertex2i(3, 32);

		//glVertex2i(0, 0);
		//glVertex2i(1, -1);
		//glVertex2i(-1, 1);


		glColor3f(1.0f, 0.0f, 0.0f); // Red
		glVertex2f(0.3f, -0.4f);
		glColor3f(0.0f, 1.0f, 0.0f); // Green
		glVertex2f(0.9f, -0.4f);
		glColor3f(0.0f, 0.0f, 1.0f); // Blue
		glVertex2f(0.6f, -0.9f);
		glEnd();

		glBegin(GL_POLYGON);            // These vertices form a closed polygon
		glColor3f(1.0f, 1.0f, 0.0f); // Yellow
		glVertex2f(0.4f, 0.2f);
		glVertex2f(0.6f, 0.2f);
		glVertex2f(0.7f, 0.4f);
		glVertex2f(0.6f, 0.6f);
		glVertex2f(0.4f, 0.6f);
		glVertex2f(0.3f, 0.4f);
		glEnd();

		glFlush();  // Render now
	}

	const Window &Context2D::get_window() const
	{
		return m_wnd;
	}

}

