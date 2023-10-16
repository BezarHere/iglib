#pragma once
#include "_iglib_base.h"
#include "_iglib_rect.h"
#include "_iglib_context2d.h"
#include "_iglib_context3d.h"
#include <memory>

namespace ig
{

	enum class WindowCallbackReason
	{
		Resized,
		Moved,
		Focused,
		Unfocused,
		Minimized,
		Maxmized,
		Restored,

		DirtyScreen,

		ResizedFramebuffer,
		RescaledContents,

		// when the user presses close/alt+f4 on the window. if the event is triggered, should_close() will always be true
		RequestedClose,
		Closing
	};

	enum class WindowVisibiltyState : uint8_t
	{
		Hidden,
		Minimized,
		Restored,
		Maximized
	};

	enum Key : int16_t
	{
		Unkown = -1,
		Key_Space = 32,
		Key_Apostrophe = 39,
		Key_Comma = 44,
		Key_Minus = 45,
		Key_Period = 46,
		Key_Slash = 47,
		Key_0 = 48,
		Key_1 = 49,
		Key_2 = 50,
		Key_3 = 51,
		Key_4 = 52,
		Key_5 = 53,
		Key_6 = 54,
		Key_7 = 55,
		Key_8 = 56,
		Key_9 = 57,
		Key_Semicolon = 59,
		Key_Equal = 61,
		Key_A = 65,
		Key_B = 66,
		Key_C = 67,
		Key_D = 68,
		Key_E = 69,
		Key_F = 70,
		Key_G = 71,
		Key_H = 72,
		Key_I = 73,
		Key_J = 74,
		Key_K = 75,
		Key_L = 76,
		Key_M = 77,
		Key_N = 78,
		Key_O = 79,
		Key_P = 80,
		Key_Q = 81,
		Key_R = 82,
		Key_S = 83,
		Key_T = 84,
		Key_U = 85,
		Key_V = 86,
		Key_W = 87,
		Key_X = 88,
		Key_Y = 89,
		Key_Z = 90,
		Key_LeftBracket = 91,
		Key_Backslash = 92,
		Key_RightBracket = 93,
		Key_GraveAccent = 96,
		Key_World1 = 161,
		Key_World2 = 162,
		Key_Escape = 256,
		Key_Enter = 257,
		Key_Tab = 258,
		Key_Backspace = 259,
		Key_Insert = 260,
		Key_Delete = 261,
		Key_Right = 262,
		Key_Left = 263,
		Key_Down = 264,
		Key_Up = 265,
		Key_PageUp = 266,
		Key_PageDown = 267,
		Key_Home = 268,
		Key_End = 269,
		Key_CapsLock = 280,
		Key_ScrollLock = 281,
		Key_NumLock = 282,
		Key_PrintScreen = 283,
		Key_Pause = 284,
		Key_F1 = 290,
		Key_F2 = 291,
		Key_F3 = 292,
		Key_F4 = 293,
		Key_F5 = 294,
		Key_F6 = 295,
		Key_F7 = 296,
		Key_F8 = 297,
		Key_F9 = 298,
		Key_F10 = 299,
		Key_F11 = 300,
		Key_F12 = 301,
		Key_F13 = 302,
		Key_F14 = 303,
		Key_F15 = 304,
		Key_F16 = 305,
		Key_F17 = 306,
		Key_F18 = 307,
		Key_F19 = 308,
		Key_F20 = 309,
		Key_F21 = 310,
		Key_F22 = 311,
		Key_F23 = 312,
		Key_F24 = 313,
		Key_F25 = 314,
		Key_Kp0 = 320,
		Key_Kp1 = 321,
		Key_Kp2 = 322,
		Key_Kp3 = 323,
		Key_Kp4 = 324,
		Key_Kp5 = 325,
		Key_Kp6 = 326,
		Key_Kp7 = 327,
		Key_Kp8 = 328,
		Key_Kp9 = 329,
		Key_KpDecimal = 330,
		Key_KpDivide = 331,
		Key_KpMultiply = 332,
		Key_KpSubtract = 333,
		Key_KpAdd = 334,
		Key_KpEnter = 335,
		Key_KpEqual = 336,
		Key_LeftShift = 340,
		Key_LeftControl = 341,
		Key_LeftAlt = 342,
		Key_LeftSuper = 343,
		Key_RightShift = 344,
		Key_RightControl = 345,
		Key_RightAlt = 346,
		Key_RightSuper = 347,
		Key_Menu = 348
	};

	enum class KeyAction : uint8_t
	{
		Released,
		Pressed,
		Repeated
	};

	enum class KeyModFlags : uint8_t
	{
		None = 0,
		Shift = 0x01,
		Control = 0x02,
		Alt = 0x04,
		Super = 0x08,
		Windows = 0x08,
		Capslock = 0x10,
		Numlock = 0x20,
	};

	class Window;

	typedef void(*WindowCallback_t)(Window &window, WindowCallbackReason reason);
	typedef void(*KeyCallback_t)(Window &window, Key key, KeyAction action, KeyModFlags mods);

	class Window final
	{
		friend class Application;
	public:
		Window() noexcept;
		//Window(Vector2i size, std::string title);
		~Window();

		Window(const Window &copy) = delete;
		Window(Window &&move) noexcept;

		Window &operator=(const Window &other) = delete;
		Window &operator=(Window &&other) noexcept;

		bool is_valid() const;

		bool should_close() const;

		Vector2i get_mouse_position() const;


		int get_width() const;
		int get_height() const;
		Vector2i get_size() const;

		Vector2i get_position() const;
		const Recti &get_rect() const;

		// refreshes the windows position/size values, not the window's content
		// only call if unexpected values are returned from get_width/get_height/get_size/get_position/get_rect
		void refresh_rect();

		const std::string &get_title() const;
		void set_title(const std::string &title);

		WindowCallback_t get_callback() const;
		void set_callback(WindowCallback_t callback);

		KeyCallback_t get_key_callback() const;
		void set_key_callback(KeyCallback_t callback);

		bool is_deffered_to_close() const noexcept;
		bool is_focused() const noexcept;

		void set_draw2d_callback(Draw2DCallback callback) noexcept;
		Draw2DCallback get_draw2d_callback() const noexcept;

		void set_draw3d_callback(Draw3DCallback callback) noexcept;
		Draw3DCallback get_draw3d_callback() const noexcept;

		Context2D &get_2d_context() noexcept;
		const Context2D &get_2d_context() const noexcept;

		// Not recomended to draw to this, full of unpredictable behivore. use 
		Context3D &get_3d_context() noexcept;
		const Context3D &get_3d_context() const noexcept;

		void hide();
		void show();

		// won't always return the value at m_visible_state
		WindowVisibiltyState get_visiblity_state() const;

		void render();

		void poll();
		void clear();

		void ping() const noexcept;

		Image to_image(const Recti rect) const;
		Image to_image() const;

		// will remove the window and render this object as invalid
		void close() noexcept;

	private:
		Window(void *const handle, const std::string &title, bool hidden) noexcept;

		class WindowCallbackEngine;

	private:
		void *m_hdl;
		bool m_hidden;
		WindowVisibiltyState m_visible_state;
		bool m_focused = true;
		Recti m_rect;
		std::string m_title{};
		Vector2i m_frambeuffer_size{ 1, 1 };
		Vector2f m_content_scale{ 1.0f, 1.0f };
		bool m_deffered_close{ false }; // did the user click close or pressed alt+f4?
		//std::shared_ptr<size_t> m_handle_rc;

		const TimeMs_t m_creation_time{ TimeMs_t::duration(TimeMs_t::clock::now().time_since_epoch().count()) };
		
		Context2D m_context_2d;
		Context3D m_context_3d;

		Draw2DCallback m_draw_2d_callback = nullptr;
		Draw3DCallback m_draw_3d_callback = nullptr;

		WindowCallback_t m_callback = nullptr;
		KeyCallback_t m_key_callback = nullptr;
	};
}