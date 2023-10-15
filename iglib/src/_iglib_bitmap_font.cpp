#include "pch.h"
#include "_iglib_bitmap_font.h"

constexpr byte UTF8LenTable[ 256 ]
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 8
};

//const BitmapFont retro =
//{
//	BitmapBuilder({ 5, 7 })
//	.add_char({ 'a', {  } })
//	.add_char({ 'A', {  } })
//	.generate()
//};

FORCEINLINE BitmapCharHeapSpan to_span(BitmapCharHeapView view)
{
	return BitmapCharHeapSpan{ (BitmapChar *)memcpy(new BitmapChar[ view.sz ]{}, view.ptr, view.sz), view.sz };
}

// TODO: optimize
FORCEINLINE BitmapCharHeapSpan charsv_to_bitmap(const std::vector<BitmapChar> &chars)
{
	size_t max_code = 0;
	for (const auto &v : chars)
	{
		if (v.code > max_code)
			max_code = v.code;
	}

	BitmapCharHeapSpan span{ max_code + 1 };

	for (size_t i = 0; i < max_code; i++)
	{
		span.ptr[ i ] = chars[ i ];
	}
	return span;
}

// TODO: optimize
FORCEINLINE BitmapCharHeapSpan chartable_to_bitmap(const std::unordered_map<uint32_t, BitmapChar> &table)
{
	uint32_t max_code = 0;
	for (const auto &v : table)
	{
		if (v.first > max_code)
			max_code = v.first;
	}

	BitmapCharHeapSpan span{ max_code + 1 };
	basic_heap_span<bool> has_char{ max_code + 1 };

	for (const auto &v : table)
	{
		has_char.ptr[v.first] = true;
	}

	for (uint32_t i = 0; i < max_code; i++)
	{
		if (has_char.ptr[ i ])
			span.ptr[ i ] = table.at(i);
		else
			span.ptr[ i ] = BitmapChar();
	}
	return span;
}

constexpr FORCEINLINE int hash_to_char_index(int code)
{
	return code;
}

namespace ig
{
	BitmapFont::BitmapFont(Vector2s size, const BitmapCharHeapSpan chars)
		: m_sz{ size }, m_chars{ chars }
	{
	}

	BitmapFont::BitmapFont(Vector2s size, const BitmapCharHeapView chars)
		: m_sz{ size }, m_chars(to_span(chars))
	{
	}

	const BitmapChar &BitmapFont::get_char(int code) const
	{
		return m_chars.ptr[code];
	}

	BitmapBuilder::BitmapBuilder(Vector2s sz)
		: m_sz{ sz }
	{
	}

	BitmapBuilder &BitmapBuilder::add_char(BitmapChar c)
	{
		m_chars_table.insert_or_assign(c.code, c);
		return *this;
	}

	BitmapFont BitmapBuilder::generate() const
	{
		return BitmapFont(m_sz, chartable_to_bitmap(m_chars_table));
	}

	BitmapChar::BitmapChar(uint32_t pcode, std::shared_ptr<byte[]> pbuf)
		: code{ pcode }, buf{ pbuf }
	{
	}

	// i know, this is gonna bug
	BitmapChar::BitmapChar(uint32_t pcode, BufferSpan pbuf)
		: code{ pcode }, buf{ pbuf.ptr }
	{
	}


	BitmapChar::BitmapChar() noexcept
		: code{ 0 }, buf{ nullptr }
	{
	}

	BitmapChar::BitmapChar(uint32_t pcode)
		: code{ pcode }, buf{ nullptr }
	{
	}

}
