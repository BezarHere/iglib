#pragma once
#include "_iglib_base.h"
#include "_iglib_vector.h"
#include "_iglib_color.h"

namespace ig
{
	enum class ShapeDrawType
	{
		Quad,
		Triangle,
		Line,
		LineStript, // connected lines
		TriangleStrip,
		TriangleFan,
		QuadStrip
	};

	struct Vertex2D
	{
		Vector2i pos;
		Colorb clr;
		Vector2f tex_coord;
	};
}
