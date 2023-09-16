#pragma once
#include "_iglib_base.h"
#include <array>

namespace ig
{

	template <typename _T, size_t _W, size_t _H>
	struct BaseMatrixTemplate
	{
		using size_type = size_t;
		using value_type = _T;
		using this_type = BaseMatrixTemplate<_T, _W, _H>;

		
		constexpr size_type width() const noexcept
		{
			return _W;
		}

		constexpr size_type height() const noexcept
		{
			return _H;
		}

		constexpr size_type size() const noexcept
		{
			return _W * _H;
		}

		std::array<value_type, _W *_H> values;
	};

}
