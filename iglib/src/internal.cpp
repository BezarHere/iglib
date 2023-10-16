#include "pch.h"
#include "internal.h"

bool glew_init = false;
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
	GLFWwindow *hdl = glfwCreateWindow(
		width, height, title.c_str(), fullscreen, share
	);

	if (!hdl)
		glfwerror(true);


	return hdl;
}

const std::vector<MonitorHandle_t> &get_monitors()
{
	return glfw_monitors;
}

bool is_glew_running()
{
	return glew_init;
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

void init_monitors()
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

		init_monitors();
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

	if (glew_init)
	{
		GLenum glew_err = glewInit();
		if (glew_err != GLEW_OK)
		{
			raise(
				"glew error: " + std::string((const char *)glewGetErrorString(glew_err))
			);
		}
	}
	//else
	//	warn("Can't init glew, already running!");

	glew_init = true;

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

