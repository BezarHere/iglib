#include "pch.h"
#include "_iglib_batchdraw.h"
#include "_iglib_canvas.h"
#include "_iglib_window.h"
#include "draw_internal.h"

const BatchDraw *g_WorkingBatchDraw = nullptr;
PrimitiveType g_WorkingBatchDrawType;
const Window *g_WorkingBatchDrawWindow;

namespace ig
{

	BatchDraw::BatchDraw(Canvas c, PrimitiveType type)
	{
		if (g_WorkingBatchDraw)
			raise("Can't create more then one BatchDraw object at the same time");
		g_WorkingBatchDraw = this;
		g_WorkingBatchDrawType = type;
		g_WorkingBatchDrawWindow = &c.get_window();
		glBegin(to_glprimitve(type));
	}

	BatchDraw::~BatchDraw()
	{
		glEnd();
		g_WorkingBatchDraw = nullptr;
	}

	void BatchDraw::vertex(Vector2f v)
	{
		glVertex2f(v);
	}

	void BatchDraw::vertex(Vector2i v)
	{
		glVertex2f(v);
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
