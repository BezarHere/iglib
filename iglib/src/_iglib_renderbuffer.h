#pragma once
#include "_iglib_base.h"
#include "_iglib_vector.h"

namespace ig
{
	enum class RenderBufferStorage
	{
		Depth24Stencil8 = 0x88F0, // 24 bits depth and 8 bits stencil
	};

	typedef unsigned int RenderbufferId;
	class RenderBuffer
	{
	public:
		RenderBuffer();
		RenderBuffer( std::_Uninitialized ) noexcept;
		RenderBuffer( RenderBuffer &&move ) noexcept;
		RenderBuffer &operator=( RenderBuffer &&move ) noexcept;

		~RenderBuffer() noexcept;

		void setup_storage(RenderBufferStorage type, Vector2i size);

		inline RenderbufferId get_name() const noexcept {
			return m_name;
		}

	private:
		RenderBuffer( const RenderBuffer & ) = delete;
		RenderBuffer &operator=( const RenderBuffer & ) = delete;

	private:
		RenderbufferId m_name;
	};
}
