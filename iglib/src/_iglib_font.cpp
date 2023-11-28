#include "pch.h"
#include "_iglib_font.h"
#include "_iglib_font.h"
#include "draw_internal.h"
#include "internal.h"

// RGBA 8x8 source 

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

FORCEINLINE Image pack_font_atlas(const Image &source, const Vector2i raw_count, const Vector2i glyph_size, const Vector2i spacing)
{
	// already packed
	if (!spacing.x && !spacing.y)
	{
		if (source.format() == ColorFormat::LA)
			return source;
		Image s{ source };
		s.convert(ColorFormat::LA);
		return s;
	}

	if (source.format() != ColorFormat::LA)
	{
		Image s{ source };
		s.convert(ColorFormat::LA);
#pragma warning(push)
#pragma warning(disable: 4714)
		return pack_font_atlas(s, raw_count, glyph_size, spacing);
#pragma warning(pop)
	}

	// source will be always formated as LA

	Image strip{ raw_count * glyph_size, ColorFormat::LA };
	for (int y = 0; y < raw_count.y; y++)
	{
		const int yglyph = (spacing.y + glyph_size.y) * y;
		for (int x = 0; x < raw_count.x; x++)
		{
			const Vector2i glyphs_cell{ (glyph_size.x + spacing.x) * x, yglyph };
			strip.blit(
								source,
								{
									(glyph_size.x + spacing.x) * x,
									yglyph,
									glyph_size.x,
									glyph_size.y
								},
								{
									x * glyph_size.x,
									y * glyph_size.y
								});
		}
	}
	return strip;
}


struct CodepoinIndex
{
	FORCEINLINE CodepoinIndex( codepoint_t cp, size_t i )
		: codepoint{ cp }, index{ i }
	{

	}

	codepoint_t codepoint;
	size_t index;
};


// TODO: complete
// sort from smallest to biggest codepoint value
inline void sort( CodepoinIndex *ptr, const uint32_t n )
{
	struct subarray_range
	{
		FORCEINLINE subarray_range( uint32_t f, uint32_t t )
			: from{ f }, to{ t }
		{

		}

		FORCEINLINE uint32_t length() const noexcept { return to - from; }

		uint32_t from;
		uint32_t to;
	};
	
	if (n < 2)
		return;
	
	std::stack<subarray_range, std::vector<subarray_range>> partition_stack{};
	
	partition_stack.emplace( 0, n );
	
	while (!partition_stack.empty())
	{
		const subarray_range subarr = partition_stack.top();
		partition_stack.pop();
		if (subarr.length() > 2)
		{
			uint32_t half = subarr.length() >> 1;
			partition_stack.emplace( subarr.from, subarr.from + half );
			partition_stack.emplace( subarr.from + half, subarr.to );
		}

		CodepoinIndex pivot = ptr[ subarr.from + (subarr.length() >> 1) ];


	}

}

int __cdecl codepoint_index_qsort_comp( void const *a, void const *b )
{
	return ((CodepoinIndex *)a)->codepoint > ((CodepoinIndex *)b)->codepoint;
}

namespace ig
{

	struct Font::FontInternal
	{
		FORCEINLINE FontInternal(const Image &source, const Vector2i glyph_size, const Vector2i spacing, const BitmapFontDef def = {} )
			: faces_atlas{}
		{
			if (glyph_size.x >= 256)
				bite::raise("Overflow: glyph_size.x was " + std::to_string(glyph_size.x) + " wich is greater/equal to 256");

			if (glyph_size.y >= 512)
				bite::raise("Overflow: glyph_size.y was " + std::to_string(glyph_size.y) + " wich is greater/equal to 512");

			const Vector2i raw_count = std::invoke([gsize = source.size(), glyph_size, spacing]()
				{
					const Vector2i spaces = (gsize * spacing / glyph_size) - spacing;
					return (gsize - spaces) / glyph_size;
				});

			const int raw_count_area = raw_count.area();
			REPORT(raw_count_area == 0);
			REPORT(raw_count.x * glyph_size.x >= MaxTextureWidth);
			REPORT(raw_count.y * glyph_size.y >= MaxTextureHeight);
			
			int current_codepoint_index = 0;
			int codepoint_value = def.start_codepoint;

#define GEN_CODEPOINT \
if (def.codepoints && def.codepoints_count > current_codepoint_index) \
     codepoint_value = def.codepoints[ current_codepoint_index++ ] + 1; \
else codepoint_value++;; \
{ \
glyphs.push_back({ codepoint_value - 1, Vector2i(), glyph_size, { x, y } }); \
m_codepoint_indexes.emplace_back( codepoint_value - 1, glyphs.size() - 1 ); \
}
			
			if (def.transposed)
			{
				for (int y = 0u; y < raw_count.y; y++)
				{
					for (int x = 0u; x < raw_count.x; x++)
					{
						GEN_CODEPOINT;
					}
				}
			}
			else
			{
				for (int x = 0u; x < raw_count.x; x++)
				{
					for (int y = 0u; y < raw_count.y; y++)
					{
						GEN_CODEPOINT;
					}
				}
			}
			m_cp_binary_search_limit = (m_codepoint_indexes.size() >> 1) + 1;
			std::qsort( m_codepoint_indexes.data(), m_codepoint_indexes.size(), sizeof( m_codepoint_indexes[ 0 ] ), codepoint_index_qsort_comp );

			gsize = glyph_size;
			faces_atlas = Texture( pack_font_atlas( source, raw_count, glyph_size, spacing ) );
			uvstep = Vector2f(gsize) / Vector2f( faces_atlas.size() );
		}

		// binary search [BUGBUG IF m_codepoint_indexes.size() < 4]
		size_t get_glyph_index( const codepoint_t cp ) const noexcept
		{
			const size_t sz = m_codepoint_indexes.size();
			WARN( sz < 4 );
			size_t i = sz / 2;
			size_t step = sz / 2;
			for (int limit = 2; limit < sz; limit++)
			{
				if (step > 1)
					step >>= 1;

				if (m_codepoint_indexes[ i ].codepoint > cp)
				{
					if (i < step)
						return NPos;
					i -= step;
				}
				else if (m_codepoint_indexes[ i ].codepoint < cp)
				{
					i += step;
					if (i >= sz)
						return NPos;
				}
				else
				{
					return m_codepoint_indexes[ i ].index;
				}
			}
			return m_codepoint_indexes[ i ].codepoint == cp ? m_codepoint_indexes[ i ].index : NPos;
		}

		Vector2i gsize;
		Vector2f uvstep;
		Texture faces_atlas;
		std::vector<Glyph> glyphs;
	private:
		std::vector<CodepoinIndex> m_codepoint_indexes; // <- should be sorted based on codepoints
		int m_cp_binary_search_limit = 4;
	};

	static std::shared_ptr<Font::FontInternal> get_default_font()
	{
		static std::shared_ptr<Font::FontInternal> DefaultBitmapFont;
		REPORT_V( !is_glew_running(), { nullptr } );
		if (!DefaultBitmapFont)
		{
			static Image DefaultBitmapGlyphImage = Image( (const byte *)DefaultGlyphs, DefaultGlyphsSize, ColorFormat::LA );
			//DefaultBitmapGlyphImage.transpose_bytes();
			DefaultBitmapFont.reset( new Font::FontInternal( DefaultBitmapGlyphImage, { 8, 8 }, { 0, 0 } ) );
		}
		return DefaultBitmapFont;
	}

	Font::Font(const std::string &filepath)
	{
	}

	Font::Font( const Image &glyphs, Vector2i glyph_size, Vector2i spacing, BitmapFontDef def )
		: m_type{ FontType::Bitmap }, m_internal{
		new FontInternal(glyphs, glyph_size, spacing, def )
	}
	{
	}

	ig::Font::Font()
		: m_type{ FontType::Bitmap }, m_internal{ get_default_font() }
	{
	}

	TextureId_t Font::get_atlas() const
	{
		return m_internal->faces_atlas.get_handle();
	}

	Font::Glyph *Font::get_glyphs()
	{
		return m_internal->glyphs.data();
	}

	const Font::Glyph *Font::get_glyphs() const
	{
		return m_internal->glyphs.data();
	}

	const int Font::get_glyphs_count() const
	{
		return m_internal->glyphs.size();
	}

	const size_t Font::get_glyph_index( const codepoint_t codepoint ) const
	{
		return m_internal->get_glyph_index(codepoint);
	}

	const Vector2f Font::get_texcoord_step() const
	{
		return m_internal->uvstep;
	}

	void Font::set_char_spacing( int value )
	{
		m_char_spacing = value;
	}

	void Font::set_line_spacing( int value )
	{
		m_line_spacing = value;
	}

	int Font::get_line_spacing() const
	{
		return m_line_spacing;
	}

	int Font::get_char_spacing() const
	{
		return m_char_spacing;
	}

}

