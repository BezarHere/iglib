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

}


