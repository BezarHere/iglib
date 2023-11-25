#pragma once

#include <vector>
#include <array>
#include <string>
#include <chrono>
#include <memory>
#include <stdint.h>

#ifdef NOTE
#error 'NOTE' is used macro and should not be defined before the 'iglib.h' header
#endif
#define NOTE(x)


namespace ig
{
	constexpr float Pi = 3.1415926f;
	constexpr float Tau = Pi * 2.f;
	constexpr float E = 2.7182818f;

	constexpr double dPi = 3.141592653589793;
	constexpr double dTau = dPi * 2.0;
	constexpr double dE = 2.718281828459045;

	constexpr float Epsilon = 1e-4f;
	constexpr double dEpsilon = 1e-7f;

	typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds> TimeMs_t;
	typedef uint8_t byte;
	typedef void(*Action_t)();
	struct Report
	{
		int code;
		std::string msg;
	};

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

	using BufferView = basic_heap_view<byte>;
	using StrView = basic_heap_view<char>;

	template <typename _T>
	struct basic_heap_span
	{

		basic_heap_span()
			: ptr{ nullptr }, sz{ 0 }
		{
		}

		basic_heap_span(size_t size)
			: ptr{ new _T[ size ]{} }, sz{ size }
		{
		}

		basic_heap_span(_T *pp, size_t size)
			: ptr{ pp }, sz{ size }
		{
		}


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
			return ptr.get();
		}

		inline const _T *end() const
		{
			return ptr.get() + sz;
		}

		std::shared_ptr<_T[]> ptr;
		size_t sz;
	};
	
	using BufferSpan = basic_heap_span<byte>;
	using StrSpan = basic_heap_span<char>;

}


