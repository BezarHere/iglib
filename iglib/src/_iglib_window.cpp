#include "pch.h"
#include "internal.h"
#include "_iglib_window.h"

static Vertex2D g_ScreenQuadVertcies[ 4 ]
{
	{ {0.f, 0.f}, {1.f, 1.f, 1.f, 1.f}, {0.f, 1.f} }, // topleft
	{ {0.f, 1.f}, {1.f, 1.f, 1.f, 1.f}, {0.f, 0.f} }, // bottomleft
	{ {1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}, {1.f, 0.f} }, // bottomright
	{ {1.f, 0.f}, {1.f, 1.f, 1.f, 1.f}, {1.f, 1.f} } // topright
};

static Vertex2DBuffer g_ScreenQuadBuffer{};
static ShaderInstance_t g_ScreenShader{};
static GLFWwindow *create_glfw_window(int width, int height, const std::string &title, GLFWmonitor *fullscreen, GLFWwindow *share)
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



	if (!is_glew_running())
	{
		glfwMakeContextCurrent(hdl);
		init_glew();
	}


	if (!g_ScreenShader)
	{
		static const std::string PostProcessing_Vert =
			"void main() { gl_Position = vec4(to_native_space(pos), 0.0, 1.0); UV = texcoord; }";

		static const std::string PostProcessing_Frag =
			"void main() {"
				"Color = vec4(texture(uTex0, UV).rgb, 1.0);"
				//"Color = vec4(UV, 1.0, 1.0);"
			"}";

		g_ScreenQuadBuffer.set_primitive(PrimitiveType::Quad);
		g_ScreenQuadBuffer.set_usage(BufferUsage::Dynamic);
		g_ScreenQuadBuffer.create(4, nullptr);
		g_ScreenShader = Shader::compile(
			PostProcessing_Vert, PostProcessing_Frag, ShaderUsage::ScreenSpace
		);
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


#pragma region(Window registry stuff)
	Window *g_main_window;
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
			static void mouse_button(WindowHandle_t hdl, int button, int action, int mods);

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

			if (!g_main_window)
			{
				g_main_window = window;
			}

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
				// window does not exists
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
			(void)glfwSetMouseButtonCallback(hdl, WindowCallbacksRouter::mouse_button);
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
			(void)glfwSetMouseButtonCallback(hdl, nullptr);
		}

		static void pop_weak(Window *window, WindowHandle_t hdl)
		{
			NOTNULL(window);
			NOTNULL(window->m_hdl);

			if (g_main_window == window)
			{
				g_main_window = nullptr;
			}

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
			/*else if (window->m_deffered_close)
			{
				window->close();
			}*/

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
		push_to_draw_pipline(hdl);
		glViewport(0, 0, w, h);
		pop_draw_pipline();
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
		(scancode);
		Window *window = WindowCallbackEngine::get_window(hdl);
		if (window->m_key_callback)
			window->m_key_callback(*window, (Key)key, (KeyAction)action, (KeyModFlags)mods);
		if (key == GLFW_KEY_F4 && mods & GLFW_MOD_ALT)
			WindowCallbacksRouter::requested_close(hdl);
	}

	void Window::WindowCallbackEngine::WindowCallbacksRouter::mouse_button(WindowHandle_t hdl, int button, int action, int mods)
	{
		Window *window = WindowCallbackEngine::get_window(hdl);
		if (window->m_mouse_callback)
			window->m_mouse_callback(*window, (MouseButton)button, (KeyAction)action, (KeyModFlags)mods);
	}
#pragma endregion

	struct Window::WindowDrawBuffer
	{
		static constexpr GLuint RenderTextureType = GL_RGB;
		static constexpr GLuint RenderDepthStencilMode = GL_DEPTH24_STENCIL8;

		~WindowDrawBuffer()
		{
			glDeleteFramebuffers(1, &fbo);
			glDeleteRenderbuffers(1, &rbo);
			glDeleteTextures(1, &cto);
		}

		FORCEINLINE static std::unique_ptr<WindowDrawBuffer> generate(Vector2i size)
		{
			GLuint fbo = 0, rbo = 0, cto = 0;
			
			glGenFramebuffers(1, &fbo);

			REPORT(fbo == NULL);
			if (fbo == NULL)
			{
				return std::unique_ptr<WindowDrawBuffer>(nullptr);
			}

			glGenTextures(1, &cto);

			REPORT(cto == NULL);
			if (cto == NULL)
			{
				glDeleteFramebuffers(1, &fbo);
				return std::unique_ptr<WindowDrawBuffer>(nullptr);
			}

			glBindTexture(GL_TEXTURE_2D, cto);
			glTexImage2D(GL_TEXTURE_2D, 0, RenderTextureType, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


			glGenRenderbuffers(1, &rbo);

			REPORT(rbo == NULL);
			if (rbo == NULL)
			{
				glDeleteFramebuffers(1, &fbo);
				glDeleteTextures(1, &cto);
				return std::unique_ptr<WindowDrawBuffer>(nullptr);
			}


			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, RenderDepthStencilMode, size.x, size.y);

			

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cto, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

			REPORT(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE); // <- BUGBUG

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NULL);
			glBindRenderbuffer(GL_RENDERBUFFER, NULL);
			glBindTexture(GL_TEXTURE_2D, NULL);

			return std::unique_ptr<WindowDrawBuffer>(new WindowDrawBuffer{ fbo, rbo, cto, size });
		}

		const GLuint fbo = 0, rbo = 0, cto = 0;
		const Vector2i ctosz{};
	};

	Window::Window() noexcept
		: m_hdl(nullptr),
		m_visible_state{ WindowVisibiltyState::Restored },
		m_focused{ false },
		m_hidden{ false },
		m_creation_time{ TimeMs_t::duration(TimeMs_t::clock::now().time_since_epoch().count()) },
		m_stp{ (float)glfwGetTime() },
		m_drawbuffer{ nullptr }
	{
		refresh_rect();
	}

	Window::Window(Vector2i size, std::string title) noexcept
		: Window(
				create_glfw_window( size.x, size.y, title, nullptr,
					g_main_window ? (GLFWwindow*)g_main_window->m_hdl : nullptr
				),
				title,
				false)
	{
	}

	Window::Window(Vector2i size) noexcept
		: Window(size, "Window")
	{
	}
		
	Window::Window(void *const handle, const std::string &title, bool hidden) noexcept
		: m_hdl(handle),
			m_visible_state{ WindowVisibiltyState::Minimized },
			m_focused{ false },
			m_title{ title },
			m_hidden{ hidden },
			m_creation_time{ TimeMs_t::duration(TimeMs_t::clock::now().time_since_epoch().count()) },
			m_stp{ (float)glfwGetTime() },
			m_drawbuffer{ nullptr }
	{
		if (handle == nullptr)
		{
			warn("NULL window handle passed to a window ctor.");
			glfwerror(true); // <- if there is any glfw error, this will raise
			return;
		}
		WindowCallbackEngine::link(this);
		refresh_rect();
		m_drawbuffer.reset(WindowDrawBuffer::generate(get_size()).release());
	}


	Window::Window(Window &&move) noexcept
		: m_hdl(move.m_hdl),
		m_visible_state{ move.m_visible_state },
		m_focused{ move.m_focused },
		m_title(move.m_title),
		m_hidden{ move.m_hidden },
		m_stp{ move.m_stp },
		m_mouse_callback{ move.m_mouse_callback },
		m_drawbuffer{ move.m_drawbuffer.release() }
	{
		if (move.m_hdl == nullptr)
			return;
		WindowCallbackEngine::unlink(&move);
		move.m_hdl = nullptr;
		WindowCallbackEngine::link(this);
		refresh_rect();
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

	Window &Window::operator=(Window &&other) noexcept
	{
		//m_handle_rc = other.m_handle_rc;
		//(*m_handle_rc)++;
		if (other.m_hdl == nullptr)
		{
			m_hdl = nullptr;
			return *this;
		}

		m_focused = other.m_focused;
		m_callback = other.m_callback;
		m_content_scale = other.m_content_scale;
		m_rect = other.m_rect;
		m_frambeuffer_size = other.m_frambeuffer_size;
		m_visible_state = other.m_visible_state;
		m_mouse_callback = other.m_mouse_callback;

		WindowHandle_t old_hdl = (WindowHandle_t)m_hdl;
		m_hdl = other.m_hdl;
		WindowCallbackEngine::unlink(&other);
		other.m_hdl = nullptr;
		WindowCallbackEngine::remap(this, old_hdl);
		refresh_rect();
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

	int Window::get_width() const
	{
		return m_rect.w;
	}

	int Window::get_height() const
	{
		return m_rect.h;
	}

	Vector2i Window::get_size() const
	{
		return m_rect.size();
	}

	Vector2i Window::get_position() const
	{
		return m_rect.position();
	}

	const Recti &Window::get_rect() const
	{
		return m_rect;
	}

	void Window::set_size(Vector2i size)
	{
		glfwSetWindowSize((GLFWwindow *)m_hdl, size.x, size.y);
	}

	void Window::set_position(Vector2i size)
	{
		glfwSetWindowPos((GLFWwindow *)m_hdl, size.x, size.y);
	}

	void Window::refresh_rect()
	{
		glfwGetWindowSize((GLFWwindow *)m_hdl, &m_rect.w, &m_rect.h);
		glfwGetWindowPos((GLFWwindow *)m_hdl, &m_rect.x, &m_rect.y);
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

	void Window::set_mouse_callback(MouseCallback_t callback) noexcept
	{
		m_mouse_callback = callback;
	}
	
	MouseCallback_t Window::get_mouse_callback() const noexcept
	{
		return m_mouse_callback;
	}

	void Window::render()
	{
		const Vector2i sz = get_size();
		// MINIMIZED, no drawing
		if (sz.area() == 0)
		{
			return;
		}
		push_to_draw_pipline((WindowHandle_t)m_hdl);
		// TODO: Invoke a query to update size, size is still updated by the resize callback but still

		if (sz != m_drawbuffer->ctosz)
		{
			m_drawbuffer.reset(WindowDrawBuffer::generate(sz).release());
		}


		//glDisable(GL_DEPTH);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_SCISSOR_TEST);

		glScissor(0, 0, sz.x, sz.y);

		const bool postprocessing = m_drawbuffer.get() != nullptr;
		if (postprocessing)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, m_drawbuffer->rbo);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_drawbuffer->fbo);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.f, get_width(), get_height(), 0.f, 0.f, 1.f);

		Canvas canvas{ *this };
		if (m_draw_callback)
			m_draw_callback(canvas);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NULL);
		glBindRenderbuffer(GL_RENDERBUFFER, NULL);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_SCISSOR_TEST);

		if (postprocessing)
		{
			g_ScreenQuadVertcies[ 1 ].pos.y = (float)sz.y;
			g_ScreenQuadVertcies[ 2 ].pos = Vector2f(sz);
			g_ScreenQuadVertcies[ 3 ].pos.x = (float)sz.x;
			g_ScreenQuadBuffer.update(g_ScreenQuadVertcies);

			canvas.bind_shader(g_ScreenShader);
			canvas.set_texture(m_drawbuffer->cto);
			canvas.draw(g_ScreenQuadBuffer);


		}

		pop_draw_pipline();
		glfwSwapBuffers((GLFWwindow *)m_hdl);
	}

	void Window::poll()
	{
		push_to_draw_pipline((WindowHandle_t)m_hdl);
		glfwPollEvents();
		pop_draw_pipline();
	}

	void Window::clear()
	{
		push_to_draw_pipline((WindowHandle_t)m_hdl);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pop_draw_pipline();
	}

	bool Window::is_deffered_to_close() const noexcept
	{
		return m_deffered_close;
	}

	bool Window::is_focused() const noexcept
	{
		return m_focused;
	}

	void Window::set_resizable(bool value)
	{
		glfwSetWindowAttrib((GLFWwindow *)m_hdl, GLFW_RESIZABLE, value);
	}

	bool Window::is_resizable() const
	{
		
		return glfwGetWindowAttrib((GLFWwindow *)m_hdl, GLFW_RESIZABLE);
	}

	void Window::set_decorated(bool value)
	{
		glfwSetWindowAttrib((GLFWwindow *)m_hdl, GLFW_DECORATED, value);
	}

	bool Window::is_decorated() const
	{
		return glfwGetWindowAttrib((GLFWwindow *)m_hdl, GLFW_DECORATED);
	}

	void Window::set_draw_callback(DrawCallback callback) noexcept
	{
		m_draw_callback = callback;
	}

	DrawCallback Window::get_draw_callback() const noexcept
	{
		return m_draw_callback;
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

	Image Window::to_image(const Recti rect) const
	{
		std::unique_ptr<byte[]> data{ new byte[ get_size().area() * Channels::RGB ] };
		glReadPixels(rect.x, rect.y, rect.w, rect.h, GL_RGB, GL_UNSIGNED_BYTE, data.get());
		return { data.get(), get_size(), Channels::RGB };
	}

	Image Window::to_image() const
	{
		return to_image({ 0, 0, get_width(), get_height() });
	}

	TimeMs_t Window::get_creation_time() const noexcept
	{
		return m_creation_time;
	}

	float Window::get_shader_time() const noexcept
	{
		return (float)glfwGetTime() - m_stp;
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
