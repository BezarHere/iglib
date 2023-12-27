#pragma once
#include "_iglib_vector.h"
#include "_iglib_rect.h"
#include "_iglib_window.h"
#include "_iglib_renderer.h"
#include "_iglib_errors.h"
#include "_iglib_wavefront_obj.h"

#define IGLIB_H

namespace ig
{
	struct Version
	{
		uint8_t major, minor, patch;
	};

	class IGlib
	{
	public:
		IGlib() = delete;

		static Version version() noexcept;

	};

	extern std::string get_opengl_version();

}
