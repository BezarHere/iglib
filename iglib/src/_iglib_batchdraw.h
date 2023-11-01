#pragma once
#include "_iglib_base.h"
#include "_iglib_vertex.h"

namespace ig
{
	class Canvas;

	// unpredictable behivore outside render/draw callbacks
	// only one can be created at a time
	class BatchDraw
	{
	public:
		BatchDraw(Canvas c, PrimitiveType type);
		~BatchDraw();

		void vertex(Vector2f v);
		void vertex(Vector2i v);

		void color(Colorb v);
		void color(Colorf v);

		void texcoord(Vector2f v);
	};

}
