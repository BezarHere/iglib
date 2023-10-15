#pragma once
#include "_iglib_base.h"
#include "_iglib_vector.h"
#include <memory>
#include <unordered_map>

namespace ig
{

	struct BitmapChar
	{
		BitmapChar() noexcept;
		BitmapChar(uint32_t pcode);
		BitmapChar(uint32_t pcode, std::shared_ptr<byte[]> pbuf);
		BitmapChar(uint32_t pcode, BufferSpan pbuf);

		uint32_t code;
		std::shared_ptr<byte[]> buf;
	};

	using BitmapCharHeapSpan = basic_heap_span<BitmapChar>;
	using BitmapCharHeapView = basic_heap_view<BitmapChar>;

	class BitmapFont
	{
	public:
		

		BitmapFont(Vector2s size, const BitmapCharHeapSpan chars);
		BitmapFont(Vector2s size, const BitmapCharHeapView chars);

		const BitmapChar &get_char(int code) const;

	private:
		const Vector2s m_sz;
		const BitmapCharHeapSpan m_chars;
	};

	class BitmapBuilder
	{
	public:
		BitmapBuilder(Vector2s sz);

		BitmapBuilder &add_char(BitmapChar c);
		BitmapFont generate() const;

	private:
		const Vector2s m_sz;
		std::unordered_map<uint32_t, BitmapChar> m_chars_table;
	};

}

