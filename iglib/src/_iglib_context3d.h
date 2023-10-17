#pragma once
#include "_iglib_image.h"
#include "_iglib_shader.h"

namespace ig
{
	class Context3D
	{
	public:
		Context3D(const Window &wnd);

		const Window &get_window() const;

	private:
		const Window &m_wnd;
	};

	typedef void(*Draw3DCallback)(Context3D context);

}
