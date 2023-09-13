#pragma once
#include "_iglib_base.h"
#include "_iglib_vector.h"
#include <memory>

namespace ig
{
	enum class WindowCallbackReason
	{
		Resized,
		Moved,
		Focused,
		Unfocused,
		Minmized,
		Maxmized,
		Closed
	};

	class Window;

	typedef void(*WindowCallback_t)(Window &window, WindowCallbackReason reason);

	class Window final
	{
		friend class Application;
	public:
		Window();
		//Window(Vector2i size, std::string title);
		~Window();

		Window(const Window &copy);

		Window &operator=(const Window &other);

		bool is_valid() const;

		bool should_close() const;
		Vector2i size() const;
		Vector2i position() const;

		void render();
		void poll();
		void clear();

		operator bool() const;

	private:
		Window(void *const handle);

		class WindowCallbackEngine;

	private:
		void *m_handle;
		std::shared_ptr<size_t> m_handle_rc;

		WindowCallback_t m_callback = nullptr;
	};
}
