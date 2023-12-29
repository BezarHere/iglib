#pragma once
#include "_iglib_base.h"

namespace ig
{
	typedef unsigned int renderbuffer_name;
	class RenderBuffer
	{
	public:
		RenderBuffer();
		RenderBuffer(std::_Uninitialized);
		RenderBuffer( RenderBuffer && ) noexcept;
		RenderBuffer &operator=( RenderBuffer && ) noexcept;

		inline renderbuffer_name get_name() const noexcept {
			return m_name;
		}

	private:
		RenderBuffer( const RenderBuffer & ) = delete;
		RenderBuffer &operator=( const RenderBuffer & ) = delete;

	private:
		renderbuffer_name m_name;
	};
}
