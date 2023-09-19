#include "pch.h"
#include "_iglib_window.h"
#include "_iglib_context2d.h"

__forceinline [[nodiscard]] Vector2f to_clamped_space(const Vector2f &val, const Window &wnd)
{
  const Vector2f wf{ wnd.get_size() };
  return Vector2f((val.x * 2.0f / wf.x) - 1.0f, -((val.y * 2.0f / wf.y) - 1.0f));
}

__forceinline [[nodiscard]] Vector2f to_clamped_size(const Vector2f &val, const Window &wnd)
{
  const Vector2f wf{ wnd.get_size() };
  return Vector2f((val.x * 2.0f / wf.x), -((val.y * 2.0f / wf.y)));
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
__forceinline __m256 pack_vector2_q(const vector2_qbuffer &qbuffer)
{
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
}

// Packed __m256 are structured as:
//    x0|x1|x2|x3|y0|y1|y2|y3
__forceinline vector2_qbuffer unpack_vector2_q(__m256 mm256)
{
  vector2_qbuffer qb{};
  const float_t *src = (const float_t *)&mm256;
  for (size_t i{}; i < vector2_qbuffer::fsize; i++)
  {
    if (i & 0x1) // y-axis (odd)
      qb.fv[ i ] = src[ (i >> 1) + 4 ];
    else // x-axis (even)
      qb.fv[ i ] = src[ i >> 1 ];
  }
  return qb;
}

__forceinline [[nodiscard]] __m256 to_clamped_space(__m256 val, __m256 wnd_size)
{
  static const __m256 one = _mm256_set1_ps(1.0f);
  static const __m256 two = _mm256_set1_ps(2.0f);
  const auto original = val;

  // val *= 2.0f
  val = _mm256_mul_ps(val, two);

  // val /= wnd_size
  val = _mm256_div_ps(val, wnd_size);

  // val -= 1.0f
  val = _mm256_sub_ps(val, one);

  // val.y = -val.y
  {
    static const __m128 zero128 = _mm_set1_ps(0.0f);
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


__forceinline [[nodiscard]] vector2_qbuffer to_clamped_space(const vector2_qbuffer &val, const Window &wnd)
{
  const Vector2f wf{ wnd.get_size() };

  // idk why flipping the x and y fixes some stretch proplems
  const __m256 wf256 = _mm256_set_ps(wf.y, wf.y, wf.y, wf.y, wf.x, wf.x, wf.x, wf.x);
  const auto mmres = unpack_vector2_q(
    to_clamped_space(
      pack_vector2_q(val), wf256
    )
  );
  return mmres;
}

namespace ig
{
  Context2D::Context2D(const Window &wnd)
    : m_wnd{ wnd }
  {
  }

  void Context2D::quad(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, const Colorb clr)
  {
    //p0 = to_clamped_space(p0, m_wnd);
    //p1 = to_clamped_space(p1, m_wnd);
    //p2 = to_clamped_space(p2, m_wnd);
    //p3 = to_clamped_space(p3, m_wnd);

    //glBegin(GL_QUADS);
    //glColor4ub(clr.r, clr.g, clr.b, clr.a);
    //glVertex2f(p0.x, p0.y);
    //glVertex2f(p1.x, p1.y);
    //glVertex2f(p2.x, p2.y);
    //glVertex2f(p3.x, p3.y);
    //glEnd();

    vector2_qbuffer vq = to_clamped_space(vector2_qbuffer( p0, p1, p2, p3 ), m_wnd);

    glBegin(GL_QUADS);
    glColor4ub(clr.r, clr.g, clr.b, clr.a);
    glVertex2f(vq.v[ 0 ].x, vq.v[ 0 ].y);
    glVertex2f(vq.v[ 1 ].x, vq.v[ 1 ].y);
    glVertex2f(vq.v[ 2 ].x, vq.v[ 2 ].y);
    glVertex2f(vq.v[ 3 ].x, vq.v[ 3 ].y);
    glEnd();
  }

  void Context2D::rect(Vector2f start, Vector2f end, const Colorb clr)
  {
    start = to_clamped_space(start, m_wnd);
    end = to_clamped_space(end, m_wnd);

    glColor4ub(clr.r, clr.g, clr.b, clr.a);
    glRectf(start.x, start.y, end.x, end.y);
  }

  void Context2D::right_traingle(Vector2f base_start, Vector2f base_end, const Colorb clr)
  {
    glBegin(GL_TRIANGLES);
    base_start = to_clamped_space(base_start, m_wnd);
    base_start = to_clamped_space(base_start, m_wnd);



    glEnd();
  }

  void Context2D::traingle(Vector2f p0, Vector2f p1, Vector2f p2, const Colorb clr)
  {
  }

  void Context2D::line(Vector2f start, Vector2f end, const Colorb clr)
  {
    line(start, end, 1.0f, clr);
  }

  void Context2D::line(Vector2f start, Vector2f end, float_t width, const Colorb clr)
  {
    start = to_clamped_space(start, m_wnd);
    end = to_clamped_space(end, m_wnd);

    glBegin(GL_LINES);
    glLineWidth(width);
    glColor4ub(clr.r, clr.g, clr.b, clr.a);
    glVertex2f(start.x, start.y);
    glVertex2f(end.x, end.y);
    glEnd();
  }

  void Context2D::demo()
	{
    glClear(GL_COLOR_BUFFER_BIT);   // Clear the color buffer with current clearing color

    // Define shapes enclosed within a pair of glBegin and glEnd
    glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
    glColor3f(1.0f, 0.0f, 0.0f); // Red
    glVertex2f(-1.0f, 0.0f);     // Define vertices in counter-clockwise (CCW) order
    glColor3f(0.0f, 1.0f, 0.0f); // green
    glVertex2f(-0.0f, 0.0f);     //  so that the normal (front-face) is facing you
    glColor3f(0.0f, 0.0f, 1.0f); // blue
    glVertex2f(-0.0f, 1.0f);
    glColor3f(0.5f, 0.5f, 0.5f); // gray
    glVertex2f(-1.0f, 1.0f);

    glColor3f(0.0f, 1.0f, 0.0f); // Green
    glVertex2f(-0.7f, -0.6f);
    glVertex2f(-0.1f, -0.6f);
    glVertex2f(-0.1f, 0.0f);
    glVertex2f(-0.7f, 0.0f);

    glColor3f(0.2f, 0.2f, 0.2f); // Dark Gray
    glVertex2f(-0.9f, -0.7f);
    glColor3f(1.0f, 1.0f, 1.0f); // White
    glVertex2f(-0.5f, -0.7f);
    glColor3f(0.2f, 0.2f, 0.2f); // Dark Gray
    glVertex2f(-0.5f, -0.3f);
    glColor3f(1.0f, 1.0f, 1.0f); // White
    glVertex2f(-0.9f, -0.3f);
    glEnd();

    glBegin(GL_TRIANGLES);          // Each set of 3 vertices form a triangle
    glColor3f(0.0f, 0.0f, 1.0f); // Blue
    glVertex2f(0.1f, -0.6f);
    glVertex2f(0.7f, -0.6f);
    glVertex2f(0.4f, -0.1f);

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
