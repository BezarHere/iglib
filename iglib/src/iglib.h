#pragma once
#include "_iglib_vector.h"
#include "_iglib_rect.h"
#include "_iglib_window.h"
#include "_iglib_errors.h"
#include "_iglib_application.h"

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
		static int bee();

	};

	extern std::string get_gl_version();

}
