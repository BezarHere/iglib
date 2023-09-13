#include "pch.h"
#include "_iglib_application.h"
#include "internal.h"

PROC glcreateporgram = nullptr;

class _AppGlobal
{
public:
	_AppGlobal()
	{
		glcreateporgram = wglGetProcAddress("glCreateProgram");
		if (glcreateporgram == nullptr || glcreateporgram == (PROC)0x1 || glcreateporgram == (PROC)0x2 || glcreateporgram == (PROC)0x3 || glcreateporgram == (PROC)-1)
		{
			HMODULE opengl32 = LoadLibraryA("opengl32.dll");
			glcreateporgram = (PROC)GetProcAddress(opengl32, "glCreateProgram");
		}
	}

	inline GLuint register_application(Application *app)
	{
		if (m_current_app)
			raise("can't run an application while there is already an application running.");
		if (!is_glfw_running())
		{
			init_glfw();
		}
		m_current_app = app;


		return glcreateporgram ? glcreateporgram() : 0;
	}

	inline void clear_application()
	{
		m_current_app = nullptr;
		kill_glfw();
	}

	inline Application *const app()
	{
		return m_current_app;
	}

	inline const Application *const app() const
	{
		return m_current_app;
	}

private:
	Application *m_current_app = nullptr;

} global_app;





namespace ig
{
	Application::Application(ApplicationConfig config)
		:
			m_pid{ global_app.register_application(this) },
			m_running_thread{ config.main_threaded ? nullptr : new std::thread() },
			m_windows{}
	{
		m_windows.reserve(256);
		create_window(config.size.x, config.size.y, config.title, config.fullscreen);
	}

	Application::~Application()
	{
		global_app.clear_application();
	}

	Window &Application::create_window(int width, int height, const std::string &title, bool fullscreen)
	{
		GLFWwindow *hdl = ::create_window(
			width, height,
			title,
			fullscreen ? glfwGetPrimaryMonitor() : nullptr,
			m_windows.empty() ? nullptr : (GLFWwindow *)m_windows[ 0 ].m_handle
		);

		if (!hdl)
			glfwerror(true);

		const bool primary = m_windows.empty();

		m_windows.push_back(Window((void *const)hdl));

		if (primary)
		{
			
			if (!is_glew_running())
			{
				glfwMakeContextCurrent(hdl);

				init_glew();
			}
			
		}

		return m_windows[m_windows.size() - 1];
	}

	Window &Application::get_primary_window()
	{
		return m_windows[ 0 ];
	}

	const Window &Application::get_primary_window() const
	{
		return m_windows[ 0 ];
	}

	Window &Application::get_window(size_t index)
	{
		return m_windows[ index ];
	}

	const Window &Application::get_window(size_t index) const
	{
		return m_windows[ index ];
	}

	void Application::_main()
	{
		// update

		// draw

		// late update

	}

}
