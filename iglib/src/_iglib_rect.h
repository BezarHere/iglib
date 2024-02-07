#pragma once
#include "_iglib_vector.h"
#include <utility>
#include <ostream>

namespace ig
{

	typedef struct
	{
		Vector2f v[ 4 ];
	} Quad_t;

	template <typename _T>
	struct BaseRect2Template
	{
		using value_type = _T;
		using this_type = BaseRect2Template<_T>;
		using vector_type = BaseVector2Template<_T>;

		inline constexpr BaseRect2Template(value_type x_, value_type y_, value_type w_, value_type h_) noexcept
			: x{ x_ }, y{ y_ }, w{ w_ }, h{ h_ }
		{
		}

		inline constexpr BaseRect2Template() noexcept
			: x{ value_type() }, y{ value_type() }, w{ value_type() }, h{ value_type() }
		{
		}

		inline constexpr BaseRect2Template(vector_type pos, vector_type sz) noexcept
			: x{ pos.x }, y{ pos.y }, w{ sz.x }, h{ sz.y }
		{
		}

		template <typename _E>
		inline constexpr BaseRect2Template(const BaseRect2Template<_E> &other) noexcept
			: x{ value_type(other.x) }, y{ value_type(other.y) }, w{ value_type(other.w) }, h{ value_type(other.h) }
		{
		}

		inline constexpr bool operator==(const this_type &other) const
		{
			return x == other.x && y == other.y && w == other.w && h == other.h;
		}

		inline constexpr vector_type position() const
		{
			return vector_type(x, y);
		}

		inline constexpr vector_type size() const
		{
			return vector_type(w, h);
		}

		inline constexpr vector_type end() const
		{
			return vector_type(x + w, y + h);
		}

		inline void expand(value_type margin) noexcept
		{
			x -= margin;
			y -= margin;
			w += margin;
			h += margin;
		}

		inline this_type expanded(value_type margin) const noexcept(this_type::expand)
		{
			this_type copy = *this;
			copy.expand( margin );
			return copy;
		}

		inline constexpr this_type intersection(const this_type &other) const
		{
			const vector_type pos{ std::max(x, other.x), std::max(y, other.y) };
			return this_type(pos.x, pos.y, std::min(x + w, other.x + other.w) - pos.x, std::min(y + h, other.y + other.h) - pos.y);
		}

		inline constexpr bool intersects(const this_type &other) const
		{
			return
				x < other.x + other.w && y < other.y + other.h && x + w > other.x && y + h > other.y;
		}

		// Same as intersects but accounts for borders
		inline constexpr bool touches(const this_type &other) const
		{
			return
				x <= other.x + other.w && y <= other.y + other.h && x + w >= other.x && y + h >= other.y;
		}

		inline constexpr bool contains( vector_type point ) const {
			return x < point.x && y < point.y && x + w > point.x && y + h > point.y;
		}

		template <typename _U>
		inline constexpr bool contains( BaseVector2Template<_U> point ) const {
			return contains( vector_type( point ) );
		}

		/// @brief translates and resizes the rect to incase `other`
		inline void incase( const this_type &other )
		{
			if (x > other.x)
				x = other.x;
			if (y > other.y)
				y = other.y;

			if (x + w < other.x + other.w)
				w = other.x + other.w - x;
			if (y + h < other.y + other.h)
				h = other.y + other.h - y;
		}

		/// @brief translates and resizes the rect to incase `other`
		inline this_type incased( const this_type &other ) const noexcept(this_type::incase) {
			this_type copy = *this;
			copy.incase(other);
			return copy;
		}

		value_type x, y, w, h;
	};

	using Rect2f = BaseRect2Template<float>;
	using Rect2d = BaseRect2Template<double>;
	using Rect2i = BaseRect2Template<int>;
	using Rect2sz = BaseRect2Template<size_t>;

}

namespace std
{
	template <typename _T>
	inline std::ostream &operator<<(std::ostream &out, const ig::BaseRect2Template<_T> &rect)
	{
		return out << "Rect(" << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << ')';
	}
}
