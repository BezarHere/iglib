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

	struct Vertex3D
	{
		Vector3i pos;
		Colorb clr;
		Vector2f tex_coord;
	};

	typedef unsigned VertexBufferId_t;
	template <typename _T>
	class BasicVertexBuffer
	{
	public:


	private:
		VertexBufferId_t m_id;
		
	};

	using VertexBuffer2D = BasicVertexBuffer<Vertex2D>;

}
