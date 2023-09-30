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
	struct BaseRectTemplate
	{
		using value_type = _T;
		using this_type = BaseRectTemplate<_T>;
		using vector_type = BaseVector2Template<_T>;

		inline constexpr BaseRectTemplate(value_type x_, value_type y_, value_type w_, value_type h_) noexcept
			: x{ x_ }, y{ y_ }, w{ w_ }, h{ h_ }
		{
		}

		inline constexpr BaseRectTemplate() noexcept
			: x{ value_type() }, y{ value_type() }, w{ value_type() }, h{ value_type() }
		{
		}

		inline constexpr BaseRectTemplate(vector_type pos, vector_type sz) noexcept
			: x{ pos.x }, y{ pos.y }, w{ sz.x }, h{ sz.y }
		{
		}

		template <typename _E>
		inline constexpr BaseRectTemplate(const BaseRectTemplate<_E> &other) noexcept
			: x{ value_type(other.x) }, y{ value_type(other.y) }, w{ value_type(other.w) }, h{ value_type(other.h) }
		{
		}

		inline bool operator==(const this_type &other) const
		{
			return x == other.x && y == other.y && w == other.w && h == other.h;
		}

		inline vector_type position() const
		{
			return vector_type(x, y);
		}

		inline vector_type size() const
		{
			return vector_type(w, h);
		}

		inline vector_type end() const
		{
			return vector_type(x + w, y + h);
		}

		inline void expand(value_type margin)
		{
			x -= margin;
			y -= margin;
			w += margin;
			h += margin;
		}

		inline this_type intersection(const this_type &other) const
		{
			const vector_type pos{ std::max(x, other.x), std::max(y, other.y) };
			return this_type(pos.x, pos.y, std::min(x + w, other.x + other.w) - pos.x, std::min(y + h, other.y + other.h) - pos.y);
		}

		inline bool intersects(const this_type &other) const
		{
			return
				x < other.x + other.w && y < other.y + other.h && x + w > other.x && y + h > other.y;
		}

		// Same as intersects but accounts for borders
		inline bool touches(const this_type &other) const
		{
			return
				x <= other.x + other.w && y <= other.y + other.h && x + w >= other.x && y + h >= other.y;
		}

		value_type x, y, w, h;
	};

	using Rectf = BaseRectTemplate<float>;
	using Rectd = BaseRectTemplate<double>;
	using Recti = BaseRectTemplate<int>;
	using Rectsz = BaseRectTemplate<size_t>;

}

namespace std
{
	template <typename _T>
	inline std::ostream &operator<<(std::ostream &out, const ig::BaseRectTemplate<_T> &rect)
	{
		return out << "Rect(" << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << ')';
	}
}
