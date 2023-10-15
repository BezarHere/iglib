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

	struct Vertex
	{
		Vector2i pos;
		Colorb clr;
		Vector2f tex_coord;
	};
}
