#include "pch.h"
#include "_iglib_texture.h"
#include "draw_internal.h"
#include "internal.h"

struct TextureInternal
{
	uint32_t w, h;
	Channels c;
	std::shared_ptr<unsigned char[]> buffer;
};

std::vector<TextureInternal> g_tex(1024);
glTextureHdl_t g_BindedHdl;

FORCEINLINE glTextureHdl_t register_tex(uint32_t w, uint32_t h, Channels c, std::shared_ptr<unsigned char[]> buf)
{
	glTextureHdl_t ghdl;
	glGenTextures(1, &ghdl);

	if (ghdl == NULL)
	{
		glfwerror();
		return 0;
	}

	glBindTexture(GL_TEXTURE_2D, ghdl);

	glTexImage2D(GL_TEXTURE_2D, 0, to_glpixelformat(c), w, h, 0, to_glpixelformat(c), GL_UNSIGNED_BYTE, buf.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (g_tex.size() < ghdl)
		g_tex.resize((size_t)(ghdl * 2));

	g_tex[ ghdl ].w = w;
	g_tex[ ghdl ].h = h;
	g_tex[ ghdl ].c = c;
	g_tex[ ghdl ].buffer = buf;

	return ghdl;
}

FORCEINLINE void unregister_tex(glTextureHdl_t hdl)
{
	if (!hdl)
		return;
	g_tex[ hdl ].buffer.reset();
	glDeleteTextures(1, &hdl);
}

FORCEINLINE glTextureHdl_t duplicate_tex(const glTextureHdl_t hdl)
{
	if (!hdl)
		return NULL;

	ASSERT(g_tex.size() > hdl);
	ASSERT(g_tex[ hdl ].buffer.get() != nullptr);

	return register_tex(g_tex[ hdl ].w, g_tex[ hdl ].h, g_tex[ hdl ].c, g_tex[ hdl ].buffer);
}

namespace ig
{
	Texture::Texture()
		: m_handle{ register_tex(0, 0, Channels::Invalid, std::shared_ptr<unsigned char[]>()) }
	{
	}

	Texture::Texture(const Image &img)
		: m_handle{
			register_tex(
				img.get_width(), img.get_height(), img.get_channels(),
				std::shared_ptr<unsigned char[]>(memcpy(new unsigned char[img.get_buffer_size()] , img.get_buffer(), img.get_buffer_size()))
			)
		}
	{
	}

	Texture::Texture(const Texture &copy) noexcept
		: m_handle{ duplicate_tex(copy.m_handle) }
	{
	}

	Texture::Texture(Texture &&move) noexcept
		: m_handle{ move.m_handle }
	{
		move.m_handle = NULL;
	}

	Texture &Texture::operator=(const Texture &copy)
	{
		unregister_tex(m_handle);
		m_handle = duplicate_tex(copy.m_handle);
		return *this;
	}

	Texture &Texture::operator=(Texture &&move) noexcept
	{
		unregister_tex(m_handle);
		m_handle = move.m_handle;
		move.m_handle = NULL;
		return *this;
	}

	Texture::~Texture() noexcept
	{
		unregister_tex(m_handle);
	}

	bool Texture::is_valid() const noexcept
	{
		return m_handle;
	}

	void Texture::bind() const noexcept
	{
		if (g_BindedHdl && g_BindedHdl != m_handle)
			return;

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
		return g_BindedHdl == m_handle;
	}

	Vector2i Texture::get_size() const noexcept
	{
		return { g_tex[ m_handle ].w, g_tex[ m_handle ].h };
	}

	Channels Texture::get_channels() const noexcept
	{
		return g_tex[ m_handle ].c;
	}

	glTextureHdl_t Texture::get_handle() const noexcept
	{
		return m_handle;
	}
}
