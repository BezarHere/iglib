#include "pch.h"
#include "internal.h"
#include "iglib.h"

bool g_glew_init = false;
bool glfw_init = false;
//GLuint glew_program_pid;

typedef void(*Action_t)(void);
static std::stack<WindowHandle_t> glfw_current_window_pipline_stack;
static std::vector<MonitorHandle_t> glfw_monitors{};
static std::vector<Action_t> openglinit_callbacks{};
static struct {
	int major, minor; bool compat;
} g_glversion = { 4, 1, false };

void static_init();

struct __static_init_run
{
	__static_init_run() {
		static_init();
	}
} __static_init_run_inst;


constexpr bool is_glversion_valid( int major, int minor ) {
	if (major == 3)
		return minor >= 0 && minor <= 3;
	return major == 4 && minor >= 0 && minor <= 6;
}

const std::vector<MonitorHandle_t> &get_monitors() {
	return glfw_monitors;
}

bool is_glew_running() {
	return g_glew_init;
}

bool is_glfw_running() {
	return glfw_init;
}

void monitor_callback( MonitorHandle_t monitor, int event ) {
	if (event == GLFW_CONNECTED)
	{
		glfw_monitors.push_back( monitor );
	}
	else if (event == GLFW_DISCONNECTED)
	{
		bool found = false;
		for (int i{}; i < glfw_monitors.size(); i++)
		{
			if (glfw_monitors[ i ] == monitor)
			{
				glfw_monitors.erase( glfw_monitors.begin() + i );
				found = true;
				break;
			}
		}

		if (!found)
		{
			std::stringstream ss{};
			ss << "Monitor " << monitor << " had a 'disconnected' event, but it doesn't exist in the connected monitors";
			raise( ss.str() );
		}

	}
}

void glfw_init_monitors() {
	int mon_count{};
	MonitorHandle_t *mons = glfwGetMonitors( &mon_count );
	glfw_monitors.clear();

	for (int i{}; i < mon_count; i++)
	{
		glfw_monitors.push_back( mons[ i ] );
	}

	glfwSetMonitorCallback( monitor_callback );
}

void glfw_init_window_hints() {
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, g_glversion.major );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, g_glversion.minor );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

// runs before 'main'
void static_init() {
}

void init_glfw() {
	if (!glfw_init)
	{
		int glfw_err = glfwInit();

		if (glfw_err == GLFW_FALSE)
		{
			std::cerr << "FAILED TO START GLFW\n";
			glfwerror( true );
		}

		glfw_init_monitors();
	}
	else
		warn( "Can't init glfw, already running!" );

	glfw_init = true;

	glewExperimental = GL_TRUE;

	// callbacks


	//glew_program_pid = __glewCreateProgram();

	//GLFWwindow *window = glfwCreateWindow(640, 480, "My Title", nullptr, nullptr);

	//glfwMakeContextCurrent(window);

}

void init_glew() {
	glewExperimental = GL_TRUE;

	if (!g_glew_init)
	{


		GLenum glew_err = glewInit();

		glfw_init_window_hints();

		if (glew_err != GLEW_OK)
		{
			raise(
				"glew error: " + std::string( (const char *)glewGetErrorString( glew_err ) )
			);
		}

		//lazyload_opengl_procs();

	}

	g_glew_init = true;
	for (const auto callback : openglinit_callbacks)
	{
		callback();
	}
	openglinit_callbacks.clear();

	// callbacks


	//glew_program_pid = __glewCreateProgram();

	//GLFWwindow *window = glfwCreateWindow(640, 480, "My Title", nullptr, nullptr);

	//glfwMakeContextCurrent(window);

}


void kill_glfw() {
	glfwTerminate();
	glfw_init = false;
}

void push_to_draw_pipline( WindowHandle_t hdl ) {
	NOTNULL( hdl );
	glfw_current_window_pipline_stack.push( hdl );
	glfwMakeContextCurrent( hdl );
}

WindowHandle_t top_draw_pipline() {
	if (glfw_current_window_pipline_stack.empty())
		return nullptr;
	return glfw_current_window_pipline_stack.top();
}

void pop_draw_pipline() {
	glfw_current_window_pipline_stack.pop();
	glfwMakeContextCurrent( top_draw_pipline() );
}

int register_openglinit_callback( void(*callback)(void) ) {
	openglinit_callbacks.push_back( callback );
	return 0;
}


bool set_glfw_context_version( int major, int minor, bool compat ) {
	if (!is_glversion_valid( major, minor ))
		return false;
	g_glversion = { major, minor, compat };
	return true;
}


