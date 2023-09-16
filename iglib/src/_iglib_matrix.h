#pragma once
#include "_iglib_base.h"
#include <type_traits>
#include <array>
#include <vector>

namespace ig
{

	template <typename _T, size_t _W, size_t _H>
	struct BaseMatrixTemplate
	{
		static_assert(std::is_arithmetic_v<_T>, "class Matrix only accepts arithmetic types only as _T");
		static_assert(_W > 0, "class Matrix should have a width (_W) greater then zero");
		static_assert(_H > 0, "class Matrix should have a height (_H) greater then zero");

		using size_type = size_t;
		using value_type = _T;
		using this_type = BaseMatrixTemplate<_T, _W, _H>;
		using array_type = std::array<value_type, _W *_H>;
		using column_matrix = BaseMatrixTemplate<_T, 1, _H>;
		using row_matrix = BaseMatrixTemplate<_T, _W, 1>;

		inline BaseMatrixTemplate(array_type arr)
			: values{ std::move(arr) }
		{}

		inline BaseMatrixTemplate()
			: values{}
		{}

		inline constexpr size_type width() const noexcept
		{
			return _W;
		}

		inline constexpr size_type height() const noexcept
		{
			return _H;
		}

		inline constexpr size_type size() const noexcept
		{
			return _W * _H;
		}

		template <typename _E>
		inline bool operator==(const BaseMatrixTemplate<_E, _W, _H> &other)
		{
			for (size_t i{}; i < size(); i++)
			{
				if (values[ i ] != other.values[ i ])
					return false;
			}
			return true;
		}

		// diffrent size matrix, not equal
		template <typename _E, size_t _Wx, size_t _Hx>
		inline bool operator==(const BaseMatrixTemplate<_E, _Wx, _Hx> &other)
		{
			return false;
		}

		inline value_type &operator[](const size_type index)
		{
			return values[ index ];
		}

		inline const value_type &operator[](const size_type index) const
		{
			return values[ index ];
		}

		array_type values;
	};

}
