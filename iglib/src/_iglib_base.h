#pragma once

#include <vector>
#include <array>
#include <string>
#include <chrono>

namespace ig
{
	typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds> TimeMs_t;
	typedef uint8_t byte;
	typedef void(*Action_t)();

	template <typename _T, typename _E>
	__forceinline void swapref(_T &a, _E &b)
	{
		const _T &c = a;
		a = b;
		b = c;
	}

	template <typename _T>
	struct basic_heap_view
	{

		inline _T *begin()
		{
			return ptr;
		}

		inline _T *end()
		{
			return ptr + sz;
		}

		inline const _T *begin() const
		{
			return ptr;
		}

		inline const _T *end() const
		{
			return ptr + sz;
		}

		_T *ptr;
		size_t sz;
	};

	typedef basic_heap_view<byte> buffer_view_t;
	typedef basic_heap_view<char> str_view_t;
	typedef basic_heap_view<size_t> sztable_view_t;

}


