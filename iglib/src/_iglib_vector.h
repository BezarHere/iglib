#pragma once
#include "_iglib_base.h"
#include <stdint.h>
#include <cmath>
#include <ostream>

namespace ig
{
	template <typename _T, typename _R = _T>
	struct BaseVector2Template
	{
		using this_type = BaseVector2Template<_T, _R>;
		using real_this_type = BaseVector2Template<_R, _R>;
		using value_type = _T;
		using real_type = _R;

		constexpr inline BaseVector2Template(value_type xx, value_type yy) noexcept
			: x{ xx }, y{ yy }
		{
		}


		constexpr inline BaseVector2Template() noexcept
			: x{}, y{}
		{
		}

		template <typename _E>
		constexpr inline BaseVector2Template(const BaseVector2Template<value_type, _E> &copy) noexcept
			: x{ value_type(copy.x) }, y{ value_type(copy.y) }
		{
		}

		template <typename _E, typename _V>
		constexpr inline BaseVector2Template(const BaseVector2Template<_E, _V> &copy) noexcept
			: x{ value_type(copy.x) }, y{ value_type(copy.y) }
		{
		}

		// for win32's Point & PointF
		template <typename _P>
		constexpr inline explicit BaseVector2Template(const _P &copy) noexcept
			: x{ value_type(copy.X) }, y{ value_type(copy.Y) }
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

		inline constexpr value_type length_squared() const
		{
			return (this->x * this->x) + (this->y * this->y);
		}

		inline real_type length() const
		{
			return std::sqrt(real_type(this->x * this->x) + real_type(this->y * this->y));
		}

		inline real_type distance(const this_type &other) const
		{
			const value_type dx = this->x - other.x;
			const value_type dy = this->y - other.y;
			return std::sqrt( real_type(dx * dx) + real_type(dy * dy));
		}

		inline constexpr value_type distance_squared(const this_type &other) const
		{
			const value_type dx = this->x - other.x;
			const value_type dy = this->y - other.y;
			return (dx * dx) + (dy * dy);
		}

		inline this_type direction(const this_type &other) const
		{
			return this_type(other.x - this->x, other.y - this->y).normalized();
		}

		inline constexpr this_type direction_squared(const this_type &other) const
		{
			const value_type dx = other.x - this->x;
			const value_type dy = other.y - this->y;
			return this_type(dx, dy) / ((dx * dx) + (dy * dy));
		}

		inline constexpr value_type dot(const this_type &other) const
		{
			return (this->x * other.x) - (this->y * other.y);
		}

		inline void normalize()
		{
			const real_type l = this->length();
			x /= l;
			y /= l;
		}

		inline this_type normalized() const
		{
			const value_type l = this->length();
			return this_type(x / l, y / l);
		}

		inline real_type angle() const
		{
			return std::atan2( real_type( this->y ), real_type( this->x ) );
		}

		inline real_type angle_to( const this_type &other ) const
		{
			return std::atan2( real_type( other.y - this->y ), real_type(other.x - this->x) );
		}

		inline this_type rotated( const value_type radians ) const
		{
			const value_type sin = std::sin( radians ), cos = std::cos( radians );
			return this_type( (this->y * sin) + (this->x * cos), (this->x * sin) + (this->y * cos) );
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

		inline constexpr this_type operator*(const value_type other) const
		{
			return this_type(x * other, y * other);
		}

		inline constexpr this_type operator/(const this_type &other) const
		{
			return this_type(x / other.x, y / other.y);
		}

		inline constexpr this_type operator/(const value_type other) const
		{
			return this_type(x / other, y / other);
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

		inline this_type &operator*=(const value_type other)
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

		inline this_type &operator/=(const value_type other)
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

		template <typename _E>
		inline constexpr this_type operator&( const _E bits ) const {
			return this_type( this->x >> bits, this->y >> bits );
		}

		template <typename _E>
		inline constexpr this_type operator<<( const _E bits ) const {
			return this_type( this->x << bits, this->y << bits );
		}

		template <typename _E>
		inline constexpr this_type operator>>( const _E bits ) const {
			return this_type( this->x >> bits, this->y >> bits );
		}

		template <typename _E>
		inline constexpr this_type operator<<( const BaseVector2Template<_E> bits_v ) const {
			return this_type( this->x << bits_v.x, this->y << bits_v.y );
		}

		template <typename _E>
		inline constexpr this_type operator>>( const BaseVector2Template<_E> bits_v ) const {
			return this_type( this->x >> bits_v.x, this->y >> bits_v.y );
		}

		template <typename _E>
		inline constexpr this_type operator|( const _E bits ) const {
			return this_type( this->x | bits, this->y | bits );
		}

		template <typename _E>
		inline constexpr this_type operator^( const _E bits ) const {
			return this_type( this->x ^ bits, this->y ^ bits );
		}

		inline constexpr this_type operator~() const {
			return this_type( ~this->x, ~this->y );
		}

		value_type x, y;
	};



	template <typename _T>
	struct BaseVector3Template
	{
		using this_type = BaseVector3Template<_T>;
		using value_type = _T;

		constexpr inline BaseVector3Template(value_type xx, value_type yy, value_type zz) noexcept
			: x{ xx }, y{ yy }, z{ zz }
		{
		}


		constexpr inline BaseVector3Template() noexcept
			: x{}, y{}, z{}
		{
		}

		template <typename _E>
		constexpr inline BaseVector3Template(const BaseVector3Template<_E> &copy) noexcept
			: x{ value_type(copy.x) }, y{ value_type(copy.y) }, z{ value_type(copy.z) }
		{
		}

		inline void set(value_type x_, value_type y_, value_type z_) noexcept
		{
			x = x_;
			y = y_;
			z = z_;
		}

		constexpr inline value_type area() const
		{
			const value_type ar = x * y * z;
			return ar < 0 ? -ar : ar;
		}

		// signed area
		constexpr inline value_type sarea() const
		{
			return x * y * z;
		}

		inline constexpr this_type abs() const
		{
			return this_type(std::abs(x), std::abs(y), std::abs(z));
		}

		inline constexpr this_type tangent() const
		{
			return this_type(this->y, -this->x, this->z);
		}

		inline constexpr _T length_squared() const
		{
			return (this->x * this->x) + (this->y * this->y) + (this->z * this->z);
		}

		inline _T length() const
		{
			return std::sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
		}

		inline _T distance(const this_type &other) const
		{
			const _T dx = this->x - other.x;
			const _T dy = this->y - other.y;
			const _T dz = this->z - other.z;
			return std::sqrt((dx * dx) + (dy * dy) + (dz * dz));
		}

		inline constexpr _T distance_squared(const this_type &other) const
		{
			const _T dx = this->x - other.x;
			const _T dy = this->y - other.y;
			const _T dz = this->z - other.z;
			return (dx * dx) + (dy * dy) + (dz * dz);
		}

		inline constexpr _T dot(const this_type &other) const
		{
			return (this->x * other.x) + (this->y * other.y) + (this->z * other.z);
		}

		inline constexpr this_type cross(const this_type &other) const
		{
			return this_type((y * other.z) - (z * other.y),
											 (z * other.x) - (x * other.z),
											 (x * other.y) - (y * other.x));
		}

		inline constexpr this_type reflect(const this_type &plane_normal) const {
			return value_type(2.0f) * plane_normal * this->dot(plane_normal) - *this;
		}

		inline constexpr this_type bounce(const this_type &plane_normal) const {
			return -this->reflect(plane_normal);
		}

		inline constexpr this_type project(const this_type &other) const
		{
			return other * (this->dot(other) / this->length_squared());
		}

		inline void normalize()
		{
			const value_type l = this->length();
			x /= l;
			y /= l;
			z /= l;
		}

		inline this_type normalized() const
		{
			const value_type l = this->length();
			return this_type(x / l, y / l, z / l);
		}

		inline constexpr this_type squared() const
		{
			return this_type(x * x, y * y, z * z);
		}

		inline this_type sqrt() const
		{
			return this_type(std::sqrt(x), std::sqrt(y), std::sqrt(z));
		}

		inline constexpr this_type operator+(const this_type &other) const
		{
			return this_type(x + other.x, y + other.y, z + other.z);
		}

		inline constexpr this_type operator-(const this_type &other) const
		{
			return this_type(x - other.x, y - other.y, z - other.z);
		}

		inline constexpr this_type operator*(const this_type &other) const
		{
			return this_type(x * other.x, y * other.y, z * other.z);
		}

		inline constexpr this_type operator*(const _T other) const
		{
			return this_type(x * other, y * other, z * other);
		}

		inline constexpr this_type operator/(const this_type &other) const
		{
			return this_type(x / other.x, y / other.y, z / other.z);
		}

		inline constexpr this_type operator/(const _T other) const
		{
			return this_type(x / other, y / other, z / other);
		}

		template <typename _E>
		inline constexpr this_type operator&(const _E bits) const
		{
			return this_type(x >> bits, y >> bits, z >> bits);
		}

		inline this_type &operator+=(const this_type &other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		inline this_type &operator-=(const this_type &other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}

		inline this_type &operator*=(const this_type &other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			return *this;
		}

		inline this_type &operator*=(const _T other)
		{
			x *= other;
			y *= other;
			z *= other;
			return *this;
		}

		inline this_type &operator/=(const this_type &other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
			return *this;
		}

		inline this_type &operator/=(const _T other)
		{
			x /= other;
			y /= other;
			z /= other;
			return *this;
		}

		inline constexpr bool operator==(const this_type &other) const
		{
			return x == other.x && y == other.y && z == other.z;
		}

		inline constexpr bool operator!=(const this_type &other) const
		{
			return x != other.x || y != other.y || z != other.z;
		}

		inline constexpr this_type operator-() const
		{
			return this_type(-x, -y, -z);
		}

		_T x, y, z;
	};

	template <typename _T>
	struct RealVector3Template : public BaseVector3Template<_T>
	{
		using base_type = BaseVector3Template<_T>;
		using this_type = RealVector3Template<_T>;
		using value_type = _T;


		constexpr inline RealVector3Template(value_type xx, value_type yy, value_type zz)
			: base_type(xx, yy, zz)
		{
		}


		constexpr inline RealVector3Template()
			: base_type()
		{
		}

		template <typename _E>
		constexpr inline RealVector3Template(const BaseVector3Template<_E> &copy)
			: base_type(copy)
		{
		}

		// TODO: add angle and angle to

		//inline _T angle() const
		//{
		//	return std::atan2(this->y, this->x);
		//}

		//inline _T angle_to(const this_type &other) const
		//{
		//	return std::atan2(other.y - this->y, other.x - this->x);
		//}

		// TODO: optimize
		inline this_type rotated(const this_type axis, const value_type radians) const
		{
			/// r1, r2, r3 : vector3
			/// this.xyz = r1.dot(this), r2.dot(this), r3.dot(this)

			this_type axis_sq = axis.squared();
			this_type basis[ 3 ];

			float cosine = std::cos(radians);

			basis[ 0 ].x = axis_sq.x + cosine * (1 - axis_sq.x);
			basis[ 1 ].y = axis_sq.y + cosine * (1 - axis_sq.y);
			basis[ 2 ].z = axis_sq.z + cosine * (1 - axis_sq.z);

			float sine = std::sin(radians);
			float t = 1 - cosine;

			float xyzt = axis.x * axis.y * t;
			float zyxs = axis.z * sine;
			basis[ 0 ].y = xyzt - zyxs;
			basis[ 1 ].x = xyzt + zyxs;

			xyzt = axis.x * axis.z * t;
			zyxs = axis.y * sine;
			basis[ 0 ].z = xyzt + zyxs;
			basis[ 2 ].x = xyzt - zyxs;

			xyzt = axis.y * axis.z * t;
			zyxs = axis.x * sine;
			basis[ 1 ].z = xyzt - zyxs;
			basis[ 2 ].y = xyzt + zyxs;

			return this_type(basis[ 0 ].dot(*this), basis[ 1 ].dot(*this), basis[ 2 ].dot(*this));
		}

		//const _T sin = std::sin(radians), cos = std::cos(radians);
		//return this_type((this->y * sin) + (this->x * cos), (this->x * sin) - (this->y * cos));
	};

	template <typename _T, typename _R>
	struct IntegralVector3Template : public BaseVector3Template<_T>
	{
		using base_type = BaseVector3Template<_T>;
		using this_type = IntegralVector3Template<_T, _R>;
		using real_type = _R;
		using integral_type = _T;
		using real_this_type = RealVector3Template<_R>;
		using value_type = integral_type;


		constexpr inline IntegralVector3Template(value_type xx, value_type yy, value_type zz)
			: base_type(xx, yy, zz)
		{
		}


		constexpr inline IntegralVector3Template()
			: base_type()
		{
		}

		template <typename _E>
		constexpr inline IntegralVector3Template(const BaseVector3Template<_E> &copy)
			: base_type(copy)
		{
		}

		inline real_type length() const
		{
			return std::sqrt(real_type((this->x * this->x) + (this->y * this->y) + (this->z * this->z)));
		}

		inline real_type distance(const this_type &other) const
		{
			const value_type dx = this->x - other.x;
			const value_type dy = this->y - other.y;
			const value_type dz = this->z - other.z;
			return std::sqrt(real_type((dx * dx) + (dy * dy) + (dz * dz)));
		}


		// TODO: add angle and angle to
		
		//inline real_type angle() const
		//{
		//	return std::atan2(real_type(this->y), real_type(this->x));
		//}

		//inline real_type angle_to(const this_type &other) const
		//{
		//	return std::atan2(real_type(other.y - this->y), real_type(other.x - this->x));
		//}


		inline real_this_type rotated(const real_this_type &axis, const real_type radians) const
		{
			return real_this_type(*this).rotated(axis, radians);
		}

		template <typename _E>
		inline constexpr this_type operator<<(const _E bits) const
		{
			return this_type(this->x << bits, this->y << bits, this->z << bits);
		}

		template <typename _E>
		inline constexpr this_type operator>>(const _E bits) const
		{
			return this_type(this->x >> bits, this->y >> bits, this->z >> bits);
		}

		template <typename _E>
		inline constexpr this_type operator<<(const BaseVector3Template<_E> bits_v) const
		{
			return this_type(this->x << bits_v.x, this->y << bits_v.y, this->z << bits_v.z);
		}

		template <typename _E>
		inline constexpr this_type operator>>(const BaseVector3Template<_E> bits_v) const
		{
			return this_type(this->x >> bits_v.x, this->y >> bits_v.y, this->z >> bits_v.z);
		}

		template <typename _E>
		inline constexpr this_type operator&(const _E bits) const
		{
			return this_type(this->x & bits, this->y & bits, this->z & bits);
		}

		template <typename _E>
		inline constexpr this_type operator|(const _E bits) const
		{
			return this_type(this->x | bits, this->y | bits, this->z | bits);
		}

		template <typename _E>
		inline constexpr this_type operator^(const _E bits) const
		{
			return this_type(this->x ^ bits, this->y ^ bits, this->z ^ bits);
		}

		inline constexpr this_type operator~() const
		{
			return this_type(~this->x, ~this->y, ~this->z);
		}

	};

	template <typename _T>
	struct BaseVector4Template
	{
		using this_type = BaseVector4Template<_T>;
		using value_type = _T;

		constexpr inline BaseVector4Template(value_type xx, value_type yy, value_type zz, value_type ww) noexcept
			: x{ xx }, y{ yy }, z{ zz }, w{ ww }
		{
		}


		constexpr inline BaseVector4Template() noexcept
			: x{}, y{}, z{}, w{}
		{
		}

		template <typename _E>
		constexpr inline BaseVector4Template(const BaseVector4Template<_E> &copy) noexcept
			: x{ value_type(copy.x) }, y{ value_type(copy.y) }, z{ value_type(copy.z) }, w{ value_type(copy.w) }
		{
		}

		inline void set(value_type xx, value_type yy, value_type zz, value_type ww) noexcept
		{
			x = xx;
			y = yy;
			z = zz;
			w = ww;
		}

		value_type x, y, z, w;
	};

	using Vector2f = BaseVector2Template<float_t>;
	using Vector2d = BaseVector2Template<double_t>;
	using Vector2s = BaseVector2Template<int16_t, float_t>;
	using Vector2u = BaseVector2Template<uint32_t, float_t>;
	using Vector2i = BaseVector2Template<int32_t, float_t>;
	using Vector2l = BaseVector2Template<int64_t, double_t>;

	using Vector3f = RealVector3Template<float_t>;
	using Vector3d = RealVector3Template<double_t>;
	using Vector3s = IntegralVector3Template<int16_t, float_t>;
	using Vector3u = IntegralVector3Template<uint32_t, float_t>;
	using Vector3i = IntegralVector3Template<int32_t, float_t>;
	using Vector3l = IntegralVector3Template<int64_t, double_t>;

	using Vector4i = BaseVector4Template<int32_t>;
	using Vector4u = BaseVector4Template<uint32_t>;
	using Vector4f = BaseVector4Template<float_t>;
	using Vector4d = BaseVector4Template<double_t>;

	typedef basic_heap_view<Vector2i> vector2i_buffer_view_t;
	typedef basic_heap_view<Vector2f> vector2f_buffer_view_t;

	typedef basic_heap_view<Vector3i> vector3i_buffer_view_t;
	typedef basic_heap_view<Vector3f> vector3f_buffer_view_t;

}

namespace std
{

	template <typename _T, typename _R>
	inline std::ostream &operator<<(std::ostream &out, const ig::BaseVector2Template<_T, _R> &vec)
	{
		return out << '(' << vec.x << ", " << vec.y << ')';
	}

	template <typename _T>
	inline std::ostream &operator<<(std::ostream &out, const ig::BaseVector3Template<_T> &vec)
	{
		return out << '(' << vec.x << ", " << vec.y << ", " << vec.z << ')';
	}

	template <typename _T>
	inline std::ostream &operator<<(std::ostream &out, const ig::BaseVector4Template<_T> &vec)
	{
		return out << '(' << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ')';
	}

}
