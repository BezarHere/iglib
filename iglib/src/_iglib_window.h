#pragma once
#include "_iglib_base.h"
#include "_iglib_rect.h"
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

	enum class WindowVisibiltyState
	{
		Hidden,
		Minimized,
		Restored,
		Maximized
	};

	enum Key
	{

	};

	enum KeyAction
	{
		Released,
		Pressed,
		Repeated
	};

	enum KeyModFlags
	{
		None = 0,
		RShift = 1,
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

		Vector2i get_size() const;
		Vector2i get_position() const;
		const Recti &get_rect() const;

		const std::string &get_title() const;
		void set_title(const std::string &title);

		WindowCallback_t get_callback() const;
		void set_callback(WindowCallback_t callback);

		KeyCallback_t get_key_callback() const;
		void set_key_callback(KeyCallback_t callback);

		bool is_deffered_to_close() const noexcept;
		bool is_focused() const noexcept;

		void hide();
		void show();

		// won't always return the value at m_visible_state
		WindowVisibiltyState get_visiblity_state() const;

		void render();
		void poll();
		void clear();

		void ping() const noexcept;
	

		// will remove the window and render this object as invalid
		void close() noexcept;

		operator bool() const;

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
		Vector2 m_content_scale{ 1.0f, 1.0f };
		bool m_deffered_close{ false }; // did the user click close or pressed alt+f4?
		//std::shared_ptr<size_t> m_handle_rc;

		const TimeMs_t m_creation_time{ TimeMs_t::duration(TimeMs_t::clock::now().time_since_epoch().count()) };
		
		WindowCallback_t m_callback = nullptr;
		KeyCallback_t m_key_callback = nullptr;
	};
}
