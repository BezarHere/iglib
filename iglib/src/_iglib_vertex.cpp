#include "pch.h"
#include "_iglib_vertex.h"
#include "_iglib_context2d.h"
#include "_iglib_window.h"

const BatchDraw *g_WorkingBatchDraw = nullptr;
VertexDrawType g_WorkingBatchDrawType;
const Window *g_WorkingBatchDrawWindow;

FORCEINLINE void glVertex2f(const Vector2f f)
{
	glVertex2f(f.x, f.y);
}

FORCEINLINE void glVertex2i(const Vector2i f)
{
	glVertex2i(f.x, f.y);
}

constexpr FORCEINLINE int to_gldraw_v(const VertexDrawType type)
{
	switch (type)
	{
	case ig::VertexDrawType::Quad:
		return GL_QUADS;
	case ig::VertexDrawType::Triangle:
		return GL_TRIANGLES;
	case ig::VertexDrawType::Line:
		return GL_LINES;
	case ig::VertexDrawType::LineStript:
		return GL_LINE_STRIP;
	case ig::VertexDrawType::TriangleStrip:
		return GL_TRIANGLE_STRIP;
	case ig::VertexDrawType::TriangleFan:
		return GL_TRIANGLE_FAN;
	case ig::VertexDrawType::QuadStrip:
		return GL_QUAD_STRIP;
	default:
		break;
	}
	raise(format("invalid draw type value {}", (int)type));
	return -1;
}


namespace ig
{

	BatchDraw::BatchDraw(Context2D c, VertexDrawType type)
	{
		if (g_WorkingBatchDraw)
			raise("Can't create more then one BatchDraw object at the same time");
		g_WorkingBatchDraw = this;
		g_WorkingBatchDrawType = type;
		g_WorkingBatchDrawWindow = &c.get_window();
		glBegin(to_gldraw_v(type));
	}

	BatchDraw::~BatchDraw()
	{
		glEnd();
		g_WorkingBatchDraw = nullptr;
	}

	void BatchDraw::vertex(Vector2f v)
	{
		glVertex2f(g_WorkingBatchDrawWindow->local_to_native(v));
	}

	void BatchDraw::vertex(Vector2i v)
	{
		glVertex2f(g_WorkingBatchDrawWindow->local_to_native(v));
	}

	void BatchDraw::color(Colorb v)
	{
		glColor4b(v.r, v.g, v.b, v.a);
	}

	void BatchDraw::color(Colorf v)
	{
		glColor4f(v.r, v.g, v.b, v.a);
	}

	void BatchDraw::texcoord(Vector2f v)
	{
		glTexCoord2f(v.x, v.y);
	}
}
