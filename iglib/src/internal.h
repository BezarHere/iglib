#pragma once
#include "pch.h"
#include "_iglib_base.h"
#include "_iglib_errors.h"
#include "_iglib_font.h"

constexpr size_t LumSize = 1;
constexpr size_t LumASize = 2;
constexpr size_t RGBSize = 3;
constexpr size_t RGBASize = 4;

using bite::raise;
using bite::warn;

struct GLVersion
{
	uint16_t major, minor;
	bool compat;
};

FORCEINLINE DECLSPEC_NORETURN void glfwerror( bool terminate = false ) {
	const char *__glfw_desc = nullptr;
	int __glfw_errcode = glfwGetError( &__glfw_desc );

	if (__glfw_errcode != GLFW_NO_ERROR)
	{
		if (terminate)
		{
			if (__glfw_desc)
				bite::raise( "GLFW Error: [" + std::to_string( __glfw_errcode ) + "] as '" + __glfw_desc + "'." );
			bite::raise( "GLFW Error: [" + std::to_string( __glfw_errcode ) + "]." );
		}

		dye::put_colors( { dye::ColorCode::Red, dye::ColorCode::Black } );
		if (__glfw_desc)
			std::cerr << "GLFW Error: [" << __glfw_errcode << "] as '" << __glfw_desc << "'.\n";
		std::cerr << "GLFW Error: [" << __glfw_errcode << "].\n";
		dye::clear_colors();
	}
}

extern const std::vector<MonitorHandle_t> &get_monitors();

extern bool is_glew_running();
extern bool is_glfw_running();

extern void init_glfw();
extern void init_glew();
extern void kill_glfw();

extern void push_to_draw_pipline( WindowHandle_t hdl );
extern WindowHandle_t top_draw_pipline();
extern void pop_draw_pipline();


extern int register_openglinit_callback( void(*callback)(void) ); // always returns 0

extern void glfw_init_window_hints();
extern bool set_glfw_context_version( int major, int minor, bool compat = false );

extern GLVersion get_glfw_context_version();

template <typename _T>
FORCEINLINE _T *blockcpy( const _T *src, const size_t size ) {
	return (_T *)memcpy( new _T[ size ], src, size );
}

// return the expo of the rounded down value to the closest power of 2 
template <typename _T>
FORCEINLINE _T closest_pow2( const _T value ) {
	for (_T i = (sizeof( _T ) * 8u) - 1; i >= 0; i--)
	{
		if (value >> i)
			return i;
	}
	return 0;
}
