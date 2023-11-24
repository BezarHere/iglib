#include "pch.h"
#include "_iglib_image.h"
#include "draw_internal.h"
#undef RGB

FORCEINLINE byte *allocate_image_data(const size_t len)
{
	void *mal = malloc(len);
	REPORT_V(mal == NULL, NULL);
	return (byte *)mal;
}

FORCEINLINE byte *realloc_image_data(const byte *buf, const size_t len)
{
	if (buf == nullptr)
		return nullptr;
	void *mal = malloc(len);
	if (!mal)
		return nullptr;
	return (byte *)memcpy(mal, buf, len);
}

FORCEINLINE byte *load_image(const std::string &filename, Vector2i &sz, ColorFormat &ch)
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

typedef void(*Formater_t)(const byte *from, byte *to);

FORCEINLINE byte constexpr grayscale(const byte data[ 3 ])
{
	return (data[ 0 ] + data[ 1 ] + data[ 2 ]) / 3;
}

template <ColorFormat _FROM, ColorFormat _TO>
FORCEINLINE static void format_color(const byte *from, byte *to)
{
	static_assert(_FROM != _TO, "_FROM & _TO shouldn't be equal");
	constexpr byte MaxValue = 255;

	if constexpr (_FROM == ColorFormat::L)
	{
		if constexpr (_TO == ColorFormat::LA)
		{
			to[ 0 ] = from[0];
			to[ 1 ] = MaxValue;
		}
		else if constexpr (_TO == ColorFormat::RGB)
		{
			to[ 0 ] = to[ 1 ] = to[ 2 ] = from[ 0 ];
		}
		else if constexpr (_TO == ColorFormat::RGBA)
		{
			to[ 0 ] = to[ 1 ] = to[ 2 ] = from[ 0 ];
			to[ 3 ] = MaxValue;
		}
	}
	else if constexpr (_FROM == ColorFormat::LA)
	{
		if constexpr (_TO == ColorFormat::L)
		{
			to[ 0 ] = (from[ 0 ] * from[ 1 ]) / 255;
		}
		else if constexpr (_TO == ColorFormat::RGB)
		{
			to[ 0 ] = to[ 1 ] = to[ 2 ] = (from[ 0 ] * from[ 1 ]) / MaxValue;
		}
		else if constexpr (_TO == ColorFormat::RGBA)
		{
			to[ 0 ] = to[ 1 ] = to[ 2 ] = from[ 0 ];
			to[ 3 ] = from[ 1 ];
		}
	}
	else if constexpr (_FROM == ColorFormat::RGB)
	{
		if constexpr (_TO == ColorFormat::L)
		{
			to[ 0 ] = (from[ 0 ] + from[ 1 ] + from[ 2 ]) / 3;
		}
		else if constexpr (_TO == ColorFormat::LA)
		{
			to[ 0 ] = grayscale(from);
			to[ 1 ] = MaxValue;
		}
		else if constexpr (_TO == ColorFormat::RGBA)
		{
			to[ 0 ] = from[ 0 ];
			to[ 1 ] = from[ 1 ];
			to[ 2 ] = from[ 2 ];
			to[ 3 ] = MaxValue;
		}
	}
	else if constexpr (_FROM == ColorFormat::RGBA)
	{
		if constexpr (_TO == ColorFormat::L)
		{
			to[ 0 ] = (grayscale(from) * from[ 3 ]) / MaxValue;
		}
		else if constexpr (_TO == ColorFormat::LA)
		{
			to[ 0 ] = grayscale(from);
			to[ 1 ] = from[ 3 ];
		}
		else if constexpr (_TO == ColorFormat::RGB)
		{
			to[ 0 ] = (from[ 0 ] * from[ 3 ]) / MaxValue;
			to[ 1 ] = (from[ 1 ] * from[ 3 ]) / MaxValue;
			to[ 2 ] = (from[ 2 ] * from[ 3 ]) / MaxValue;
		}
	}
}


template <ColorFormat _FROM, ColorFormat _TO>
FORCEINLINE static byte *convert_formater(const byte *data, const Vector2i sz)
{
	constexpr ColorFormat from = _FROM, to = _TO;
	constexpr Formater_t formater = format_color<from, to>;

	//const size_t FromSize = size_t(sz.area()) * get_colorformat_size(from);
	const size_t ToSize = size_t(sz.area()) * get_colorformat_size(to);

	byte *to_data = allocate_image_data(ToSize);

	for (int y = 0; y < sz.y; y++)
	{
		for (int x = 0; x < sz.x; x++)
		{
			const int index_raw = (y * sz.x) + x;
			formater(data + (index_raw * get_colorformat_size(from)), to_data + (index_raw * get_colorformat_size(to)));
		}
	}
	return to_data;
}

namespace ig
{
	Image::Image()
		: m_sz{}, m_format{}, m_buf{}
	{
	}

	Image::Image(const Vector2i size, ColorFormat format)
		: m_sz{ size }, m_format{ format }, m_buf{allocate_image_data(get_buffer_size())}
	{
	}

	Image::Image(const byte *data, const Vector2i size, ColorFormat format)
		: m_sz{ size }, m_format{ format }, m_buf{ realloc_image_data(data, get_buffer_size()) }
	{
	}

	Image::Image(const std::string &filename)
		: m_sz{}, m_format{}, m_buf{ load_image(filename, m_sz, m_format) }
	{
	}

	Image::~Image() noexcept
	{
		if (valid())
			SOIL_free_image_data(m_buf);
	}

	Image::Image(const Image &copy) noexcept
		: m_sz{ copy.m_sz }, m_format{ copy.m_format },
		m_buf{ realloc_image_data(copy.m_buf, copy.get_buffer_size()) }
	{
	}

	Image::Image(Image &&move) noexcept
		: m_sz{ move.m_sz }, m_format{ move.m_format }, m_buf{ move.m_buf }
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
		m_format = move.m_format;

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
		m_format = copy.m_format;

		m_buf = realloc_image_data(copy.m_buf, copy.get_buffer_size());
		return *this;
	}

	int Image::width() const noexcept
	{
		return m_sz.x;
	}

	int Image::height() const noexcept
	{
		return m_sz.y;
	}

	Vector2i Image::size() const
	{
		return m_sz;
	}

	ColorFormat Image::format() const
	{
		return ColorFormat(m_format);
	}

	bool Image::valid() const
	{
		return m_buf != nullptr && get_buffer_size();
	}

	byte *Image::get_buffer()
	{
		return m_buf;
	}

	const byte *Image::get_buffer() const
	{
		return m_buf;
	}

	size_t Image::get_buffer_size() const
	{
		return size_t(m_sz.area()) * (int)m_format;
	}

	void Image::flip_v()
	{
		::flip_v(m_buf, width(), height(), format());
	}

	void Image::flip_h()
	{
		::flip_h(m_buf, width(), height(), format());
	}

	void Image::rotate_clockwise()
	{
	}

	void Image::rotate_counter_clockwise()
	{
	}

	void Image::transpose()
	{
		
		byte *newbuf = (byte *)malloc( get_buffer_size() );
		if (newbuf == nullptr)
			throw std::bad_alloc();

		for (int i = 0; i < m_sz.y; i++)
		{
			for (int j = 0; j < m_sz.x; j++)
			{
				const int index1 = ((i * m_sz.x) + j) * int(m_format);
				const int index2 = ((j * m_sz.x) + i) * int(m_format);

				for (int f = 0; f < int(m_format); f++)
					newbuf[ index2 + f ] = m_buf[ index1 + f ];
			}
		}
		SOIL_free_image_data(m_buf);
		m_buf = newbuf;
	}

	void Image::blit(const Image &src, const Rect2i &src_rect, const Vector2i dst_pos)
	{
		REPORT(src.format() != this->format());

		const Vector2i src_range{
			src_rect.w + src_rect.x > src.width() ? src.width() - src_rect.x : src_rect.w,
			src_rect.h + src_rect.y > src.height() ? src.height() - src_rect.y : src_rect.h
		};
		const Vector2i range{
			src_range.x + dst_pos.x > m_sz.x ? m_sz.x - dst_pos.x : src_range.x,
			src_range.y + dst_pos.y > m_sz.y ? m_sz.y - dst_pos.y : src_range.y,
		};

		const int fs = get_colorformat_size(m_format);
		for (int fb = 0; fb < fs; fb++)
		{
			for (int y = 0; y < range.y; y++)
			{
				for (int x = 0; x < range.x; x++)
				{
					m_buf[ ((y + dst_pos.y) * range.x + (x + dst_pos.x)) * fs + fb ] = src.m_buf[ ((y + src_rect.y) * range.x + (x + src_rect.x)) * fs + fb ];
				}
			}
		}
	}

	void Image::blit(const Image &src, const Vector2i dst_pos)
	{
		blit(src, { 0, 0, src.width(), src.height() }, dst_pos);
	}

	// tga 2.0
	void Image::save_tga(const std::string &path) const
	{
		// 8 for every channel
		//constexpr byte BitPerPixel = 8 * 4;
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



		SOIL_save_image(path.c_str(), SOIL_SAVE_TYPE_TGA, width(), height(), format(), get_buffer());
	}

	Image Image::subimage(Rect2i rect) const
	{
		rect = rect.intersection({ 0, 0, m_sz.x, m_sz.y });

		return Image(subbuffer(rect).get(), rect.size(), m_format);
	}

	std::unique_ptr<byte[]> Image::subbuffer(Rect2i rect) const
	{
		rect = rect.intersection({ 0, 0, m_sz.x, m_sz.y });
		const int subbuf_sz = std::abs(rect.w * rect.h) * (int)m_format;
		byte *subbuf = new byte[ subbuf_sz ]{};

		for (int y = 0; y < rect.h; y++)
		{
			for (int x = 0; x < rect.w; x++)
			{
				const int local_index = ((y * rect.w) + x) * int(m_format);
				const int index = ((((rect.h - (y + 1)) + rect.y) * m_sz.x) + ((rect.w - (x + 1)) + rect.x)) * int(m_format);


				for (int f = 0; f < int(m_format); f++)
				{
					subbuf[ local_index + f ] = m_buf[ index + f ];
				}

			}
		}
		return std::unique_ptr<byte[]>( subbuf );
	}

	void Image::convert(ColorFormat to_format)
	{
		REPORT(format() == to_format);

		byte *new_buf = nullptr;

#define TO_SW(base, a, b, c) switch (to_format) \
{ case ColorFormat::a:{ new_buf = convert_formater<base, ColorFormat::a>(m_buf, size()); }break;case ColorFormat::b:{ new_buf = convert_formater<base, ColorFormat::b>(m_buf, size()); }break;case ColorFormat::c:{ new_buf = convert_formater<base, ColorFormat::c>(m_buf, size()); }break; default: break; }

		switch (m_format)
		{
		case ig::L:
			TO_SW(ColorFormat::L, LA, RGB, RGBA);
			break;
		case ig::LA:
			TO_SW(ColorFormat::LA, L, RGB, RGBA);
			break;
		case ig::RGB:
			TO_SW(ColorFormat::RGB, L, LA, RGBA);
			break;
		case ig::RGBA:
			TO_SW(ColorFormat::RGBA, L, LA, RGB);
			break;
		default:
			break;
		}
		REPORT(new_buf == nullptr);
		SOIL_free_image_data(new_buf);
		m_buf = new_buf;
		m_format = to_format;
	}
}
