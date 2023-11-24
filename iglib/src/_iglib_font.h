#pragma once
#include "_iglib_base.h"
#include "_iglib_vector.h"
#include "_iglib_texture.h"
#include <memory>



namespace ig
{
	
	enum class FontType
	{
		Bitmap,
		TrueType
	};

	class Font
	{
		friend class Canvas;
	public:
		struct Charecter
		{
			int codepoint;
			Vector2i pivot, size;
		};


		Font(const std::string &filepath); // <- truetype
		Font(const Image &glyphs, Vector2i glyph_size, Vector2i spacing = {0, 0}); // <- bitmap
		Font();// <- default bitmap font


		struct FontInternal;
	private:
		FontType m_type;
		std::shared_ptr<FontInternal> m_internal;
	};

}

