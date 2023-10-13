#pragma once
#include "_iglib_base.h"
#include "_iglib_image.h"

namespace ig
{
	typedef unsigned int glTextureHdl_t;

	class Texture
	{
		friend class Context2D;
		friend class Context3D;
	public:
		Texture(const Texture &copy) noexcept;
		Texture(Texture &&move) noexcept;

		Texture &operator=(const Texture &copy);
		Texture &operator=(Texture &&move) noexcept;

		~Texture() noexcept;
		
		glTextureHdl_t get_opengl_handle() const noexcept;

	private:
		bool m_locked;
		glTextureHdl_t m_handle;
	};
}
