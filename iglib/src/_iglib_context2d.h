#pragma once
#include "_iglib_base.h"
#include "_iglib_color.h"
#include "_iglib_vector.h"

namespace ig
{
	class Window;

	class Context2D
	{
	public:
		Context2D(const Window &wnd);

		void draw_quad(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, const Colorb clr);
		void draw_rect(Vector2f position, Vector2f size, const Colorb clr);
		void demo();

		const Window &get_window() const;

	private:
		const Window &m_wnd;
	};

	typedef void(*Draw2DCallback)(Context2D &context);

}
