#include "pch.h"
#include "_iglib_renderbuffer.h"

namespace ig
{

	RenderBuffer::RenderBuffer() : RenderBuffer( std::_Uninitialized::_Noinit ) {
		glGenRenderbuffers( 1, &m_name );
	}

	RenderBuffer::RenderBuffer( std::_Uninitialized ) noexcept : m_name{ 0 } {
	}

	RenderBuffer::RenderBuffer( RenderBuffer &&move ) noexcept : m_name{ move.m_name } {
		move.m_name = 0;
	}

	RenderBuffer &RenderBuffer::operator=( RenderBuffer &&move ) noexcept {
		glDeleteRenderbuffers( 1, &m_name );
		m_name = move.m_name;
		move.m_name = 0;
		return *this;
	}

	RenderBuffer::~RenderBuffer() noexcept {
		glDeleteRenderbuffers( 1, &m_name );
	}

	void RenderBuffer::setup_storage( RenderBufferStorage type, Vector2i size ) {
		glRenderbufferStorage( GL_RENDERBUFFER, static_cast<GLenum>(type), size.x, size.y );
	}

}
