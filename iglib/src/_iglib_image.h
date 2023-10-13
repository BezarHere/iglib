#pragma once
#include "_iglib_vector.h"
#include "_iglib_color.h"

namespace ig
{
	enum Channels
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
		// does not take ownership of data
		Image(const byte *data, const Vector2i size, Channels channels);
		Image(const std::string &filename);
		~Image() noexcept;

		Image(Image &&move) noexcept;
		Image(const Image &copy) noexcept;
		Image &operator=(Image &&move) noexcept;
		Image &operator=(const Image &copy) noexcept;

		int get_width() const noexcept;
		int get_height() const noexcept;
		Vector2i get_size() const;

		Channels get_channels() const;

		bool valid() const;
		const byte *get_buffer() const;
		size_t get_buffer_size() const;

		//void save_png(const std::string &path) const;


		// tga 2.0
		void save_tga(const std::string &path) const;

	private:
		Vector2i m_sz;
		Channels m_ch;

		byte *m_buf;
	};
}
