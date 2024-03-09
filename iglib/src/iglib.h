#pragma once
#include "_iglib_diag.h"
#include "_iglib_errors.h"
#include "_iglib_rect.h"
#include "_iglib_renderer.h"
#include "_iglib_vector.h"
#include "_iglib_wavefront_obj.h"
#include "_iglib_window.h"

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
		static constexpr Version version = { 2, 1, 0 };
		IGlib() = delete;

		/// @brief sets the opengl version, versions range from 3.0 to 3.3 and 4.0 to 4.6
		/// @brief the default version is 4.1 and the patch number is unused
		/// @return weather the change was successful
		static bool set_opengl_version( Version v );

	};

	extern std::string get_opengl_version();

}
