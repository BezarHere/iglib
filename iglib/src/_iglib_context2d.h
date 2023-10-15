#pragma once
#include "_iglib_vertex.h"
#include "_iglib_batchdraw.h"
#include "_iglib_image.h"

namespace ig
{
	class Window;

	class Context2D
	{
	public:
		Context2D(const Window &wnd);

		void quad(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, const Colorb clr);
		void rect(Vector2f start, Vector2f end, const Colorb clr);
		void traingle(Vector2f p0, Vector2f p1, Vector2f p2, const Colorb clr);
		void line(Vector2f start, Vector2f end, const Colorb clr);
		void line(Vector2f start, Vector2f end, float_t width, const Colorb clr);

		void traingle_strips(const vector2f_buffer_view_t points, const Colorb clr);

		void vertecies(Vertex *vert, size_t count, VertexDrawType draw_type);

		void circle(float radius, Vector2f center, const Colorb clr, const uint16_t vertcies_count = 32);
		
		void demo();

		const Window &get_window() const;

	private:
		const Window &m_wnd;
	};

	typedef void(*Draw2DCallback)(Context2D context);

}
