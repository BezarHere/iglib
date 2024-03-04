#pragma once
#include "_iglib_base.h"
#include "_iglib_image.h"

namespace ig
{
	typedef unsigned int TextureId;


	enum class TextureSlot
	{
		Slot0, Slot1, Slot2, Slot3,
		Slot4, Slot5, Slot6, Slot7,
		_MAX
	};

	// an 2D array of pixel saved in the VRAM (currently only GL_TEXTURE_2D is supported)
	class Texture
	{
		friend class Canvas;
		friend class Context3D;
	public:
		Texture();
		Texture(const Image &img);
		Texture(const Texture &copy) noexcept;
		Texture(Texture &&move) noexcept;

		/// @warn This will waste the old texture buffer (no way to not wast it) so please, PLEASE AVOID REPLACING THE TEXTURE BUFFER AFTER ASSIGNMENT
		Texture &operator=(const Texture &copy);
		
		/// @warn Will waste the old texture (no way to not wast the old one) so please, PLEASE AVOID REPLACING THE TEXTURE BUFFER AFTER ASSIGNMENT
		Texture &operator=(Texture &&move) noexcept;

		~Texture() noexcept;
		
		bool is_valid() const noexcept;

		void bind() const noexcept;
		bool is_bound() const noexcept;

		static TextureId get_bound();
		static void clear_bound();
		static void activate_slot( TextureSlot slot );
		static TextureSlot get_active_slot();


		Vector2i size() const noexcept;
		ColorFormat get_channels() const noexcept;

		TextureId get_handle() const noexcept;


		struct _TextureInternal;
	private:
		std::unique_ptr<_TextureInternal> m_internal;
	};
}
