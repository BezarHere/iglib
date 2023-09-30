#include "pch.h"
#include "_iglib_image.h"

__forceinline byte *realloc_image_data(const byte *buf, const size_t len)
{
	if (buf == nullptr)
		return nullptr;
	void *mal = malloc(len);
	if (!mal)
		return nullptr;
	return (byte *)memcpy(mal, buf, len);
}

namespace ig
{
	Image::Image()
		: m_sz{}, m_ch{}, m_buf{}
	{
	}

	Image::Image(const std::string &filename)
		: m_sz{}, m_ch{}, m_buf{ SOIL_load_image(filename.c_str(), &m_sz.x, &m_sz.y, &m_ch, SOIL_LOAD_AUTO) }
	{
	}

	Image::~Image() noexcept
	{
		if (valid())
			SOIL_free_image_data(m_buf);
	}

	Image::Image(const Image &copy) noexcept
		: m_sz{ copy.m_sz }, m_ch{ copy.m_ch },
		m_buf{ realloc_image_data(copy.m_buf, copy.get_buffer_size()) }
	{
	}

	Image::Image(Image &&move) noexcept
		: m_sz{ move.m_sz }, m_ch{ move.m_ch }, m_buf{ move.m_buf }
	{
		// taking control from 'move'
		move.m_buf = nullptr;
	}

	Image &Image::operator=(Image &&move) noexcept
	{
		// remove the current image buffer
		if (valid())
			SOIL_free_image_data(m_buf);

		m_sz = move.m_sz;
		m_ch = move.m_ch;

		// taking control from 'move'
		m_buf = move.m_buf;
		move.m_buf = nullptr;
		return *this;
	}

	Image &Image::operator=(const Image &copy) noexcept
	{
		// remove the current image buffer
		if (valid())
			SOIL_free_image_data(m_buf);

		m_sz = copy.m_sz;
		m_ch = copy.m_ch;

		m_buf = realloc_image_data(copy.m_buf, copy.get_buffer_size());
		return *this;
	}

	Vector2i Image::get_size() const
	{
		return m_sz;
	}

	Channels Image::get_channels() const
	{
		return Channels(m_ch);
	}

	bool Image::valid() const
	{
		return m_buf != nullptr && get_buffer_size();
	}

	const byte *Image::get_buffer() const
	{
		return m_buf;
	}

	size_t Image::get_buffer_size() const
	{
		return size_t(m_sz.area()) * m_ch;
	}

}
