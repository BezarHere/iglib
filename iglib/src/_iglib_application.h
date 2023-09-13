#pragma once
#include <thread>
#include "_iglib_base.h"
#include "_iglib_application_config.h"
#include "_iglib_window.h"

namespace ig
{
	typedef void(*UpdateCallback_t)();
	typedef void(*DrawCallback_t)();

	class Application final
	{
	public:
		Application(ApplicationConfig config);
		~Application();

		Application(const Application &copy) = delete;
		Application(Application &&move) = delete;
		bool operator=(const Application &other) = delete;
		bool operator=(Application &&other) = delete;

		Window &create_window(int width, int height, const std::string &title, bool fullscreen);

		Window &get_primary_window();
		const Window &get_primary_window() const;

		// Window index 0 is always the primary window
		Window &get_window(size_t index);

		// Window index 0 is always the primary window
		const Window &get_window(size_t index) const;

	private:
		// calling _main will be undefined behivoure
		void _main();

	private:
		const uint32_t m_pid = 0xffffffff;
		std::vector<Window> m_windows{};
		UpdateCallback_t m_update_callback = nullptr;
		DrawCallback_t m_draw_callback = nullptr;
		std::unique_ptr<std::thread> m_running_thread;
	};
}
