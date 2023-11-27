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

 Image pack_font_atlas(const Image &source, const Vector2i raw_count, const Vector2i glyph_size, const Vector2i spacing)
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


namespace ig
{

	
	struct Font::FontInternal
	{
		FontInternal(const Image &source, const Vector2i glyph_size, const Vector2i spacing, const BitmapFontDef def = {} )
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

#define ASSIGEN_CHAR \
if (def.codepoints && def.codepoints_count > current_codepoint_index) \
     codepoint_value = def.codepoints[ current_codepoint_index++ ]; \
else codepoint_value++; \
charecters.push_back({ codepoint_value, Vector2i(), glyph_size, { x, y } })
			
			if (def.transposed)
			{
				for (int y = 0u; y < raw_count.y; y++)
				{
					for (int x = 0u; x < raw_count.x; x++)
					{
						ASSIGEN_CHAR;
					}
				}
			}
			else
			{
				for (int x = 0u; x < raw_count.x; x++)
				{
					for (int y = 0u; y < raw_count.y; y++)
					{
						ASSIGEN_CHAR;
					}
				}
			}

			gsize = glyph_size;
			faces_atlas = Texture( pack_font_atlas( source, raw_count, glyph_size, spacing ) );
			uvstep = Vector2f(gsize) / Vector2f( faces_atlas.size() );
		}

		Vector2i gsize;
		Vector2f uvstep;
		Texture faces_atlas;
		std::vector<Charecter> charecters;
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

	Font::Charecter *Font::get_chars()
	{
		return m_internal->charecters.data();
	}

	const Font::Charecter *Font::get_chars() const
	{
		return m_internal->charecters.data();
	}

	const int Font::get_chars_count() const
	{
		return m_internal->charecters.size();
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

