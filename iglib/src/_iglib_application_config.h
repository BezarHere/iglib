#pragma once
#include "_iglib_base.h"
#include "_iglib_vector.h"

namespace ig
{
	struct ApplicationConfig
	{
		Vector2i size{ 512, 350 };
		std::string title = "Demo";
		bool fullscreen = false;
		bool main_threaded = true;
	};
}
