#include "pch.h"
#include "_iglib_texture.h"
#include "draw_internal.h"
#include "internal.h"
#include <map>

struct Texture::_TextureInternal
{

	FORCEINLINE ~_TextureInternal()
	{
		if (handle)
			glDeleteTextures(count, &handle);
	}

	TextureId_t handle;
	GLsizei count = 1; // <- might be a VERY buggy thing
	uint32_t w = 0, h = 0;
	Channels c = Channels::Invalid;
	std::shared_ptr<unsigned char[]> buffer{};
};

using TextureInternal = Texture::_TextureInternal;

static TextureId_t g_BindedHdl;


FORCEINLINE std::unique_ptr<TextureInternal> register_tex(uint32_t w, uint32_t h, Channels c, std::shared_ptr<unsigned char[]> buf)
{
	std::unique_ptr<TextureInternal> tex{ new TextureInternal{} };
	TextureId_t hdl = 0;
	glGenTextures(1, &hdl);
	tex->handle = hdl;

	if (tex->handle == NULL)
	{
		glfwerror();
		return tex;
	}

	glBindTexture(GL_TEXTURE_2D, tex->handle);

	glTexImage2D(GL_TEXTURE_2D, 0, to_glpixelformat(c), w, h, 0, to_glpixelformat(c), GL_UNSIGNED_BYTE, buf.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	tex->w = w;
	tex->h = h;
	tex->c = c;
	tex->buffer = buf;

	return tex;
}

FORCEINLINE std::unique_ptr<TextureInternal> duplicate_tex(const TextureInternal *inter)
{
	if (!inter->handle)
		return NULL;

	ASSERT(inter->buffer.get() != nullptr);

	return register_tex(inter->w, inter->h, inter->c, inter->buffer);
}

namespace ig
{
	Texture::Texture()
		: m_internal{ register_tex(0, 0, Channels::Invalid, std::shared_ptr<unsigned char[]>()) }
	{
	}

	Texture::Texture(const Image &img)
		: m_internal{
			register_tex(
				img.get_width(), img.get_height(), img.get_channels(),
				std::shared_ptr<unsigned char[]>((unsigned char *)memcpy(new unsigned char[img.get_buffer_size()] , img.get_buffer(), img.get_buffer_size()))
			)
		}
	{
	}

	Texture::Texture(const Texture &copy) noexcept
		: m_internal{ duplicate_tex(copy.m_internal.get()) }
	{
	}

	Texture::Texture(Texture &&move) noexcept
		: m_internal{ move.m_internal.release() }
	{
	}

	Texture &Texture::operator=(const Texture &copy)
	{
		m_internal = duplicate_tex(copy.m_internal.get());
		return *this;
	}

	Texture &Texture::operator=(Texture &&move) noexcept
	{
		m_internal.swap(move.m_internal);
		return *this;
	}

	Texture::~Texture() noexcept
	{
	}

	bool Texture::is_valid() const noexcept
	{
		return m_internal->handle;
	}

	void Texture::bind() const noexcept
	{
		if (g_BindedHdl && g_BindedHdl != m_internal->handle)
			return;

		glBindTexture(GL_TEXTURE_2D, m_internal->handle);
		g_BindedHdl = m_internal->handle;
	}

	void Texture::unbind() const noexcept
	{
		glBindTexture(GL_TEXTURE_2D, NULL);
		g_BindedHdl = NULL;
	}

	bool Texture::is_binded() const noexcept
	{
		return g_BindedHdl == m_internal->handle;
	}

	Vector2i Texture::get_size() const noexcept
	{
		return { (int)m_internal->w, (int)m_internal->h };
	}

	Channels Texture::get_channels() const noexcept
	{
		return m_internal->c;
	}

	TextureId_t Texture::get_handle() const noexcept
	{
		return m_internal->handle;
	}
}
