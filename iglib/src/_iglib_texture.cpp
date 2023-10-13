#include "pch.h"
#include "_iglib_texture.h"

FORCEINLINE glTextureHdl_t copy_texture_buffers(const Texture &tex)
{
	glTextureHdl_t ghdl;
	glGenTextures(1, &ghdl);
	glBindTexture(GL_TEXTURE_2D, ghdl);
}

namespace ig
{
	Texture::Texture(const Texture &copy) noexcept
		: m_handle{  }
	{
	}

	Texture::Texture(Texture &&move) noexcept
	{
	}

	Texture &Texture::operator=(const Texture &copy)
	{
		return *this;
	}

	Texture &Texture::operator=(Texture &&move) noexcept
	{
		return *this;
	}

	Texture::~Texture() noexcept
	{

	}

	glTextureHdl_t Texture::get_opengl_handle() const noexcept
	{
		return m_handle;
	}
}
