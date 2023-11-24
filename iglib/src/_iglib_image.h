#pragma once
#include "_iglib_vector.h"
#include "_iglib_rect.h"
#include "_iglib_color.h"

namespace ig
{
	enum ColorFormat
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
		Image(const Vector2i size, ColorFormat format = ColorFormat::RGBA);
		Image(const byte *data, const Vector2i size, ColorFormat format);
		Image(const std::string &filename);
		~Image() noexcept;

		Image(Image &&move) noexcept;
		Image(const Image &copy) noexcept;
		Image &operator=(Image &&move) noexcept;
		Image &operator=(const Image &copy) noexcept;

		int width() const noexcept;
		int height() const noexcept;
		Vector2i size() const;

		ColorFormat format() const;

		bool valid() const;
		byte *get_buffer();
		const byte *get_buffer() const;
		size_t get_buffer_size() const;


		void flip_v();
		void flip_h();
		void rotate_clockwise();
		void rotate_counter_clockwise();
		void transpose();

		void blit(const Image &src, const Rect2i &src_rect, const Vector2i dst_pos);
		void blit(const Image &src, const Vector2i dst_pos);
		
		// tga 2.0
		void save_tga(const std::string &path) const;

		/// \note out of bounds rect will be clipped for this image
		Image subimage(Rect2i rect) const;

		/// \note out of bounds rect will be clipped for this image
		std::unique_ptr<byte[]> subbuffer(Rect2i rect) const;

	private:
		Vector2i m_sz;
		ColorFormat m_format;
		byte *m_buf;
	};
}
