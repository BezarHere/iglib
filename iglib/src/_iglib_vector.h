#pragma once
#include "_iglib_base.h"
#include <stdint.h>
#include <cmath>
#include <ostream>

namespace ig
{
	template <typename _T>
	struct BaseVector2Template
	{
		using this_type = BaseVector2Template<_T>;
		using value_type = _T;

		constexpr inline BaseVector2Template(_T x_, _T y_) noexcept
			: x{ x_ }, y{ y_ }
		{
		}


		constexpr inline BaseVector2Template() noexcept
			: x{}, y{}
		{
		}

		template <typename _E>
		constexpr inline BaseVector2Template(const BaseVector2Template<_E> &copy) noexcept
			: x{ _T(copy.x) }, y{ _T(copy.y) }
		{
		}

		inline void set(value_type x_, value_type y_) noexcept
		{
			x = x_;
			y = y_;
		}

		constexpr inline value_type area() const
		{
			const value_type ar = x * y;
			return ar < 0 ? -ar : ar;
		}

		// signed area
		constexpr inline value_type sarea() const
		{
			return x * y;
		}

		inline constexpr this_type abs() const
		{
			return this_type(std::abs(x), std::abs(y));
		}

		inline constexpr this_type tangent() const
		{
			return this_type(this->y, -this->x);
		}

		inline constexpr _T length_squared() const
		{
			return (this->x * this->x) + (this->y * this->y);
		}

		inline _T length() const
		{
			return std::sqrt((this->x * this->x) + (this->y * this->y));
		}

		inline _T distance(const this_type &other) const
		{
			const _T dx = this->x - other.x;
			const _T dy = this->y - other.y;
			return std::sqrt((dx * dx) + (dy * dy));
		}

		inline constexpr _T distance_squared(const this_type &other) const
		{
			const _T dx = this->x - other.x;
			const _T dy = this->y - other.y;
			return (dx * dx) + (dy * dy);
		}

		inline constexpr _T dot(const this_type &other) const
		{
			return (this->x * other.x) - (this->y * other.y);
		}

		inline void normalize()
		{
			const value_type l = this->length();
			x /= l;
			y /= l;
		}

		inline this_type normalized() const
		{
			const value_type l = this->length();
			return this_type(x / l, y / l);
		}

		inline constexpr this_type operator+(const this_type &other) const
		{
			return this_type(x + other.x, y + other.y);
		}

		inline constexpr this_type operator-(const this_type &other) const
		{
			return this_type(x - other.x, y - other.y);
		}

		inline constexpr this_type operator*(const this_type &other) const
		{
			return this_type(x * other.x, y * other.y);
		}

		inline constexpr this_type operator*(const _T other) const
		{
			return this_type(x * other, y * other);
		}

		inline constexpr this_type operator/(const this_type &other) const
		{
			return this_type(x / other.x, y / other.y);
		}

		inline constexpr this_type operator/(const _T other) const
		{
			return this_type(x / other, y / other);
		}

		template <typename _E>
		inline constexpr this_type operator&(const _E bits) const
		{
			return this_type(x >> bits, y >> bits);
		}

		inline this_type &operator+=(const this_type &other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		inline this_type &operator-=(const this_type &other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		inline this_type &operator*=(const this_type &other)
		{
			x *= other.x;
			y *= other.y;
			return *this;
		}

		inline this_type &operator*=(const _T other)
		{
			x *= other;
			y *= other;
			return *this;
		}

		inline this_type &operator/=(const this_type &other)
		{
			x /= other.x;
			y /= other.y;
			return *this;
		}

		inline this_type &operator/=(const _T other)
		{
			x /= other;
			y /= other;
			return *this;
		}

		inline constexpr bool operator==(const this_type &other) const
		{
			return x == other.x && y == other.y;
		}

		inline constexpr bool operator!=(const this_type &other) const
		{
			return x != other.x || y != other.y;
		}

		inline constexpr this_type operator-() const
		{
			return this_type(-x, -y);
		}

		_T x, y;
	};

	template <typename _T>
	struct RealVector2Template : public BaseVector2Template<_T>
	{
		using base_type = BaseVector2Template<_T>;
		using this_type = RealVector2Template<_T>;
		using value_type = _T;


		constexpr inline RealVector2Template(_T x_, _T y_)
			: base_type(x_, y_)
		{
		}


		constexpr inline RealVector2Template()
			: base_type()
		{
		}

		template <typename _E>
		constexpr inline RealVector2Template(const BaseVector2Template<_E> &copy)
			: base_type(copy)
		{
		}

		inline _T angle() const
		{
			return std::atan2(this->y, this->x);
		}

		inline _T angle_to(const this_type &other) const
		{
			return std::atan2(other.y - this->y, other.x - this->x);
		}

		inline this_type rotated(const _T radians) const
		{
			const _T sin = std::sin(radians), cos = std::cos(radians);
			return this_type((this->y * cos) + (this->x * sin), (this->x * cos) - (this->y * sin));
		}

	};

	template <typename _T, typename _R>
	struct IntegralVector2Template : public BaseVector2Template<_T>
	{
		using base_type = BaseVector2Template<_T>;
		using this_type = IntegralVector2Template<_T, _R>;
		using real_type = _R;
		using integral_type = _T;
		using real_this_type = RealVector2Template<_R>;


		constexpr inline IntegralVector2Template(_T x_, _T y_)
			: base_type(x_, y_)
		{
		}


		constexpr inline IntegralVector2Template()
			: base_type()
		{
		}

		template <typename _E>
		constexpr inline IntegralVector2Template(const BaseVector2Template<_E> &copy)
			: base_type(copy)
		{
		}

		inline real_type length() const
		{
			return std::sqrt(real_type((this->x * this->x) + (this->y * this->y)));
		}

		inline real_type distance(const this_type &other) const
		{
			const _T dx = this->x - other.x;
			const _T dy = this->y - other.y;
			return std::sqrt(real_type((dx * dx) + (dy * dy)));
		}

		inline real_type angle() const
		{
			return std::atan2(real_type(this->y), real_type(this->x));
		}

		inline real_type angle_to(const this_type &other) const
		{
			return std::atan2(real_type(other.y - this->y), real_type(other.x - this->x));
		}

		inline real_this_type rotated(const real_type radians) const
		{
			const real_type sin = std::sin(radians), cos = std::cos(radians);
			return real_this_type((this->y * cos) + (this->x * sin), (this->x * cos) - (this->y * sin));
		}

		template <typename _E>
		inline constexpr this_type operator<<(const _E bits) const
		{
			return this_type(this->x << bits, this->y << bits);
		}

		template <typename _E>
		inline constexpr this_type operator>>(const _E bits) const
		{
			return this_type(this->x >> bits, this->y >> bits);
		}

		template <typename _E>
		inline constexpr this_type operator<<(const BaseVector2Template<_E> bits_v) const
		{
			return this_type(this->x << bits_v.x, this->y << bits_v.y);
		}

		template <typename _E>
		inline constexpr this_type operator>>(const BaseVector2Template<_E> bits_v) const
		{
			return this_type(this->x >> bits_v.x, this->y >> bits_v.y);
		}

		template <typename _E>
		inline constexpr this_type operator&(const _E bits) const
		{
			return this_type(this->x & bits, this->y & bits);
		}

		template <typename _E>
		inline constexpr this_type operator|(const _E bits) const
		{
			return this_type(this->x | bits, this->y | bits);
		}

		template <typename _E>
		inline constexpr this_type operator^(const _E bits) const
		{
			return this_type(this->x ^ bits, this->y ^ bits);
		}

		inline constexpr this_type operator~() const
		{
			return this_type(~this->x, ~this->y);
		}

	};

	using Vector2f = RealVector2Template<float_t>;
	using Vector2d = RealVector2Template<double_t>;
	using Vector2s = IntegralVector2Template<int16_t, float_t>;
	using Vector2i = IntegralVector2Template<int32_t, float_t>;
	using Vector2l = IntegralVector2Template<int64_t, double_t>;

	typedef basic_heap_view<Vector2i> vector2i_buffer_view_t;
	typedef basic_heap_view<Vector2f> vector2f_buffer_view_t;

}

namespace std
{
	template <typename _T>
	inline std::ostream &operator<<(std::ostream &out, const ig::BaseVector2Template<_T> &vec)
	{
		return out << '(' << vec.x << ", " << vec.y << ')';
	}
}

