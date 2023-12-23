#include "pch.h"
#include "_iglib_font.h"
#include "_iglib_font.h"
#include "draw_internal.h"
#include "internal.h"

inline std::string FT_Error_Get_String( FT_Error error )
{
	const std::string FTErrorMessages[]
	{
		"No error",
		"Cannot open resource",
		"Unknown file format",
		"Broken file",
		"Invalid freetype version",
		"Module version is too low",
		"Invalid argument",
		"Unimplemented feature",
		"Broken table",
		"Broken offset within table",
		"Array allocation size too large",
		"Missing module",
		"Missing property",
		"UNNAMED_ERROR_ID_0XD",
		"UNNAMED_ERROR_ID_0XE",
		"UNNAMED_ERROR_ID_0XF",
		"Invalid glyph index",
		"Invalid character code",
		"Unsupported glyph image format",
		"Cannot render this glyph format",
		"Invalid outline",
		"Invalid composite glyph",
		"Too many hints",
		"Invalid pixel size",
		"Invalid svg document",
		"UNNAMED_ERROR_ID_0X19",
		"UNNAMED_ERROR_ID_0X1A",
		"UNNAMED_ERROR_ID_0X1B",
		"UNNAMED_ERROR_ID_0X1C",
		"UNNAMED_ERROR_ID_0X1D",
		"UNNAMED_ERROR_ID_0X1E",
		"UNNAMED_ERROR_ID_0X1F",
		"Invalid object handle",
		"Invalid library handle",
		"Invalid module handle",
		"Invalid face handle",
		"Invalid size handle",
		"Invalid glyph slot handle",
		"Invalid charmap handle",
		"Invalid cache manager handle",
		"Invalid stream handle",
		"UNNAMED_ERROR_ID_0X29",
		"UNNAMED_ERROR_ID_0X2A",
		"UNNAMED_ERROR_ID_0X2B",
		"UNNAMED_ERROR_ID_0X2C",
		"UNNAMED_ERROR_ID_0X2D",
		"UNNAMED_ERROR_ID_0X2E",
		"UNNAMED_ERROR_ID_0X2F",
		"Too many modules",
		"Too many extensions",
		"UNNAMED_ERROR_ID_0X32",
		"UNNAMED_ERROR_ID_0X33",
		"UNNAMED_ERROR_ID_0X34",
		"UNNAMED_ERROR_ID_0X35",
		"UNNAMED_ERROR_ID_0X36",
		"UNNAMED_ERROR_ID_0X37",
		"UNNAMED_ERROR_ID_0X38",
		"UNNAMED_ERROR_ID_0X39",
		"UNNAMED_ERROR_ID_0X3A",
		"UNNAMED_ERROR_ID_0X3B",
		"UNNAMED_ERROR_ID_0X3C",
		"UNNAMED_ERROR_ID_0X3D",
		"UNNAMED_ERROR_ID_0X3E",
		"UNNAMED_ERROR_ID_0X3F",
		"Out of memory",
		"Unlisted object",
		"UNNAMED_ERROR_ID_0X42",
		"UNNAMED_ERROR_ID_0X43",
		"UNNAMED_ERROR_ID_0X44",
		"UNNAMED_ERROR_ID_0X45",
		"UNNAMED_ERROR_ID_0X46",
		"UNNAMED_ERROR_ID_0X47",
		"UNNAMED_ERROR_ID_0X48",
		"UNNAMED_ERROR_ID_0X49",
		"UNNAMED_ERROR_ID_0X4A",
		"UNNAMED_ERROR_ID_0X4B",
		"UNNAMED_ERROR_ID_0X4C",
		"UNNAMED_ERROR_ID_0X4D",
		"UNNAMED_ERROR_ID_0X4E",
		"UNNAMED_ERROR_ID_0X4F",
		"UNNAMED_ERROR_ID_0X50",
		"Cannot open stream",
		"Invalid stream seek",
		"Invalid stream skip",
		"Invalid stream read",
		"Invalid stream operation",
		"Invalid frame operation",
		"Nested frame access",
		"Invalid frame read",
		"UNNAMED_ERROR_ID_0X59",
		"UNNAMED_ERROR_ID_0X5A",
		"UNNAMED_ERROR_ID_0X5B",
		"UNNAMED_ERROR_ID_0X5C",
		"UNNAMED_ERROR_ID_0X5D",
		"UNNAMED_ERROR_ID_0X5E",
		"UNNAMED_ERROR_ID_0X5F",
		"Raster uninitialized",
		"Raster corrupted",
		"Raster overflow",
		"Negative height while rastering",
		"UNNAMED_ERROR_ID_0X64",
		"UNNAMED_ERROR_ID_0X65",
		"UNNAMED_ERROR_ID_0X66",
		"UNNAMED_ERROR_ID_0X67",
		"UNNAMED_ERROR_ID_0X68",
		"UNNAMED_ERROR_ID_0X69",
		"UNNAMED_ERROR_ID_0X6A",
		"UNNAMED_ERROR_ID_0X6B",
		"UNNAMED_ERROR_ID_0X6C",
		"UNNAMED_ERROR_ID_0X6D",
		"UNNAMED_ERROR_ID_0X6E",
		"UNNAMED_ERROR_ID_0X6F",
		"Too many registered caches",
		"UNNAMED_ERROR_ID_0X71",
		"UNNAMED_ERROR_ID_0X72",
		"UNNAMED_ERROR_ID_0X73",
		"UNNAMED_ERROR_ID_0X74",
		"UNNAMED_ERROR_ID_0X75",
		"UNNAMED_ERROR_ID_0X76",
		"UNNAMED_ERROR_ID_0X77",
		"UNNAMED_ERROR_ID_0X78",
		"UNNAMED_ERROR_ID_0X79",
		"UNNAMED_ERROR_ID_0X7A",
		"UNNAMED_ERROR_ID_0X7B",
		"UNNAMED_ERROR_ID_0X7C",
		"UNNAMED_ERROR_ID_0X7D",
		"UNNAMED_ERROR_ID_0X7E",
		"UNNAMED_ERROR_ID_0X7F",
		"Invalid opcode",
		"Too few arguments",
		"Stack overflow",
		"Code overflow",
		"Bad argument",
		"Division by zero",
		"Invalid reference",
		"Found debug opcode",
		"Found endf opcode in execution stream",
		"Nested defs",
		"Invalid code range",
		"Execution context too long",
		"Too many function definitions",
		"Too many instruction definitions",
		"Sfnt font table missing",
		"Horizontal header (hhea) table missing",
		"Locations (loca) table missing",
		"Name table missing",
		"Character map (cmap) table missing",
		"Horizontal metrics (hmtx) table missing",
		"Postscript (post) table missing",
		"Invalid horizontal metrics",
		"Invalid character map (cmap) format",
		"Invalid ppem value",
		"Invalid vertical metrics",
		"Could not find context",
		"Invalid postscript (post) table format",
		"Invalid postscript (post) table",
		"Found fdef or idef opcode in glyf bytecode",
		"Missing bitmap in strike",
		"Svg hooks have not been set",
		"UNNAMED_ERROR_ID_0X9F",
		"Opcode syntax error",
		"Argument stack underflow",
		"Ignore",
		"No unicode glyph name found",
		"Glyph too big for hinting",
		"UNNAMED_ERROR_ID_0XA5",
		"UNNAMED_ERROR_ID_0XA6",
		"UNNAMED_ERROR_ID_0XA7",
		"UNNAMED_ERROR_ID_0XA8",
		"UNNAMED_ERROR_ID_0XA9",
		"UNNAMED_ERROR_ID_0XAA",
		"UNNAMED_ERROR_ID_0XAB",
		"UNNAMED_ERROR_ID_0XAC",
		"UNNAMED_ERROR_ID_0XAD",
		"UNNAMED_ERROR_ID_0XAE",
		"UNNAMED_ERROR_ID_0XAF",
		"`startfont' field missing",
		"`font' field missing",
		"`size' field missing",
		"`fontboundingbox' field missing",
		"`chars' field missing",
		"`startchar' field missing",
		"`encoding' field missing",
		"`bbx' field missing",
		"`bbx' too big",
		"Font header corrupted or missing fields",
		"Font glyphs corrupted or missing fields"
	};

	static const size_t FTErrorMessagesSize = ARRAYSIZE( FTErrorMessages );
	if (error >= FTErrorMessagesSize)
		return "UNKOWN_ERROR_" + std::to_string( error );
	return FTErrorMessages[ error ];
}

#define FT_CHECKED_CALL(code) { const FT_Error _ft_err = (code); if (_ft_err) { bite::warn("FreeType Error: \"" #code "\" Failed and returned \"" + FT_Error_Get_String(_ft_err) + "\" in \"" __FILE__ "\" line " + std::to_string(__LINE__)); return; } }
#define FT_CHECKED_CALL_V(code, ret) { const FT_Error _ft_err = (code); if (_ft_err) { bite::warn("FreeType Error: \"" #code "\" Failed and returned \"" + FT_Error_Get_String(_ft_err) + "\" in \"" __FILE__ "\" line " + std::to_string(__LINE__) + ", returning " #ret); return ret; } }

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

static FT_Library freetype_lib;

struct CodepoinIndex
{
	FORCEINLINE CodepoinIndex( codepoint_t cp, size_t i )
		: codepoint{ cp }, index{ i }
	{

	}

	codepoint_t codepoint;
	size_t index;
};

// TODO: Test
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

FORCEINLINE static int __cdecl codepoint_index_qsort_comp( void const *a, void const *b )
{
	return ((CodepoinIndex *)a)->codepoint > ((CodepoinIndex *)b)->codepoint;
}

FORCEINLINE static void init_freetype()
{
	{
		static bool _called = false;
		if (_called) return;
		_called = true;
	}
	FT_CHECKED_CALL( FT_Init_FreeType( &freetype_lib ) );
}

FORCEINLINE static FT_Face load_freetype_face(const std::string &path, FT_UInt width, FT_Long face_index = 0)
{
	FT_Face face;
	FT_CHECKED_CALL_V( FT_New_Face( freetype_lib, path.c_str(), face_index, &face ), nullptr );
	FT_CHECKED_CALL_V( FT_Set_Pixel_Sizes( face, width, 0 ), nullptr );
	return face;
}

struct GlyphBitmap
{
	FT_ULong code;
	std::shared_ptr<uint8_t[]> buffer;
	size_t len;
	Vector2i size;
	Vector2i advance; // 1/64 pixel per unit
	Vector2i bearing;
};

FORCEINLINE Vector2i squared_pow2( Vector2i size )
{
	const auto p2 = closest_pow2( size.area() ) >> 1;

	// odd
	if (p2 & 1)
		return { 1 << (p2 + 1), 1 << p2 };

	// even
	return { 1 << p2, 1 << p2 };
}

FORCEINLINE static GlyphBitmap gen_glyph_bitmap_image( const FT_GlyphSlot glyph, const FT_ULong code )
{
	const size_t n = size_t(glyph->bitmap.width) * glyph->bitmap.rows;
	uint8_t *buffer = (uint8_t *)memcpy( new uint8_t[ glyph->bitmap.width * glyph->bitmap.rows ], glyph->bitmap.buffer, n );

	return {
		code,
		std::shared_ptr<uint8_t[]>( buffer ),
		n,
		Vector2i( int( glyph->bitmap.width ), int( glyph->bitmap.rows ) ),
		Vector2i( glyph->metrics.horiAdvance, glyph->metrics.vertAdvance ),
		Vector2i( glyph->bitmap_left, glyph->bitmap_top - int( glyph->bitmap.rows ) )
	};
}

namespace ig
{

	struct Font::FontInternal
	{
		// TODO: SDFs!
		// TODO: OPTIMIZE!!
		FORCEINLINE FontInternal( const std::string &source, FT_UInt width, ValidGlyphsPredicate_t glyphs_predicate )
		{
			struct AlwaysTrue
			{
				static bool func( codepoint_t ) { return true; }
			};

			init_freetype();
			FT_Face face = load_freetype_face( source, width );
			FT_UInt gindex;
			std::vector<GlyphBitmap> gbitmaps;
			Vector2i collective_size{};
			size_t colective_len = 0;

			gbitmaps.reserve( 256 );

			if (!glyphs_predicate)
			{
				glyphs_predicate = AlwaysTrue::func;
			}

			for ( FT_ULong gchar = FT_Get_First_Char( face, &gindex );
						(gchar = FT_Get_Next_Char( face, gchar, &gindex )) != 0;
						gindex )
			{
				if (!glyphs_predicate( codepoint_t( gchar ) ))
					continue;

				if (FT_Load_Char( face, gchar, FT_LOAD_RENDER ))
				{
					bite::warn( "FreeType Error: could not load char with codepoint " + std::to_string( gchar ) );
					bite::warn( "Failed to load font from \"" + source + "\", size = " + std::to_string( width ) );
					break;
				}

				FT_GlyphSlot glyph = face->glyph;
				if (glyph->bitmap.buffer == nullptr)
					continue;

				GlyphBitmap gbitmap = gen_glyph_bitmap_image( glyph, gchar );
				colective_len += gbitmap.len;
				collective_size.x += gbitmap.size.x;
				collective_size.y = std::max( collective_size.y, gbitmap.size.y );

				gbitmaps.push_back( std::move(gbitmap) );
			}

			/// !BUGBUG [maybe too small if glyphs have janky sizes]
			Vector2i atlas_size = squared_pow2( collective_size );

			if (atlas_size.x > atlas_size.y)
			{
				atlas_size.y <<= 1;
			}
			else
			{
				atlas_size.x <<= 1;
			}

			Image img{ atlas_size, ColorFormat::LA };
			img.clear( Colorb( 255, 0, 0 ) );
			const size_t bitmaps_count = gbitmaps.size();

			// TODO: replace tile packing with the more compact mozaic packing
			Vector2i src_orig{};
			int line_height = 0;

			for (size_t i = 0; i < bitmaps_count; i++)
			{

				const GlyphBitmap &gb = gbitmaps[ i ];
				if (atlas_size.x - src_orig.x < gb.size.x)
				{
					src_orig.x = 0;
					src_orig.y += line_height;
					line_height = 0;

				}

				// NO MORE ROOM IN IMAGE, EXPAND
				if (atlas_size.y - src_orig.y < gb.size.y)
				{
					const int atlas_size_new_height = atlas_size.y + gb.size.y * 2;
					const float atlas_size_new_heightf = float(atlas_size_new_height);

					// update old glyph's atlas coords
					for (auto &c : glyphs)
					{
						c.atlas_uvbox.origin.y = (c.atlas_uvbox.origin.y * atlas_size.y) / atlas_size_new_heightf;
						c.atlas_uvbox.left.y   = (c.atlas_uvbox.left.y   * atlas_size.y) / atlas_size_new_heightf;
						c.atlas_uvbox.bottom.y = (c.atlas_uvbox.bottom.y * atlas_size.y) / atlas_size_new_heightf;
					}

					atlas_size.y = atlas_size_new_height;
					Image big{ atlas_size, img.format() };
					big.blit( img, { 0, 0 } );
					img = big;
				}

				byte *buf = img.get_buffer();
				const uint8_t *gb_buffer = gb.buffer.get();

				for (int y = 0; y < gb.size.y; y++)
				{
					for (int x = 0; x < gb.size.x; x++)
					{
						const int buf_i = ((y + src_orig.y) * atlas_size.x + (x + src_orig.x)) * 2;
#ifdef _PARANOID
						ASSERT( buf_i + 1 < img.get_buffer_size() );
						ASSERT( (y *gb.width.x + x) < gb.len );
#endif // _PARANOID


						buf[ buf_i ] = 255; // pixel x, y lum
						buf[ buf_i + 1 ] = gb_buffer[ y * gb.size.x + x ]; // pixel x, y alpha
					}
				}

				const UVBox uvbox = {
					Vector2f( float( src_orig.x ), float( src_orig.y ) ) / Vector2f( atlas_size ),
					Vector2f( 0.f, float( gb.size.y ) / atlas_size.y ),
					Vector2f( float( gb.size.x ) / atlas_size.x, 0.f )
				};

				m_codepoint_indexes.emplace_back( (codepoint_t)gb.code, glyphs.size() );
				glyphs.push_back( { (codepoint_t)gb.code,
														Vector2f( float(gb.bearing.x), -gb.bearing.y + float( gb.size.y < int( width ) ? (int( width ) - gb.size.y) : 0.f ) ), // offset
														gb.size,
														uvbox,
														float((gb.advance).x >> 6) - gb.size.x } );


				src_orig.x += gb.size.x;
				if (line_height < gb.size.y)
					line_height = gb.size.y;
			}
			

			FT_Done_Face( face );

			// isn't freetype already giving char codepoint in order?
			std::qsort( m_codepoint_indexes.data(), m_codepoint_indexes.size(), sizeof( m_codepoint_indexes[ 0 ] ), codepoint_index_qsort_comp );

			std::cout << img.size() << '\n';

			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
			faces_atlas = Texture( img );
			glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
		}

		/// @brief FOR BITMAPS
		/// @param glyph_size the size of each glyph bounding box
		/// @param spacing the padding/spacing between glyph's bounding box
		/// @param def indexing of codingpoints
		FORCEINLINE FontInternal(const Image &source, const Vector2i glyph_size, const Vector2i spacing, const BitmapFontDef &def )
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
			const Vector2f atlas_coord_factor = Vector2f( 1.f, 1.f ) / Vector2f( raw_count );

#define GEN_CODEPOINT \
if (def.codepoints && def.codepoints_count > current_codepoint_index) \
     codepoint_value = def.codepoints[ current_codepoint_index++ ] + 1; \
else codepoint_value++;; \
{ \
glyphs.push_back({ codepoint_value - 1, Vector2i(), glyph_size, { Vector2f( atlas_coord_factor.x * x, atlas_coord_factor.y * y ), Vector2f(atlas_coord_factor.x, 0.f), Vector2f(0.f, atlas_coord_factor.y) } }); \
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

			std::qsort( m_codepoint_indexes.data(), m_codepoint_indexes.size(), sizeof( m_codepoint_indexes[ 0 ] ), codepoint_index_qsort_comp );
			
			const Image packed = pack_font_atlas( source, raw_count, glyph_size, spacing );
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
			faces_atlas = Texture( packed );
			glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
		}

		// binary search [BUGBUG IF m_codepoint_indexes.size() < 4]
		size_t get_glyph_index( const codepoint_t cp ) const noexcept
		{
			const size_t sz = m_codepoint_indexes.size();
			REPORT_V( sz < 4, NPos );
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

		Texture faces_atlas;
		std::vector<Glyph> glyphs;
	private:
		std::vector<CodepoinIndex> m_codepoint_indexes; // <- should be sorted based on codepoints
	};

	static std::shared_ptr<Font::FontInternal> get_default_font()
	{
		static std::shared_ptr<Font::FontInternal> DefaultBitmapFont;
		REPORT_V( !is_glew_running(), { nullptr } );
		if (!DefaultBitmapFont)
		{
			static Image DefaultBitmapGlyphImage = Image( (const byte *)DefaultGlyphs, DefaultGlyphsSize, ColorFormat::LA );

			DefaultBitmapFont.reset( new Font::FontInternal( DefaultBitmapGlyphImage, { 8, 8 }, { 0, 0 }, BitmapFontDef() ) );
		}
		return DefaultBitmapFont;
	}

	Font::Font( const std::string &filepath, FT_UInt width, ValidGlyphsPredicate_t glyphs_predicate )
		: m_type{ FontType::TrueType }, m_space_width{ width }, m_internal{ new FontInternal( filepath, width, glyphs_predicate ) }
	{
	}

	Font::Font( const Image &glyphs, Vector2i glyph_size, Vector2i spacing, BitmapFontDef def )
		: m_type{ FontType::Bitmap }, m_space_width{ uint32_t( glyph_size.x ) }, m_internal{
		new FontInternal(glyphs, glyph_size, spacing, def )
	}
	{
	}

	ig::Font::Font()
		: m_type{ FontType::Bitmap }, m_space_width{ 8u }, m_internal{ nullptr }
	{
	}

	Font Font::get_default()
	{
		Font f;
		f.m_type = FontType::Bitmap;
		f.m_space_width = 8;
		f.m_internal = get_default_font();
		return f;
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

	const size_t Font::get_glyphs_count() const
	{
		return m_internal->glyphs.size();
	}

	const size_t Font::get_glyph_index( const codepoint_t codepoint ) const
	{
		return m_internal->get_glyph_index(codepoint);
	}

	void Font::set_char_spacing( int value )
	{
		m_char_spacing = value;
	}

	void Font::set_line_spacing( int value )
	{
		m_line_spacing = value;
	}

	void Font::set_space_width( uint32_t width )
	{
		m_space_width = width;
	}

	int Font::get_line_spacing() const
	{
		return m_line_spacing;
	}

	int Font::get_char_spacing() const
	{
		return m_char_spacing;
	}

	uint32_t Font::get_space_width() const
	{
		return m_space_width;
	}

	bool Font::valid() const
	{
		return m_internal && !m_internal->glyphs.empty();
	}

}

