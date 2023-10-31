#include "pch.h"
#include "internal.h"

bool g_glew_init = false;
bool glfw_init = false;
//GLuint glew_program_pid;

std::stack<WindowHandle_t> glfw_current_window_pipline_stack;

std::vector<MonitorHandle_t> glfw_monitors{};
void static_init();

struct __static_init_run
{
	__static_init_run()
	{
		static_init();
	}
} __static_init_run_inst;

GLFWwindow *create_window(int width, int height, const std::string &title, GLFWmonitor *fullscreen, GLFWwindow *share)
{
	if (!is_glfw_running())
	{
		init_glfw();
	}


	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


	GLFWwindow *hdl = glfwCreateWindow(
		width, height, title.c_str(), fullscreen, share
	);

	if (!hdl)
		glfwerror(true);



	if (!is_glew_running())
	{
		glfwMakeContextCurrent(hdl);
		init_glew();
	}

	return hdl;
}

const std::vector<MonitorHandle_t> &get_monitors()
{
	return glfw_monitors;
}

bool is_glew_running()
{
	return g_glew_init;
}

bool is_glfw_running()
{
	return glfw_init;
}

void monitor_callback(MonitorHandle_t monitor, int event)
{
	if (event == GLFW_CONNECTED)
	{
		glfw_monitors.push_back(monitor);
	}
	else if (event == GLFW_DISCONNECTED)
	{
		bool found = false;
		for (int i{}; i < glfw_monitors.size(); i++)
		{
			if (glfw_monitors[ i ] == monitor)
			{
				glfw_monitors.erase(glfw_monitors.begin() + i);
				found = true;
				break;
			}
		}

		if (!found)
		{
			std::stringstream ss{};
			ss << "Monitor " << monitor << " had a 'disconnected' event, but it doesn't exist in the connected monitors";
			raise(ss.str());
		}

	}
}

void glfw_init_monitors()
{
	int mon_count{};
	MonitorHandle_t *mons = glfwGetMonitors(&mon_count);
	glfw_monitors.clear();

	for (int i{}; i < mon_count; i++)
	{
		glfw_monitors.push_back(mons[ i ]);
	}

	glfwSetMonitorCallback(monitor_callback);
}

void glfw_init_window_hints()
{
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

// runs before 'main'
void static_init()
{
}

void init_glfw()
{
	if (!glfw_init)
	{
		int glfw_err = glfwInit();

		if (glfw_err == GLFW_FALSE)
		{
			std::cerr << "FAILED TO START GLFW\n";
			glfwerror(true);
		}

		glfw_init_monitors();
		glfw_init_window_hints();
	}
	else
		warn("Can't init glfw, already running!");

	glfw_init = true;

	glewExperimental = GL_TRUE;

	// callbacks
	

	//glew_program_pid = __glewCreateProgram();

	//GLFWwindow *window = glfwCreateWindow(640, 480, "My Title", nullptr, nullptr);

	//glfwMakeContextCurrent(window);

}

void init_glew()
{
	glewExperimental = GL_TRUE;

	if (!g_glew_init)
	{
		

		GLenum glew_err = glewInit();
		if (glew_err != GLEW_OK)
		{
			raise(
				"glew error: " + std::string((const char *)glewGetErrorString(glew_err))
			);
		}

		//lazyload_opengl_procs();

	}

	g_glew_init = true;

	// callbacks
	

	//glew_program_pid = __glewCreateProgram();

	//GLFWwindow *window = glfwCreateWindow(640, 480, "My Title", nullptr, nullptr);

	//glfwMakeContextCurrent(window);

}


void kill_glfw()
{
	glfwTerminate();
	glfw_init = false;
}

void push_to_draw_pipline(WindowHandle_t hdl)
{
	NOTNULL(hdl);
	glfw_current_window_pipline_stack.push(hdl);
	glfwMakeContextCurrent(hdl);
}

WindowHandle_t top_draw_pipline()
{
	if (glfw_current_window_pipline_stack.empty())
		return nullptr;
	return glfw_current_window_pipline_stack.top();
}

void pop_draw_pipline()
{
	glfw_current_window_pipline_stack.pop();
	glfwMakeContextCurrent(top_draw_pipline());
}

void lazyload_opengl_procs()
{
	if (__glewCreateProgram == NULL)
	{
		PROC glcreateporgram = wglGetProcAddress("__glewCreateProgram");
		if (glcreateporgram == nullptr || glcreateporgram == (PROC)0x1 || glcreateporgram == (PROC)0x2 || glcreateporgram == (PROC)0x3 || glcreateporgram == (PROC)-1)
		{
			HMODULE opengl32 = LoadLibraryA("opengl32.dll");
			ASSERT(opengl32 != NULL);
			glcreateporgram = (PROC)GetProcAddress(opengl32, "__glewCreateProgram");
		}
	}
}


