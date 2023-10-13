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

FORCEINLINE void invert_y(unsigned char *data, const size_t ww, const size_t hh, const size_t ch_count)
{
	assert(ch_count <= 4 && ch_count > 0);
	assert(ww * hh > 0);
	NOTNULL(data);
	const size_t halfheight = hh >> 1;

#define InvertMethod 1

#if InvertMethod == 0
	for (size_t x{}; x < ww; x++)
	{
		for (size_t y{}; y < halfheight; y++)
		{
			// first pixel
			const size_t index1 = (x + (y * ww)) * ch_count;
			// second pixel
			const size_t index2 = (x + ((hh - y) * ww)) * ch_count;

			// copy pixel data
			for (size_t i{}; i < ch_count; i++)
				data[ index1 + i ] = data[ index2 + i ];
		}
	}
#elif InvertMethod == 1
	for (size_t j{}; j < halfheight; ++j)
	{
		size_t index1 = j * ww * ch_count;
		size_t index2 = (hh - 1 - j) * ww * 3;
		for (size_t i = ww * ch_count; i > 0; --i)
		{
			const auto temp = data[ index1 ];
			data[ index1 ] = data[ index2 ];
			data[ index2 ] = temp;
			++index1;
			++index2;
		}
	}
#else
	const size_t buf_len = ww * halfheight * ch_count;
	const size_t data_len = ww * hh * ch_count;
	auto buf = memcpy(new unsigned char[ buf_len ], data, buf_len);
	memmove(data, data + buf_len, data_len - buf_len);
	memcpy(data + buf_len, buf, buf_len);
#endif
#undef InvertMethod
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


