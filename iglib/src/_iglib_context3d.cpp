#include "pch.h"
#include "_iglib_window.h"
#include "_iglib_context3d.h"

namespace ig
{

	Context3D::Context3D(const Window &wnd)
		: m_wnd{ wnd }
	{
	}

	const Window &Context3D::get_window() const
	{
		return m_wnd;
	}

}
