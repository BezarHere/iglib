#include "pch.h"
#include "iglib.h"
#include "internal.h"


namespace ig
{

	Error init() {


		return Error();
	}

	bool IGlib::set_opengl_version( Version v ) {
		return set_glfw_context_version( v.major, v.minor );
	}


	std::string get_opengl_version() {
		auto c = glGetString( GL_VERSION );
		// glew not init
		if (!c)
		{
			warn( "Create an ig::Window to get the current OpenGL Version\n" );
			return std::string();
		}

		return std::string( (const char *)c );
	}

}
