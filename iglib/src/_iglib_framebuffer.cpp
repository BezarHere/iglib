#include "pch.h"
#include "_iglib_framebuffer.h"

#define PUSH_FBS(t, new_n) int _last_fb; glGetIntegerv(t##_BINDING, &_last_fb); glBindFramebuffer(t, new_n)
#define POP_FBS(t) glBindFramebuffer(t, _last_fb)

namespace ig
{
	Framebuffer::Framebuffer()
		: Framebuffer(nullptr) {
		glGenFramebuffers( 1, &m_name );
	}

	Framebuffer::Framebuffer( std::nullptr_t ) noexcept
		: m_name{ 0 } {
	}

	Framebuffer::Framebuffer( Framebuffer &&move ) noexcept
		: m_name{ move.m_name } {
		move.m_name = 0;
	}

	Framebuffer &Framebuffer::operator=( Framebuffer &&move ) noexcept {
		if (move.m_name == m_name)
			return *this;
		glDeleteFramebuffers( 1, &m_name );
		m_name = move.m_name;
		move.m_name = 0;
		return *this;
	}

	Framebuffer::~Framebuffer() {
		glDeleteFramebuffers( 1, &m_name );
	}

	void Framebuffer::attach_texture( FramebufferAttachmentSlot slot, TextureId_t tex_id ) {
		PUSH_FBS( GL_DRAW_FRAMEBUFFER, m_name );
		glFramebufferTexture( GL_FRAMEBUFFER, (GLenum)slot, tex_id, 0 );
		POP_FBS( GL_DRAW_FRAMEBUFFER );
	}
}
