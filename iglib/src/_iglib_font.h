#pragma once
#include "_iglib_base.h"
#include "_iglib_vector.h"
#include "_iglib_texture.h"
#include <memory>



namespace ig
{
	typedef int codepoint_t;

	enum class FontType
	{
		Bitmap,
		TrueType
	};


	// definintion to allow utf-8/16/32 in bitmap fonts
	// 
	//   if codepoints is a nullptr
	//       the codepoint for generated glyphs will be increments of start_codepoint
	//   if there is codepoints defined but their count is not enough
	//       the codepoints for generated glyphs will be increments of the last codepoint
	struct BitmapFontDef
	{
		codepoint_t start_codepoint = 0;
		int codepoints_count = 0;
		std::shared_ptr<codepoint_t[]> codepoints = { nullptr };
		bool transposed = false;
	};

	// should the glyph with codepoint 'codepoint' be loaded to the font?
	typedef bool (*ValidGlyphsPredicate_t)(codepoint_t codepoint);

	class Font
	{
		friend class Canvas;
	public:
		static constexpr size_t NPos = static_cast<size_t>(-1);

		struct UVBox
		{
			Vector2f origin;
			Vector2f left;
			Vector2f bottom;
		};

		struct Glyph
		{
			codepoint_t codepoint;
			Vector2i offset, size;
			UVBox atlas_uvbox;
			float advance;
		};


		Font( const std::string &filepath, uint32_t width = 14u, ValidGlyphsPredicate_t glyphs_predicate = nullptr ); // <- truetype
		Font( const Image &glyphs, Vector2i glyph_size, Vector2i spacing = { 0, 0 }, BitmapFontDef def = {} ); // <- bitmap
		Font();// <- default bitmap font

		TextureId_t get_atlas() const;

		Glyph *get_glyphs();
		const Glyph *get_glyphs() const;

		const size_t get_glyphs_count() const;

		// might return NPos if no glyph has the codepoint
		const size_t get_glyph_index( const codepoint_t codepoint ) const;

		void set_char_spacing( int value );
		int get_char_spacing() const;

		void set_line_spacing( int value );
		int get_line_spacing() const;

		// the default space width is set automaticly per font
		void set_space_width( uint32_t width );
		uint32_t get_space_width() const;

		bool valid() const;

		struct FontInternal;
	private:
		FontType m_type;
		uint32_t m_space_width;
		int m_char_spacing = 0;
		int m_line_spacing = 0;
		std::shared_ptr<FontInternal> m_internal;
	};

}

