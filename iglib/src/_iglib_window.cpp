#include "pch.h"
#include "internal.h"
#include "_iglib_window.h"

namespace ig
{
	//Window::Window(Vector2i size, std::string title)
	//	: m_hdl((void *)create_window(size.x, size.y, title))
	//{
	//	glCreateProgram();
	//}



	std::vector<Window *> wce_available_windows;
	std::unordered_map<WindowHandle_t, Window *> wce_handles_map;

	class Window::WindowCallbackEngine
	{
		class WindowCallbacksRouter
		{
			friend class Window::WindowCallbackEngine;


			static void moved(WindowHandle_t hdl, int x, int y);
			static void resized(WindowHandle_t hdl, int w, int h);


			static void requested_close(WindowHandle_t hdl);
			static void dirty(WindowHandle_t hdl);

			static void focuse_changed(WindowHandle_t hdl, GLFWbool_t focused);
			static void minimized(WindowHandle_t hdl, GLFWbool_t is_minimized);
			static void maximized(WindowHandle_t hdl, GLFWbool_t is_maximized);

			static void framebuffer_resized(WindowHandle_t hdl, int w, int h);
			static void contents_rescaled(WindowHandle_t hdl, float xfactor, float yfactor);

			static void key_pressed(WindowHandle_t hdl, int key, int scancode, int action, int mods);

		};


	public:
		WindowCallbackEngine()
		{
			WindowCallbackEngine::init();
		}


		static void link(Window *window)
		{
			NOTNULL(window);
			NOTNULL(window->m_hdl);
			if (has_hdl((WindowHandle_t)window->m_hdl))
				raise("More then one window have the same handle, pease review you code for mis-use with window handles.");

			wce_available_windows.push_back(window);
			wce_handles_map[ (WindowHandle_t)window->m_hdl ] = window;
			connect_callbacks((WindowHandle_t)window->m_hdl);
		}

		static void unlink(Window *window)
		{
			pop_weak(window, (WindowHandle_t)window->m_hdl);
		}

		static void remap(Window *window, WindowHandle_t old_hdl)
		{
			NOTNULL(window);

			// more like a reset
			pop_weak(window, old_hdl);

			link(window);
		}

		// -------------------- PRIVATE --------------------
	private:

		static void init()
		{

		}

		static bool has_hdl(WindowHandle_t hdl)
		{
			return wce_handles_map.find(hdl) != wce_handles_map.end();
		}

		static void remove_from_handle_ptr_map(WindowHandle_t hdl)
		{
			NOTNULL(hdl);
			wce_handles_map.erase(hdl);
		}

		static void remove_from_handle_ptr_map(Window *window)
		{
			NOTNULL(window);

			for (auto &kv : wce_handles_map)
			{
				if (kv.second == window)
				{
					remove_from_handle_ptr_map(kv.first);
					break;
				}
			}
		}

		static void remove_from_listed_windows(Window *window)
		{
			NOTNULL(window);

			auto i = std::find(wce_available_windows.begin(), wce_available_windows.end(), window);

			if (i == wce_available_windows.end())
			{
				// does not exists
				// TODO: make something here
			}
			else
			{
				wce_available_windows.erase(i);
			}
		}

		static inline void connect_callbacks(WindowHandle_t hdl)
		{
			NOTNULL(hdl);
			(void)glfwSetWindowSizeCallback(hdl, WindowCallbacksRouter::resized);
			(void)glfwSetWindowPosCallback(hdl, WindowCallbacksRouter::moved);

			(void)glfwSetWindowCloseCallback(hdl, WindowCallbacksRouter::requested_close);
			(void)glfwSetWindowRefreshCallback(hdl, WindowCallbacksRouter::dirty);

			(void)glfwSetWindowFocusCallback(hdl, WindowCallbacksRouter::focuse_changed);
			(void)glfwSetWindowIconifyCallback(hdl, WindowCallbacksRouter::minimized);
			(void)glfwSetWindowMaximizeCallback(hdl, WindowCallbacksRouter::maximized);

			(void)glfwSetFramebufferSizeCallback(hdl, WindowCallbacksRouter::framebuffer_resized);
			(void)glfwSetWindowContentScaleCallback(hdl, WindowCallbacksRouter::contents_rescaled);


			(void)glfwSetKeyCallback(hdl, WindowCallbacksRouter::key_pressed);
		}

		static inline void disconnect_callbacks(WindowHandle_t hdl)
		{
			NOTNULL(hdl);
			(void)glfwSetWindowSizeCallback(hdl, nullptr);
			(void)glfwSetWindowPosCallback(hdl, nullptr);

			(void)glfwSetWindowCloseCallback(hdl, nullptr);
			(void)glfwSetWindowRefreshCallback(hdl, nullptr);

			(void)glfwSetWindowFocusCallback(hdl, nullptr);
			(void)glfwSetWindowIconifyCallback(hdl, nullptr);
			(void)glfwSetWindowMaximizeCallback(hdl, nullptr);

			(void)glfwSetFramebufferSizeCallback(hdl, nullptr);
			(void)glfwSetWindowContentScaleCallback(hdl, nullptr);


			(void)glfwSetKeyCallback(hdl, nullptr);
		}

		static void pop_weak(Window *window, WindowHandle_t hdl)
		{
			NOTNULL(window);
			NOTNULL(window->m_hdl);

			remove_from_listed_windows(window);
			
			disconnect_callbacks(hdl);

			remove_from_handle_ptr_map(hdl);
			remove_from_handle_ptr_map(window);
		}

		static inline Window *get_window(const WindowHandle_t hdl)
		{
			NOTNULL(hdl);
			return wce_handles_map.at(hdl);
		}

		static inline void recall_command(WindowHandle_t hdl, WindowCallbackReason reason)
		{
			Window *window = get_window(hdl);
			switch (reason)
			{
			case ig::WindowCallbackReason::Focused:
				window->m_focused = true;
				break;
			case ig::WindowCallbackReason::Unfocused:
				window->m_focused = false;
				break;
			case ig::WindowCallbackReason::Minimized:
				window->m_visible_state = WindowVisibiltyState::Minimized;
				break;
			case ig::WindowCallbackReason::Maxmized:
				window->m_visible_state = WindowVisibiltyState::Maximized;
				break;
			case ig::WindowCallbackReason::Restored:
				window->m_visible_state = WindowVisibiltyState::Restored;
				break;
			case ig::WindowCallbackReason::RequestedClose:
				window->m_deffered_close = true;
				break;
			default:
				break;
			}
			if (window->m_callback)
				window->m_callback(*window, reason);
			else if (window->m_deffered_close)
			{
				window->close();
			}

		}

		static inline void recall_command(WindowHandle_t hdl, WindowCallbackReason reason, int a, int b)
		{
			Window *window = get_window(hdl);

			switch (reason)
			{
			case ig::WindowCallbackReason::Resized:
				window->m_rect.w = a;
				window->m_rect.h = b;
				break;
			case ig::WindowCallbackReason::Moved:
				window->m_rect.x = a;
				window->m_rect.y = b;
				break;
			case ig::WindowCallbackReason::ResizedFramebuffer:
				window->m_frambeuffer_size.set(a, b);
				break;
			default:
				break;
			}

			if (window->m_callback)
				window->m_callback(*window, reason);
		}

		static inline void recall_command(WindowHandle_t hdl, WindowCallbackReason reason, float a, float b)
		{
			Window *window = get_window(hdl);

			window->m_content_scale.set(a, b);

			if (window->m_callback)
				window->m_callback(*window, reason);
		}
	} __WindowCallbackEngine_Dummy;



	void Window::WindowCallbackEngine::WindowCallbacksRouter::moved(WindowHandle_t hdl, int x, int y)
	{
		WindowCallbackEngine::recall_command(hdl, WindowCallbackReason::Moved, x, y);
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::resized(WindowHandle_t hdl, int w, int h)
	{
		WindowCallbackEngine::recall_command(hdl, WindowCallbackReason::Resized, w, h);

	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::requested_close(WindowHandle_t hdl)
	{
		WindowCallbackEngine::recall_command(hdl, WindowCallbackReason::RequestedClose);
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::dirty(WindowHandle_t hdl)
	{
		WindowCallbackEngine::recall_command(hdl, WindowCallbackReason::DirtyScreen);
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::focuse_changed(WindowHandle_t hdl, GLFWbool_t focused)
	{
		if (focused)
			WindowCallbackEngine::recall_command(hdl, WindowCallbackReason::Focused);
		else
			WindowCallbackEngine::recall_command(hdl, WindowCallbackReason::Unfocused);
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::minimized(WindowHandle_t hdl, GLFWbool_t is_minimized)
	{
		if (is_minimized)
			WindowCallbackEngine::recall_command(hdl, WindowCallbackReason::Minimized);
			else
			WindowCallbackEngine::recall_command(hdl, WindowCallbackReason::Restored);
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::maximized(WindowHandle_t hdl, GLFWbool_t is_maximized)
	{
		if (is_maximized)
			WindowCallbackEngine::recall_command(hdl, WindowCallbackReason::Maxmized);
		else
			WindowCallbackEngine::recall_command(hdl, WindowCallbackReason::Restored);
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::framebuffer_resized(WindowHandle_t hdl, int w, int h)
	{
		WindowCallbackEngine::recall_command(hdl, WindowCallbackReason::ResizedFramebuffer, w, h);
	}
		
	void Window::WindowCallbackEngine::WindowCallbacksRouter::contents_rescaled(WindowHandle_t hdl, float xfactor, float yfactor)
	{
		WindowCallbackEngine::recall_command(hdl, WindowCallbackReason::RescaledContents, xfactor, yfactor);
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::key_pressed(WindowHandle_t hdl, int key, int scancode, int action, int mods)
	{
		Window *window = WindowCallbackEngine::get_window(hdl);
		if (window->m_key_callback)
			window->m_key_callback(*window, (Key)key, (KeyAction)action, (KeyModFlags)mods);
		if (key == GLFW_KEY_F4 && mods & GLFW_MOD_ALT)
			WindowCallbacksRouter::requested_close(hdl);
	}


	Window::Window() noexcept
		: m_hdl(nullptr),
		m_visible_state{ WindowVisibiltyState::Restored },
		m_focused{ false },
		m_hidden{ false }

			//m_handle_rc{ new size_t{1} }
	{
	}

	//Window::Window(const Window &copy)
	//	: m_hdl(copy.m_hdl)
	//{
	//	if (&copy == this)
	//		return;
	//	m_handle_rc = copy.m_handle_rc;
	//	(*m_handle_rc)++;
	//	WindowCallbackEngine::link(this);
	//}

	Window::Window(Window &&move) noexcept
		: m_hdl(move.m_hdl),
			m_visible_state{ move.m_visible_state },
			m_focused{ move.m_focused },
			m_title( move.m_title ),
			m_hidden{ move.m_hidden }
	{
		WindowCallbackEngine::unlink(&move);
		move.m_hdl = nullptr;
		WindowCallbackEngine::link(this);
	}
		
	Window::Window(void *const handle, const std::string &title, bool hidden) noexcept
		: m_hdl(handle),
			m_visible_state{ WindowVisibiltyState::Minimized },
			m_focused{ false },
			m_title{ title },
			m_hidden{ hidden }
		//m_handle_rc{ new size_t{1} }
	{
		if (handle == nullptr)
			raise("NULL window handle passed to a window ctor.");
		WindowCallbackEngine::link(this);
	}

	Window::~Window()
	{
		
		if (m_hdl)
		{
			close();
		}
		//if (*m_handle_rc.get() <= 1)
		//{
		//	glfwDestroyWindow((GLFWwindow *)m_hdl);
		//}
		//(*m_handle_rc)--;
	}

	//Window &Window::operator=(const Window &other)
	//{
	//	if (&other == this)
	//		return *this;
	//	m_handle_rc = other.m_handle_rc;
	//	(*m_handle_rc)++;
	//	WindowHandle_t old_hdl = (WindowHandle_t)(m_hdl);
	//	m_hdl = other.m_hdl;
	//	WindowCallbackEngine::remap(this, old_hdl);
	//	return *this;
	//}

	Window &Window::operator=(Window &&other) noexcept
	{
		//m_handle_rc = other.m_handle_rc;
		//(*m_handle_rc)++;

		m_focused = other.m_focused;
		m_callback = other.m_callback;
		m_content_scale = other.m_content_scale;
		m_rect = other.m_rect;
		m_frambeuffer_size = other.m_frambeuffer_size;
		m_visible_state = other.m_visible_state;

		WindowHandle_t old_hdl = (WindowHandle_t)m_hdl;
		m_hdl = other.m_hdl;
		WindowCallbackEngine::unlink(&other);
		other.m_hdl = nullptr;
		WindowCallbackEngine::remap(this, old_hdl);
		return *this;
	}

	bool Window::is_valid() const
	{
		return m_hdl != nullptr;
	}

	bool Window::should_close() const
	{
		return m_hdl == nullptr || m_deffered_close || glfwWindowShouldClose((GLFWwindow *)m_hdl);
	}

	Vector2i Window::get_mouse_position() const
	{
		double x, y;
		glfwGetCursorPos((WindowHandle_t)m_hdl, &x, &y);
		return Vector2i((int)x, (int)y);
	}

	Vector2i Window::get_size() const
	{
		//glfwGetWindowSize((GLFWwindow *)m_hdl, &m_rect.w, &m_rect.h);
		return m_rect.size();
	}

	Vector2i Window::get_position() const
	{
		//glfwGetWindowSize((GLFWwindow *)m_hdl, &m_rect.x, &m_rect.y);
		return m_rect.position();
	}

	const Recti &Window::get_rect() const
	{
		return m_rect;
	}

	const std::string &Window::get_title() const
	{
		return m_title;
	}

	void Window::set_title(const std::string &title)
	{
		m_title = title;
		glfwSetWindowTitle((WindowHandle_t)m_hdl, title.c_str());
	}

	WindowCallback_t Window::get_callback() const
	{
		return m_callback;
	}

	void Window::set_callback(WindowCallback_t callback)
	{
		m_callback = callback;
	}

	KeyCallback_t Window::get_key_callback() const
	{
		return m_key_callback;
	}

	void Window::set_key_callback(KeyCallback_t callback)
	{
		m_key_callback = callback;
	}

	void Window::render()
	{
		glfwSwapBuffers((GLFWwindow *)m_hdl);
	}

	void Window::poll()
	{
		glfwPollEvents();
	}

	void Window::clear()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	bool Window::is_deffered_to_close() const noexcept
	{
		return m_deffered_close;
	}

	bool Window::is_focused() const noexcept
	{
		return m_focused;
	}

	void Window::hide()
	{
		glfwHideWindow((WindowHandle_t)m_hdl);
		m_hidden = true;
	}

	void Window::show()
	{
		glfwShowWindow((WindowHandle_t)m_hdl);
		m_hidden = false;
	}

	WindowVisibiltyState Window::get_visiblity_state() const
	{
		if (m_hidden)
			return WindowVisibiltyState::Hidden;
		return m_visible_state;
	}

	//void Window::focuse()
	//{
	//	glfwFocusWindow((WindowHandle_t)m_hdl);
	//}

	//void Window::unfocuse()
	//{
	//}

	void Window::ping() const noexcept
	{
		glfwRequestWindowAttention((WindowHandle_t)m_hdl);
	}

	void Window::close() noexcept
	{
		if (m_callback)
			m_callback(*this, WindowCallbackReason::Closing);
		if (m_hdl)
		{
			WindowCallbackEngine::unlink(this);
			glfwDestroyWindow((WindowHandle_t)m_hdl);
			m_hdl = nullptr;
		}
	}

	Window::operator bool() const
	{
		return should_close();
	}

}
