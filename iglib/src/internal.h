#pragma once
#include "pch.h"
#include "_iglib_base.h"
#include "_iglib_errors.h"

__forceinline void glfwerror(bool terminate = false)
{
	const char *desc = nullptr;
	int i = glfwGetError(&desc);

	if (i == GLFW_NO_ERROR)
		return;

	if (terminate)
	{
		if (desc)
			throw std::runtime_error("GLFW Error: [" + std::to_string(i) + "] as '" + desc + "'.");
		throw std::runtime_error("GLFW Error: [" + std::to_string(i) + "].");
	}

	dye::put_colors({ dye::ColorCode::Red, dye::ColorCode::Black });
	if (desc)
		std::cerr << "GLFW Error: [" << i << "] as '" << desc << "'.\n";
	std::cerr << "GLFW Error: [" << i << "].\n";
	dye::clear_colors();
}


__forceinline void raise(const std::exception &exc)
{
	dye::put_colors({ dye::ColorCode::Red, dye::ColorCode::Black });
	std::cerr << "#Error:\n" << exc.what() << '\n';
	dye::clear_colors();
	throw exc;
}

__forceinline void raise(const std::string &msg)
{
	raise(std::runtime_error(msg));
}


__forceinline void raise(const char* const msg)
{
	raise(std::runtime_error(msg));
}

// nullptr safe
__forceinline void warn(const char *const msg)
{
	if (!msg)
		return;
	dye::put_colors({ dye::ColorCode::LightYellow, dye::ColorCode::Black });
	std::cerr << msg << '\n';
	dye::clear_colors();
}

__forceinline void warn(const std::string &msg)
{
	dye::put_colors({ dye::ColorCode::LightYellow, dye::ColorCode::Black });
	std::cerr << msg << '\n';
	dye::clear_colors();
}


extern GLFWwindow *create_window(int width, int height, const std::string &title, GLFWmonitor *fullscreen = nullptr, GLFWwindow *share = nullptr);

extern const std::vector<MonitorHandle_t> &get_monitors();

extern bool is_glew_running();
extern bool is_glfw_running();

extern void init_glfw();
extern void init_glew();
extern void kill_glfw();
