#pragma once
#include "_iglib_texture.h"

namespace ig
{
	enum class FramebufferAttachmentSlot
	{
		Color0 = 0x8CE0, // <- specific value, do not change
		Color1,
		Color2,
		Color3,
		Color4,
		Color5,
		Color6,
		Color7,
		Color8,
		Color9,
		Color10,
		Color11,
		Color12,
		Color13,
		Color14,
		Color15,
		Depth = 0x8D00,
		Stencil = 0x8D20,
		DepthStencil = 0x821A
	};

	typedef unsigned int framebuffer_id;
	class Framebuffer
	{
	public:
		Framebuffer();
		// will not create the framebuffer
		Framebuffer( std::nullptr_t ) noexcept;
		Framebuffer( Framebuffer &&move ) noexcept;
		Framebuffer &operator=( Framebuffer &&move ) noexcept;
		~Framebuffer();

		void attach_texture( FramebufferAttachmentSlot slot, TextureId_t tex_id );
		//void attach_render_buffer( FramebufferAttachmentSlot slot, renderbuffer_id rb_id );

		inline framebuffer_id get_id() const noexcept {
			return m_id;
		}

	private:
		// too much hassle
		Framebuffer( const Framebuffer & ) = delete;
		Framebuffer &operator=( const Framebuffer & ) = delete;

	private:
		framebuffer_id m_id;
	};

}
