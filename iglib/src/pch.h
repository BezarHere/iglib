#pragma once
#ifndef PCH_H
#define PCH_H

#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <exception>
#include <intrin.h>
#include <cassert>
#include <stack>

#include <Windows.h>
#include <Bite.h>

#include <SOIL/SOIL.h>

#include <ft2build.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>



#define PEEK(p) std::cout << #p << ": " << (p) << '\n'

#define NOTNULL(exp) ASSERT(exp != nullptr)
#define ISNULL(exp) ASSERT(exp == nullptr)
#define FORCEINLINE __forceinline

typedef GLFWwindow *WindowHandle_t;
typedef GLFWmonitor *MonitorHandle_t;

typedef int GLFWbool_t;

template <typename _T>
struct _run_funcptr
{
	inline _run_funcptr(_T ptr)
		: _dummy(nullptr)
	{
		ptr();
	}

	void *_dummy;
};

namespace ig {}

using namespace ig;
using namespace bite;

constexpr dye::TerminalColor ErrorColor{ dye::ColorCode::LightRed, dye::ColorCode::Black };
constexpr dye::TerminalColor WarningColor{ dye::ColorCode::LightYellow, dye::ColorCode::Black };
constexpr dye::TerminalColor PingColor{ dye::ColorCode::LightBlue, dye::ColorCode::Black };

#ifndef ASSERT
#define ASSERT(cond) if (!(cond)) bite::raise(bite::format("failed condition \"{}\" at \"{}\" line {}", #cond, __FILE__, __LINE__))
#endif

#ifndef WARN
#define WARN(cond) if (!!(cond)) { bite::warn("\"" #cond  "\" is true at \"" __FILE__ "\" line " + std::to_string(__LINE__) + "\n"); }
#endif
#ifndef REPORT
// reverse of ASSERT, only prints warning IF the condition is TRUE also returns
#define REPORT(cond) if (!!(cond)) { bite::warn("\"" #cond  "\" is true at \"" __FILE__ "\" line " + std::to_string(__LINE__) + ", returning...\n"); return; }
#define REPORT_V(cond, value) if (!!(cond)) { bite::warn("\"" #cond  "\" is true at \"" __FILE__ "\" line " + std::to_string(__LINE__) + ", returning: " #value "\n"); return value; }
#endif // !REPORT


#endif