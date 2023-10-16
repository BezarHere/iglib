#pragma once
#include "pch.h"
#include "_iglib_base.h"
#include "_iglib_errors.h"

constexpr size_t LumSize = 1;
constexpr size_t LumASize = 2;
constexpr size_t RGBSize = 3;
constexpr size_t RGBASize = 4;

using bite::raise;
using bite::warn;

FORCEINLINE void glfwerror(bool terminate = false)
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

extern GLFWwindow *create_window(int width, int height, const std::string &title, GLFWmonitor *fullscreen = nullptr, GLFWwindow *share = nullptr);

extern const std::vector<MonitorHandle_t> &get_monitors();

extern bool is_glew_running();
extern bool is_glfw_running();

extern void init_glfw();
extern void init_glew();
extern void kill_glfw();

extern void push_to_draw_pipline(WindowHandle_t hdl);
extern WindowHandle_t top_draw_pipline();
extern void pop_draw_pipline();

