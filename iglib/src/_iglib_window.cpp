#include "pch.h"
#include "internal.h"
#include "_iglib_window.h"

constexpr std::array<const char *, 6> DebugSources =
{
	"API", "WindowSys", "ShaderCompiler", "3rdParty", "Application", "Other"
};

constexpr std::array<const char *, 9> DebugMsgTypes =
{
	"Error", "DeprecatedBehavior", "UndefinedBehavior", "Portability", "Performance", "Marker",
	"PushGroup", "PopGroup", "Other"
};

constexpr std::array<const char *, 4> DebugMsgSeverity =
{
	"High", "Med", "Low", "None"
};

constexpr FORCEINLINE const char *gldbg_get_severity( const GLenum s ) {
	if (s == GL_DEBUG_SEVERITY_NOTIFICATION)
		return DebugMsgSeverity[ 3 ];
	return DebugMsgSeverity[ s - GL_DEBUG_SEVERITY_HIGH ];
}

void APIENTRY debug_callback( GLenum source, GLenum type, GLuint id,
															GLenum severity, GLsizei length, const GLchar *message, const void *userParam ) {
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return; // skips notifications
	(void)userParam;
	(void)id;
	bite::warn(
		"Message from "
		+ std::string( DebugSources[ source - GL_DEBUG_SOURCE_API ] )
		+ " severity "
		+ gldbg_get_severity( severity )
		+ " type "
		+ DebugMsgTypes[ type - GL_DEBUG_TYPE_ERROR ] // <- works for now
		+ ": "
		+ std::string( message, length )
	);
}

static GLFWwindow *create_glfw_window( int width, int height, const std::string &title, GLFWmonitor *fullscreen, GLFWwindow *share ) {
	if (!is_glfw_running())
	{
		init_glfw();
	}

	glfw_init_window_hints();
	GLFWwindow *hdl = glfwCreateWindow(
		width, height, title.c_str(), fullscreen, share
	);



	if (!is_glew_running())
	{
		glfwMakeContextCurrent( hdl );
		init_glew();
#ifdef _DEBUG
		glEnable( GL_DEBUG_OUTPUT );
		glDebugMessageCallback( debug_callback, nullptr );
#endif // _DEBUG

	}

	return hdl;
}

namespace ig
{
	//Window::Window(Vector2i size, std::string title)
	//	: m_hdl((void *)create_glfw_window(size.x, size.y, title))
	//{
	//	glCreateProgram();
	//}

	constexpr size_t InputEventSizeOf = sizeof( InputEvent );
	static_assert(sizeof( InputEvent ) <= sizeof( int ), "input event should smaller then/equal to an int");

#pragma region(Window registry stuff)
	Window *g_main_window;
	std::vector<Window *> wce_available_windows;
	std::unordered_map<WindowHandle_t, Window *> wce_handles_map;

	class Window::WindowCallbackEngine
	{
		class WindowCallbacksRouter
		{
			friend class Window::WindowCallbackEngine;


			static void moved( WindowHandle_t hdl, int x, int y );
			static void resized( WindowHandle_t hdl, int w, int h );


			static void requested_close( WindowHandle_t hdl );
			static void dirty( WindowHandle_t hdl );

			static void focus_changed( WindowHandle_t hdl, GLFWbool_t focused );
			static void minimized( WindowHandle_t hdl, GLFWbool_t is_minimized );
			static void maximized( WindowHandle_t hdl, GLFWbool_t is_maximized );

			static void framebuffer_resized( WindowHandle_t hdl, int w, int h );
			static void contents_rescaled( WindowHandle_t hdl, float xfactor, float yfactor );

			static void key_pressed( WindowHandle_t hdl, int key, int scancode, int action, int mods );
			static void mouse_button( WindowHandle_t hdl, int button, int action, int mods );
			static void mouse_scroll( WindowHandle_t hdl, double x, double y );
		};


	public:
		WindowCallbackEngine() {
			WindowCallbackEngine::init();
		}


		static void link( Window *window ) {
			NOTNULL( window );
			NOTNULL( window->m_hdl );
			if (has_hdl( (WindowHandle_t)window->m_hdl ))
				raise( "More then one window have the same handle, pease review you code for mis-use with window handles." );

			if (!g_main_window)
			{
				g_main_window = window;
			}

			wce_available_windows.push_back( window );
			wce_handles_map[ (WindowHandle_t)window->m_hdl ] = window;
			connect_callbacks( (WindowHandle_t)window->m_hdl );
		}

		static void unlink( Window *window ) {
			pop_weak( window, (WindowHandle_t)window->m_hdl );
		}

		static void remap( Window *window, WindowHandle_t old_hdl ) {
			NOTNULL( window );

			// more like a reset
			pop_weak( window, old_hdl );

			link( window );
		}

		// -------------------- PRIVATE --------------------
	private:

		static void init() {

		}

		static bool has_hdl( WindowHandle_t hdl ) {
			return wce_handles_map.find( hdl ) != wce_handles_map.end();
		}

		static void remove_from_handle_ptr_map( WindowHandle_t hdl ) {
			NOTNULL( hdl );
			wce_handles_map.erase( hdl );
		}

		static void remove_from_handle_ptr_map( Window *window ) {
			NOTNULL( window );

			for (auto &kv : wce_handles_map)
			{
				if (kv.second == window)
				{
					remove_from_handle_ptr_map( kv.first );
					break;
				}
			}
		}

		static void remove_from_listed_windows( Window *window ) {
			NOTNULL( window );

			auto i = std::find( wce_available_windows.begin(), wce_available_windows.end(), window );

			if (i == wce_available_windows.end())
			{
				// window does not exists
				// TODO: make something here
			}
			else
			{
				wce_available_windows.erase( i );
			}
		}

		static inline void connect_callbacks( WindowHandle_t hdl ) {
			NOTNULL( hdl );
			(void)glfwSetWindowSizeCallback( hdl, WindowCallbacksRouter::resized );
			(void)glfwSetWindowPosCallback( hdl, WindowCallbacksRouter::moved );

			(void)glfwSetWindowCloseCallback( hdl, WindowCallbacksRouter::requested_close );
			(void)glfwSetWindowRefreshCallback( hdl, WindowCallbacksRouter::dirty );

			(void)glfwSetWindowFocusCallback( hdl, WindowCallbacksRouter::focus_changed );
			(void)glfwSetWindowIconifyCallback( hdl, WindowCallbacksRouter::minimized );
			(void)glfwSetWindowMaximizeCallback( hdl, WindowCallbacksRouter::maximized );

			(void)glfwSetFramebufferSizeCallback( hdl, WindowCallbacksRouter::framebuffer_resized );
			(void)glfwSetWindowContentScaleCallback( hdl, WindowCallbacksRouter::contents_rescaled );


			(void)glfwSetKeyCallback( hdl, WindowCallbacksRouter::key_pressed );
			(void)glfwSetMouseButtonCallback( hdl, WindowCallbacksRouter::mouse_button );
			(void)glfwSetScrollCallback( hdl, WindowCallbacksRouter::mouse_scroll );
		}

		static inline void disconnect_callbacks( WindowHandle_t hdl ) {
			NOTNULL( hdl );
			(void)glfwSetWindowSizeCallback( hdl, nullptr );
			(void)glfwSetWindowPosCallback( hdl, nullptr );

			(void)glfwSetWindowCloseCallback( hdl, nullptr );
			(void)glfwSetWindowRefreshCallback( hdl, nullptr );

			(void)glfwSetWindowFocusCallback( hdl, nullptr );
			(void)glfwSetWindowIconifyCallback( hdl, nullptr );
			(void)glfwSetWindowMaximizeCallback( hdl, nullptr );

			(void)glfwSetFramebufferSizeCallback( hdl, nullptr );
			(void)glfwSetWindowContentScaleCallback( hdl, nullptr );


			(void)glfwSetKeyCallback( hdl, nullptr );
			(void)glfwSetMouseButtonCallback( hdl, nullptr );
			(void)glfwSetScrollCallback( hdl, nullptr );
		}

		static void pop_weak( Window *window, WindowHandle_t hdl ) {
			NOTNULL( window );
			NOTNULL( window->m_hdl );

			if (g_main_window == window)
			{
				g_main_window = nullptr;
			}

			remove_from_listed_windows( window );

			disconnect_callbacks( hdl );

			remove_from_handle_ptr_map( hdl );
			remove_from_handle_ptr_map( window );
		}

		static inline Window *get_window( const WindowHandle_t hdl ) {
			NOTNULL( hdl );
			return wce_handles_map.at( hdl );
		}

		static inline void recall_command( WindowHandle_t hdl, WindowCallbackReason reason ) {
			Window *window = get_window( hdl );
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
				window->m_deferred_close = true;
				break;
			default:
				break;
			}
			if (window->m_callback)
				window->m_callback( *window, reason );
			/*else if (window->m_deferred_close)
			{
				window->close();
			}*/

		}

		static inline void recall_command( WindowHandle_t hdl, WindowCallbackReason reason, int a, int b ) {
			Window *window = get_window( hdl );

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
				//case ig::WindowCallbackReason::ResizedFramebuffer:
				//	window->m_frambeuffer_size.set(a, b);
				//	break;
			default:
				break;
			}

			if (window->m_callback)
				window->m_callback( *window, reason );
		}

		static inline void recall_command( WindowHandle_t hdl, WindowCallbackReason reason, float a, float b ) {
			Window *window = get_window( hdl );

			window->m_content_scale.set( a, b );

			if (window->m_callback)
				window->m_callback( *window, reason );
		}
	} __WindowCallbackEngine_Dummy;


	void Window::WindowCallbackEngine::WindowCallbacksRouter::moved( WindowHandle_t hdl, int x, int y ) {
		WindowCallbackEngine::recall_command( hdl, WindowCallbackReason::Moved, x, y );
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::resized( WindowHandle_t hdl, int w, int h ) {
		push_to_draw_pipline( hdl );
		glViewport( 0, 0, w, h );
		pop_draw_pipline();
		WindowCallbackEngine::recall_command( hdl, WindowCallbackReason::Resized, w, h );
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::requested_close( WindowHandle_t hdl ) {
		WindowCallbackEngine::recall_command( hdl, WindowCallbackReason::RequestedClose );
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::dirty( WindowHandle_t hdl ) {
		WindowCallbackEngine::recall_command( hdl, WindowCallbackReason::DirtyScreen );
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::focus_changed( WindowHandle_t hdl, GLFWbool_t focused ) {
		if (focused)
			WindowCallbackEngine::recall_command( hdl, WindowCallbackReason::Focused );
		else
			WindowCallbackEngine::recall_command( hdl, WindowCallbackReason::Unfocused );
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::minimized( WindowHandle_t hdl, GLFWbool_t is_minimized ) {
		if (is_minimized)
			WindowCallbackEngine::recall_command( hdl, WindowCallbackReason::Minimized );
		else
			WindowCallbackEngine::recall_command( hdl, WindowCallbackReason::Restored );
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::maximized( WindowHandle_t hdl, GLFWbool_t is_maximized ) {
		if (is_maximized)
			WindowCallbackEngine::recall_command( hdl, WindowCallbackReason::Maxmized );
		else
			WindowCallbackEngine::recall_command( hdl, WindowCallbackReason::Restored );
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::framebuffer_resized( WindowHandle_t hdl, int w, int h ) {
		WindowCallbackEngine::recall_command( hdl, WindowCallbackReason::ResizedFramebuffer, w, h );
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::contents_rescaled( WindowHandle_t hdl, float xfactor, float yfactor ) {
		WindowCallbackEngine::recall_command( hdl, WindowCallbackReason::RescaledContents, xfactor, yfactor );
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::key_pressed( WindowHandle_t hdl, int key, int scancode, int action, int mods ) {
		(void)(scancode);
		Window *window = WindowCallbackEngine::get_window( hdl );

		if (window->m_input_callback)
		{
			InputEvent event;
			event.key = { KeyCode( key ), InputAction( action ), InputModFlags( mods ) };

			window->m_input_callback( *window, event, InputEventType::Key );
		}

		if (key == GLFW_KEY_F4 && mods & GLFW_MOD_ALT)
		{
			WindowCallbacksRouter::requested_close( hdl );
		}
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::mouse_button( WindowHandle_t hdl, int button, int action, int mods ) {
		Window *window = WindowCallbackEngine::get_window( hdl );
		if (window->m_input_callback)
		{
			InputEvent event;
			event.mouse_button = { MouseButton( button ), InputAction( action ), InputModFlags( mods ) };

			window->m_input_callback( *window, event, InputEventType::MouseButton );
		}
	}

	// i had known these doubles will haunt me someday
	void Window::WindowCallbackEngine::WindowCallbacksRouter::mouse_scroll( WindowHandle_t hdl, double x, double y ) {
		Window *window = WindowCallbackEngine::get_window( hdl );

		if (window->m_input_callback)
		{
			InputEvent event;
			event.mouse_scroll = { static_cast<int16_t>(x), static_cast<int16_t>(y) };

			window->m_input_callback( *window, event, InputEventType::MouseScrollWheel );
		}

	}

#pragma endregion



	Window::Window() noexcept
		: m_hdl( nullptr ),
		m_visible_state{ WindowVisibiltyState::Restored },
		m_focused{ false },
		m_hidden{ false },
		m_creation_time{ TimeMs_t::duration( TimeMs_t::clock::now().time_since_epoch().count() ) },
		m_stp{ (float)glfwGetTime() } {
		refresh_rect();
	}

	Window::Window( Vector2i size, std::string title ) noexcept
		: Window(
			create_glfw_window( size.x, size.y, title, nullptr,
													g_main_window ? (GLFWwindow *)g_main_window->m_hdl : nullptr
			),
			title,
			false ) {
	}

	Window::Window( Vector2i size ) noexcept
		: Window( size, "Window" ) {
	}

	Window::Window( void *const handle, const std::string &title, bool hidden ) noexcept
		: m_hdl( handle ),
		m_visible_state{ WindowVisibiltyState::Minimized },
		m_focused{ false },
		m_title{ title },
		m_hidden{ hidden },
		m_creation_time{ TimeMs_t::duration( TimeMs_t::clock::now().time_since_epoch().count() ) },
		m_stp{ (float)glfwGetTime() } {
		if (handle == nullptr)
		{
			warn( "NULL window handle passed to a window ctor." );
			glfwerror( true ); // <- if there is any glfw error, this will raise
			return;
		}
		WindowCallbackEngine::link( this );
		refresh_rect();
	}


	Window::Window( Window &&move ) noexcept
		: m_hdl( move.m_hdl ),
		m_visible_state{ move.m_visible_state },
		m_focused{ move.m_focused },
		m_title( move.m_title ),
		m_hidden{ move.m_hidden },
		m_stp{ move.m_stp },
		m_input_callback{ move.m_input_callback } {
		if (move.m_hdl == nullptr)
			return;
		WindowCallbackEngine::unlink( &move );
		move.m_hdl = nullptr;
		WindowCallbackEngine::link( this );
		refresh_rect();
	}

	Window::~Window() {
		if (m_hdl)
		{
			close();
		}
	}

	Window &Window::operator=( Window &&other ) noexcept {
		if (other.m_hdl == nullptr)
		{
			m_hdl = nullptr;
			return *this;
		}

		m_focused = other.m_focused;
		m_callback = other.m_callback;
		m_content_scale = other.m_content_scale;
		m_rect = other.m_rect;
		m_visible_state = other.m_visible_state;
		m_input_callback = other.m_input_callback;

		WindowHandle_t old_hdl = (WindowHandle_t)m_hdl;
		m_hdl = other.m_hdl;
		WindowCallbackEngine::unlink( &other );
		other.m_hdl = nullptr;
		WindowCallbackEngine::remap( this, old_hdl );
		refresh_rect();
		return *this;
	}

	bool Window::is_valid() const {
		return m_hdl != nullptr;
	}

	bool Window::should_close() const {
		return m_hdl == nullptr || m_deferred_close || glfwWindowShouldClose( (GLFWwindow *)m_hdl );
	}

	Vector2i Window::get_mouse_position() const {
		double x, y;
		glfwGetCursorPos( (WindowHandle_t)m_hdl, &x, &y );
		return Vector2i( (int)x, (int)y );
	}

	int Window::width() const {
		return m_rect.w;
	}

	int Window::height() const {
		return m_rect.h;
	}

	Vector2i Window::size() const {
		return m_rect.size();
	}

	Vector2i Window::get_position() const {
		return m_rect.position();
	}

	const Rect2i &Window::get_rect() const {
		return m_rect;
	}

	void Window::set_size( Vector2i size ) {
		glfwSetWindowSize( (GLFWwindow *)m_hdl, size.x, size.y );
	}

	void Window::set_position( Vector2i size ) {
		glfwSetWindowPos( (GLFWwindow *)m_hdl, size.x, size.y );
	}

	void Window::refresh_rect() {
		glfwGetWindowSize( (GLFWwindow *)m_hdl, &m_rect.w, &m_rect.h );
		glfwGetWindowPos( (GLFWwindow *)m_hdl, &m_rect.x, &m_rect.y );
	}

	const std::string &Window::get_title() const {
		return m_title;
	}

	void Window::set_title( const std::string &title ) {
		m_title = title;
		glfwSetWindowTitle( (WindowHandle_t)m_hdl, title.c_str() );
	}

	WindowCallback_t Window::get_callback() const {
		return m_callback;
	}

	void Window::set_callback( WindowCallback_t callback ) {
		m_callback = callback;
	}

	InputCallback_t Window::get_input_callback() const {
		return m_input_callback;
	}

	void Window::set_input_callback( InputCallback_t callback ) {
		m_input_callback = callback;
	}

	void Window::poll() {
		push_to_draw_pipline( (WindowHandle_t)m_hdl );
		glfwPollEvents();
		pop_draw_pipline();
	}

	bool Window::is_deferred_to_close() const noexcept {
		return m_deferred_close;
	}

	bool Window::is_focused() const noexcept {
		return m_focused;
	}

	void Window::set_resizable( bool value ) {
		glfwSetWindowAttrib( (GLFWwindow *)m_hdl, GLFW_RESIZABLE, value );
	}

	bool Window::is_resizable() const {

		return glfwGetWindowAttrib( (GLFWwindow *)m_hdl, GLFW_RESIZABLE );
	}

	void Window::set_decorated( bool value ) {
		glfwSetWindowAttrib( (GLFWwindow *)m_hdl, GLFW_DECORATED, value );
	}

	bool Window::is_decorated() const {
		return glfwGetWindowAttrib( (GLFWwindow *)m_hdl, GLFW_DECORATED );
	}

	void Window::hide() {
		glfwHideWindow( (WindowHandle_t)m_hdl );
		m_hidden = true;
	}

	void Window::show() {
		glfwShowWindow( (WindowHandle_t)m_hdl );
		m_hidden = false;
	}

	WindowVisibiltyState Window::get_visibility_state() const {
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

	void Window::ping() const noexcept {
		glfwRequestWindowAttention( (WindowHandle_t)m_hdl );
	}

	Image Window::to_image( const Rect2i rect ) const {
		std::unique_ptr<byte[]> data{ new byte[ size().area() * ColorFormat::RGB ] };
		glReadPixels( rect.x, rect.y, rect.w, rect.h, GL_RGB, GL_UNSIGNED_BYTE, data.get() );
		return { data.get(), size(), ColorFormat::RGB };
	}

	Image Window::to_image() const {
		return to_image( { 0, 0, width(), height() } );
	}

	TimeMs_t Window::get_creation_time() const noexcept {
		return m_creation_time;
	}

	float Window::get_shader_time() const noexcept {
		return (float)glfwGetTime() - m_stp;
	}

	void Window::close() noexcept {
		if (m_callback)
			m_callback( *this, WindowCallbackReason::Closing );
		if (m_hdl)
		{
			WindowCallbackEngine::unlink( this );
			glfwDestroyWindow( (WindowHandle_t)m_hdl );
			m_hdl = nullptr;
		}
	}

	//Vector2f Window::local_to_native(Vector2f pos) const
	//{
	//	const Vector2f wf = get_size();
	//	return Vector2f((pos.x * 2.0f / wf.x) - 1.0f, -((pos.y * 2.0f / wf.y) - 1.0f));
	//}

	//Vector2f Window::local_to_native(Vector2i pos) const
	//{
	//	const Vector2f wf = get_size();
	//	return Vector2f(((pos.x >> 1) / wf.x) - 1.0f, -(((pos.y >> 1) / wf.y) - 1.0f));
	//}

}
