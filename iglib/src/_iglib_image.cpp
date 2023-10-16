#include "pch.h"
#include "_iglib_image.h"
#include "draw_internal.h"

FORCEINLINE byte *realloc_image_data(const byte *buf, const size_t len)
{
	if (buf == nullptr)
		return nullptr;
	void *mal = malloc(len);
	if (!mal)
		return nullptr;
	return (byte *)memcpy(mal, buf, len);
}

FORCEINLINE byte *load_image(const std::string &filename, Vector2i &sz, Channels &ch)
{
	byte *buf = SOIL_load_image(filename.c_str(), &sz.x, &sz.y, (int *)&ch, SOIL_LOAD_AUTO);
	if (buf == nullptr || sz.area() == 0)
	{
		if (SOIL_last_result()[ 0 ] != 0)
			bite::warn(SOIL_last_result());
		else
			bite::warn("Unknown SOIL error while loading: " + filename);
	}

	return buf;
}

namespace ig
{
	Image::Image()
		: m_sz{}, m_ch{}, m_buf{}
	{
	}

	Image::Image(const byte *data, const Vector2i size, Channels channels)
		: m_sz{ size }, m_ch{ channels }, m_buf{ realloc_image_data(data, size.area() * (int)channels) }
	{
	}

	Image::Image(const std::string &filename)
		: m_sz{}, m_ch{}, m_buf{ load_image(filename, m_sz, m_ch) }
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

	int Image::get_width() const noexcept
	{
		return m_sz.x;
	}

	int Image::get_height() const noexcept
	{
		return m_sz.y;
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
		return size_t(m_sz.area()) * (int)m_ch;
	}

	void Image::flip_v()
	{
		::flip_v(m_buf, get_width(), get_height(), get_channels());
	}

	void Image::flip_h()
	{
		::flip_h(m_buf, get_width(), get_height(), get_channels());
	}

	void Image::rotate_clockwise()
	{
	}

	void Image::rotate_counter_clockwise()
	{
	}

	// tga 2.0
	void Image::save_tga(const std::string &path) const
	{
		// 8 for every channel
		constexpr byte BitPerPixel = 8 * 4;
		enum TgaImageType
		{
			None,
			RawColorMapped,
			RawTrueColor,
			RawBW,
			RLEColorMapped,
			RLETrueColor,
			RLEBW,

		};



		SOIL_save_image(path.c_str(), SOIL_SAVE_TYPE_TGA, get_width(), get_height(), get_channels(), get_buffer());

		return ;

		StreamWriter wr{ path, EndianOrder::Little };
		
		wr.write<byte>(0); // id field length
		wr.write<byte>(0); // color map
		wr.write<byte>(0); // type
		
		wr.write<uint16_t>(0); // first colormap entry index
		wr.write<uint16_t>(0); // number of colormap entries
		wr.write<byte>(BitPerPixel); // bits per colormap entry

		wr.write<Vector2s>({0, 0}); // origin
		wr.write<Vector2s>(get_size()); // size
		wr.write<byte>(BitPerPixel); // pixel depth
		wr.write<byte>(0); // descriptor

		//wr.write<somethin>() // img id field
		//wr.write<somethin>() // colormap field

	}

#if 0
	void Image::save_png(const std::string &path) const
	{


		static constexpr uint64_t PngSign = 9894494448401390090U;
		static constexpr EndianOrder PngOrder = EndianOrder::Big;
		bite::StreamWriter writer{ path, PngOrder };
		bite::StreamReader reader{ path, PngOrder };
		writer.write(PngSign);
		writer.write<int32_t>(13);
		writer.write('IHDR');
		writer.write(get_width());
		writer.write(get_height());
		writer.write<int8_t>(8); // bit depth

		std::cout << "channel: " << m_ch << '\n';
		// color type
		switch (m_ch)
		{
		case ig::L:
			writer.write<uint8_t>(0);
			break;
		case ig::LA:
			writer.write<uint8_t>(4);
			break;
		case ig::RGB:
			writer.write<uint8_t>(2);
			break;
		case ig::RGBA:
			writer.write<uint8_t>(6);
			break;
		default:
			break;
		}

		writer.write<uint8_t>(0); // compression (always 0)
		writer.write<uint8_t>(0); // filter (always 0)
		writer.write<uint8_t>(0); // interlaced


		writer.flush();
		
		// FIXME
		// FIXME
		writer.write<int32_t>(0); // crc

		writer.write<uint32_t>((uint32_t)get_buffer_size() + get_height());
		writer.write('IDAT');

		for (size_t x{}; x < get_width(); x++)
		{
			writer.write<uint8_t>(0);
			writer.write(m_buf + (x * get_height() * m_ch), get_height() * (size_t)m_ch);
		}

		writer.write<int32_t>(0); // crc

		writer.write<int32_t>(0);
		writer.write('IEND');
		writer.write<int32_t>(2187346606); // crc

	}
#endif
}
