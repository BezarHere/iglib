#include "pch.h"
#include "_iglib_texture.h"

glTextureHdl_t g_BindedHdl;

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

	void Texture::bind() const noexcept
	{
		glBindTexture(GL_TEXTURE_2D, m_handle);
		g_BindedHdl = m_handle;
	}

	void Texture::unbind() const noexcept
	{
		glBindTexture(GL_TEXTURE_2D, NULL);
		g_BindedHdl = NULL;
	}

	bool Texture::is_binded() const noexcept
	{
		return g_BindedHdl = m_handle;
	}

	glTextureHdl_t Texture::get_opengl_handle() const noexcept
	{
		return m_handle;
	}
}
