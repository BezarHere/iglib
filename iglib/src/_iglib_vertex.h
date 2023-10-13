#pragma once
#include "_iglib_base.h"
#include "_iglib_vector.h"
#include "_iglib_color.h"

namespace ig
{
	enum class VertexDrawType
	{
		Quad,
		Triangle,
		Line,
		LineStript, // connected lines
		TriangleStrip,
		TriangleFan,
		QuadStrip
	};

	class Context2D;

	// unpredictable behivore outside render/draw callbacks
	class BatchDraw
	{
	public:
		BatchDraw(Context2D c, VertexDrawType type);
		~BatchDraw();

		void vertex(Vector2f v);
		void vertex(Vector2i v);

		void color(Colorb v);
		void color(Colorf v);

		void texcoord(Vector2f v);
	};

}
