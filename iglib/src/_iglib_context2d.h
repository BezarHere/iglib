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

		void quad(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, const Colorb clr);
		void rect(Vector2f start, Vector2f end, const Colorb clr);
		void right_traingle(Vector2f base_start, Vector2f base_end, const Colorb clr);
		void traingle(Vector2f p0, Vector2f p1, Vector2f p2, const Colorb clr);
		void line(Vector2f start, Vector2f end, const Colorb clr);
		void line(Vector2f start, Vector2f end, float_t width, const Colorb clr);

		void demo();

		const Window &get_window() const;

	private:
		const Window &m_wnd;
	};

	typedef void(*Draw2DCallback)(Context2D &context);

}
