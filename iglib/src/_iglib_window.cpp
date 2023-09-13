#include "pch.h"
#include "internal.h"
#include "_iglib_window.h"

namespace ig
{
	//Window::Window(Vector2i size, std::string title)
	//	: m_handle((void *)create_window(size.x, size.y, title))
	//{
	//	glCreateProgram();
	//}

	class Window::WindowCallbackEngine
	{
	public:
		WindowCallbackEngine()
		{
		}

		static void add(Window *window)
		{
			__WindowCallbackEngine.m_window.push_back(window);
		}

		static void pop(Window *window)
		{
			auto i = std::find(__WindowCallbackEngine.m_window.begin(), __WindowCallbackEngine.m_window.end(), window);
			
			if (i == __WindowCallbackEngine.m_window.end())
			{
				// does not exists
				// TODO: make something here
			}
			else
			{
				__WindowCallbackEngine.m_window.erase(i);
			}

		}

		static void remap_window_ptr(Window *window, WindowPtr_t data)
		{
			for (auto &kv : __WindowCallbackEngine.m_window_ptr_map)
			{
				if (kv.second == window)
				{
					__WindowCallbackEngine.m_window_ptr_map.erase(kv.first);
				}
			}

			__WindowCallbackEngine.m_window_ptr_map.insert_or_assign(data, window);
		}

	private:
		std::vector<Window *> m_window{};
		std::unordered_map<WindowPtr_t, Window *> m_window_ptr_map{};
	} __WindowCallbackEngine;





	Window::Window()
		: m_handle(nullptr),
			m_handle_rc{ new size_t{1} }
	{
	}

	Window::~Window()
	{
		if (*m_handle_rc.get() <= 1)
			glfwDestroyWindow((GLFWwindow *)m_handle);
		(*m_handle_rc)--;
	}

	Window::Window(const Window &copy)
		: m_handle{ copy.m_handle }
	{
		if (&copy == this)
			return;
		m_handle_rc = copy.m_handle_rc;
		(*m_handle_rc)++;
	}

	Window &Window::operator=(const Window &other)
	{
		if (&other == this)
			return *this;
		m_handle_rc = other.m_handle_rc;
		(*m_handle_rc)++;
		m_handle = other.m_handle;
		return *this;
	}

	bool Window::is_valid() const
	{
		return m_handle != nullptr;
	}

	bool Window::should_close() const
	{
		return glfwWindowShouldClose((GLFWwindow *)m_handle);
	}

	Vector2i Window::size() const
	{
		Vector2i i;
		glfwGetWindowSize((GLFWwindow *)m_handle, &i.x, &i.y);
		return i;
	}

	Vector2i Window::position() const
	{
		Vector2i i;
		glfwGetWindowPos((GLFWwindow *)m_handle, &i.x, &i.y);
		return i;
	}

	void Window::render()
	{
		glfwSwapBuffers((GLFWwindow *)m_handle);
	}

	void Window::poll()
	{
		glfwPollEvents();
	}

	void Window::clear()
	{
		glClearColor(0.0f, 0.0f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	Window::operator bool() const
	{
		return should_close();
	}

	Window::Window(void *const handle)
		: m_handle( handle ),
			m_handle_rc{ new size_t{1} }
	{
	}

}
