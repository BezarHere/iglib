#pragma once
#include "_iglib_vector.h"
#include "_iglib_color.h"

namespace ig
{
	enum class Channels
	{
		Invalid = 0,
		L = 1,
		LA = 2,
		RGB = 3,
		RGBA = 4
	};

	class Image
	{
	public:
		Image();
		Image(const std::string &filename);
		~Image() noexcept;

		Image(Image &&move) noexcept;
		Image(const Image &copy) noexcept;
		Image &operator=(Image &&move) noexcept;
		Image &operator=(const Image &copy) noexcept;

		Vector2i get_size() const;

		Channels get_channels() const;

		bool valid() const;
		const byte *get_buffer() const;
		size_t get_buffer_size() const;

	private:
		Vector2i m_sz;
		int m_ch;

		byte *m_buf;
	};
}
