#include "pch.h"
#include "_iglib_window.h"
#include "_iglib_context2d.h"

__forceinline Vector2f to_clamped_space(const Vector2f &val, const Window &wnd)
{
  const Vector2f wf{ wnd.get_size() };
  return Vector2f(val.x / wf.x, 1.0f - (val.y / wf.y));
}

__forceinline void to_clamped_space(Vector2f &val, const Window &wnd)
{
  const Vector2f wf{ wnd.get_size() };
  val.x /= wf.x;
  val.y = 1.0f - (val.y / wf.y);
}

namespace ig
{
  Context2D::Context2D(const Window &wnd)
    : m_wnd{ wnd }
  {
  }

  void Context2D::draw_quad(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, const Colorb clr)
    {
      glBegin(GL_QUADS);
      glColor4f(clr.r, clr.g, clr.b, clr.a);
      glVertex2f(p0.x, p0.y);
      glVertex2f(p1.x, p1.y);
      glVertex2f(p2.x, p2.y);
      glVertex2f(p3.x, p3.y);
      glEnd();
    }

    void Context2D::draw_rect(Vector2f position, Vector2f size, const Colorb clr)
    {
      glColor4f(clr.r, clr.g, clr.b, clr.a);
      glRectf(position.x, position.y, size.x, size.y);
    }

    void Context2D::demo()
	  {
      glClear(GL_COLOR_BUFFER_BIT);   // Clear the color buffer with current clearing color

      // Define shapes enclosed within a pair of glBegin and glEnd
      glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
      glColor3f(1.0f, 0.0f, 0.0f); // Red
      glVertex2f(-0.8f, 0.1f);     // Define vertices in counter-clockwise (CCW) order
      glVertex2f(-0.2f, 0.1f);     //  so that the normal (front-face) is facing you
      glVertex2f(-0.2f, 0.7f);
      glVertex2f(-0.8f, 0.7f);

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
