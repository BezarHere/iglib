#include "pch.h"
#include "iglib.h"
#include "internal.h"

constexpr Version CurrentVersion{ 1, 6, 0 };

namespace ig
{

	Error init()
	{

		
		return Error();
	}

	Version IGlib::version() noexcept
	{
		return CurrentVersion;
	}

	
	std::string get_opengl_version()
	{
		auto c = glGetString(GL_VERSION);
		// glew not init
		if (!c)
		{
			warn("Create an ig::Window to get the current OpenGL Version\n");
			return std::string();
		}

		return std::string((const char *)c);
	}

}
