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

	class Font
	{
		friend class Canvas;
	public:
		static constexpr size_t NPos = static_cast<size_t>(-1);
		
		struct Glyph
		{
			codepoint_t codepoint;
			Vector2i offset, size;
			Vector2i atlas_coord;
		};


		Font(const std::string &filepath); // <- truetype
		Font( const Image &glyphs, Vector2i glyph_size, Vector2i spacing = { 0, 0 }, BitmapFontDef def = {} ); // <- bitmap
		Font();// <- default bitmap font

		TextureId_t get_atlas() const;

		Glyph *get_glyphs();
		const Glyph *get_glyphs() const;

		const int get_glyphs_count() const;

		// might return NPos if no glyph has the codepoint
		const size_t get_glyph_index(const codepoint_t codepoint) const;

		const Vector2f get_texcoord_step() const;

		void set_char_spacing(int value);
		int get_char_spacing() const;

		void set_line_spacing( int value );
		int get_line_spacing() const;

		struct FontInternal;
	private:
		FontType m_type;
		int m_char_spacing = 0;
		int m_line_spacing = 0;
		std::shared_ptr<FontInternal> m_internal;
	};

}

