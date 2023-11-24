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

namespace ig
{

	static std::shared_ptr<Font::FontInternal> DefaultBitmapFont;
	struct Font::FontInternal
	{
		FORCEINLINE FontInternal(const Image &source, const Vector2i glyph_size, const Vector2i spacing)
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
			Image strip = pack_font_atlas(source, raw_count, glyph_size, spacing);
			Image grgba = strip;
			grgba.convert(ColorFormat::RGBA);
			grgba.save_tga("F:\\Assets\\visual studio\\IGLib\\IGLibDemo\\src\\atlas.tga");
			faces_atlas = Texture(strip);
		}

		Texture faces_atlas;
		std::vector<Charecter> charecters;
	};

	void _font_init()
	{
		static Image DefaultBitmapGlyphImage = Image((const byte *)DefaultGlyphs, DefaultGlyphsSize, ColorFormat::LA);
		//DefaultBitmapGlyphImage.transpose();
		DefaultBitmapFont.reset(new Font::FontInternal(DefaultBitmapGlyphImage, { 8, 8 }, { 0, 0 }));
	}

	Font::Font(const std::string &filepath)
	{
	}

	Font::Font(const Image &glyphs, Vector2i glyph_size, Vector2i spacing)
		: m_type{ FontType::Bitmap }, m_internal{
		new FontInternal(glyphs, glyph_size, spacing)
	}
	{
	}

	ig::Font::Font()
		: m_type{ FontType::Bitmap }, m_internal{ DefaultBitmapFont }
	{
	}

}

