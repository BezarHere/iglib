#pragma once
#include "_iglib_base.h"
#include "_iglib_vector.h"

#pragma warning(push)
#pragma warning(disable: 4201)
namespace ig
{

	class Transform2D
	{
	public:
		union transform_element3x2
		{
			Vector2f v[ 3 ]{ Vector2f(1.0f, 0.0f), Vector2f(0.0f, 1.0f), Vector2f(0.0f, 0.0f) };
			float_t f[ 6 ];
			struct
			{
				Vector2f xdir;
				Vector2f ydir;
				Vector2f origin;
			};
		};
		static_assert(sizeof(transform_element3x2) == sizeof(float_t) * 6, "Invalid transform elements size");



		inline constexpr Transform2D()
		{}

		inline constexpr Transform2D(Vector2f xdir, Vector2f ydir, Vector2f origin = Vector2f())
			: m_elements{ xdir, ydir, origin }
		{
		}

		inline constexpr Transform2D(const Transform2D &copy)
			: m_elements{ copy.m_elements }
		{
		}
		
		inline static Transform2D from_coords(const Vector2f &position, const float rotation, const Vector2f &scale)
		{
			const Vector2f v = Vector2f{ std::cos(rotation), std::sin(rotation) };
			return Transform2D(v * scale.x, Vector2f(-v.y, v.x) * scale.y, position);
		}

		inline float_t basis_determinant() const
		{
			return m_elements.xdir.x * m_elements.ydir.y - m_elements.xdir.y * m_elements.ydir.x;
		}

		inline float_t get_rotation() const
		{
			return m_elements.xdir.angle();
		}

		inline Vector2f get_position() const noexcept
		{
			return m_elements.origin;
		}

		inline Vector2f get_scale() const noexcept
		{
			return { m_elements.xdir.length(), basis_determinant() > 0 ? m_elements.ydir.length() : -m_elements.ydir.length() };
		}

		inline void set_position(Vector2f pos)
		{
			m_elements.origin = pos;
		}

		inline void set_scale(Vector2f scale)
		{
			m_elements.xdir = m_elements.xdir.normalized() * scale.x;
			m_elements.ydir = m_elements.ydir.normalized() * scale.y;
		}

		inline void set_rotation(float_t radians)
		{
			const Vector2f scale = get_scale();
			const float cosv = std::cos(radians);
			const float  sinv = std::sin(radians);
			m_elements.f[ 0 ] = cosv;
			m_elements.f[ 1 ] = sinv;
			m_elements.f[ 2 ] = -sinv;
			m_elements.f[ 3 ] = cosv;
			set_scale(scale);
		}

		inline void rotate(float_t radians)
		{
			set_rotation(radians + get_rotation());
		}

		inline void invert()
		{
				const float_t idet = 1.0f / basis_determinant();

				swapref(m_elements.v[ 0 ].x, m_elements.v[ 1 ].y);
				m_elements.v[ 0 ] *= Vector2f(idet, -idet);
				m_elements.v[ 1 ] *= Vector2f(-idet, idet);

				m_elements.v[ 2 ] = this->operator*(-m_elements.v[ 2 ]);
		}

		inline Transform2D &&inverse() const
		{
			Transform2D temp{ *this };
			temp.invert();
			return std::move(temp);
		}

		inline constexpr Vector2f operator*(const Vector2f &other) const
		{
			// did take 'show your steps or i will not grade it' very literally
			//return (m_elements.xdir * other.x) + (m_elements.ydir * other.y) + m_elements.origin;
			//return Vector2f(m_elements.xdir.x * other.x, m_elements.xdir.y * other.x) + Vector2f(m_elements.ydir.x * other.y, m_elements.ydir.y * other.y) + m_elements.origin;
			//return Vector2f((m_elements.xdir.x * other.x) + (m_elements.ydir.x * other.y), (m_elements.xdir.y * other.y) + (m_elements.ydir.y * other.y)) + m_elements.origin;
			return Vector2f((m_elements.xdir.x * other.x) + (m_elements.ydir.x * other.y) + m_elements.origin.x, (m_elements.xdir.y * other.x) + (m_elements.ydir.y * other.y) + m_elements.origin.y);
		}
	
		// returns a transform that when it's applied to a point returns the same as applying the two original transforms
		// t1 * (t2 * v) = (t1 * t2) * v
		inline constexpr Transform2D operator*(const Transform2D &other) const
		{
			return Transform2D(this->operator*(other.m_elements.xdir), this->operator*(other.m_elements.ydir), this->operator*(other.m_elements.origin));
		}

		inline const transform_element3x2 &get_data()
		{
			return m_elements;
		}


	private:
		transform_element3x2 m_elements;
	};

	struct Quaternion
	{
		


		float x, y, z, w;
	};

	struct Basis
	{


		inline float determinant() const
		{
			return (xdir.x * (ydir.y * zdir.z - zdir.y * ydir.z)) - (ydir.x * (xdir.y * zdir.z - zdir.y * xdir.z)) + (zdir.x * (xdir.y * ydir.z - ydir.y * xdir.z));
		}


		inline void set_rotation(Vector3f axis, float angle)
		{
			const Vector3f axis_sq = axis.squared();
			const float cosine = std::cos(angle);
			xdir.x = axis_sq.x + cosine * (1 - axis_sq.x);
			ydir.y = axis_sq.y + cosine * (1 - axis_sq.y);
			zdir.z = axis_sq.z + cosine * (1 - axis_sq.z);

			const float sine = std::sin(angle);
			const float t = 1 - cosine;

			float xyzt = axis.x * axis.y * t;
			float zyxs = axis.z * sine;
			xdir.y = xyzt - zyxs;
			ydir.x = xyzt + zyxs;

			xyzt = axis.x * axis.z * t;
			zyxs = axis.y * sine;
			xdir.z = xyzt + zyxs;
			zdir.x = xyzt - zyxs;

			xyzt = axis.y * axis.z * t;
			zyxs = axis.x * sine;
			ydir.z = xyzt - zyxs;
			zdir.y = xyzt + zyxs;
		}

		union
		{
			Vector3f v[ 3 ];
			struct
			{
				Vector3f xdir;
				Vector3f ydir;
				Vector3f zdir;
			};
		};
	};

	class Transform3D
	{
	public:
		union transform_element4x3
		{
			Vector3f v[ 4 ]{ Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 0.0f, 0.0f) };
			float_t f[ 12 ];
			struct
			{
				union
				{
					Basis basis;
					struct
					{
						Vector3f xdir;
						Vector3f ydir;
						Vector3f zdir;
					};
				};
				Vector3f origin;
			};
		};
		static_assert(sizeof(transform_element4x3) == sizeof(float_t) * 12, "Invalid transform elements size");



		inline constexpr Transform3D()
			: m_elements{ Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 0.0f, 0.0f) }
		{}

		inline constexpr Transform3D(Vector3f xdir, Vector3f ydir, Vector3f zdir, Vector3f origin = Vector3f())
			: m_elements{ xdir, ydir, zdir, origin }
		{
		}

		inline constexpr Transform3D(const Transform3D &copy)
			: m_elements{ copy.m_elements }
		{
		}

		inline float_t basis_determinant() const
		{
			return m_elements.basis.determinant();
		}

		inline Vector3f get_position() const noexcept
		{
			return m_elements.origin;
		}

		inline Vector3f get_scale() const noexcept
		{
			return { m_elements.xdir.length(), basis_determinant() > 0 ? m_elements.ydir.length() : -m_elements.ydir.length(), m_elements.zdir.length() };
		}

		inline void set_position(Vector3f pos)
		{
			m_elements.origin = pos;
		}

		inline void set_scale(Vector3f scale)
		{
			m_elements.xdir = m_elements.xdir.normalized() * scale.x;
			m_elements.ydir = m_elements.ydir.normalized() * scale.y;
			m_elements.zdir = m_elements.zdir.normalized() * scale.z;
		}

		/*inline void set_rotation(float_t radians)
		{
			const Vector2f scale = get_scale();
			const float cosv = std::cos(radians);
			const float  sinv = std::sin(radians);
			m_elements.f[ 0 ] = cosv;
			m_elements.f[ 1 ] = sinv;
			m_elements.f[ 2 ] = -sinv;
			m_elements.f[ 3 ] = cosv;
			set_scale(scale);
		}*/

		/*inline void rotate(float_t radians)
		{
			set_rotation(radians + get_rotation());
		}*/

		/*inline void invert()
		{
			const float_t idet = 1.0f / basis_determinant();

			swapref(m_elements.v[ 0 ].x, m_elements.v[ 1 ].y);
			m_elements.v[ 0 ] *= Vector2f(idet, -idet);
			m_elements.v[ 1 ] *= Vector2f(-idet, idet);

			m_elements.v[ 2 ] = this->operator*(-m_elements.v[ 2 ]);
		}*/

		/*inline Transform2D &&inverse() const
		{
			Transform2D temp{ *this };
			temp.invert();
			return std::move(temp);
		}*/

		inline constexpr Vector3f operator*(const Vector3f &other) const
		{
			// did take 'show your steps or i will not grade it' very literally
			//return (m_elements.xdir * other.x) + (m_elements.ydir * other.y) + (m_elements.zdir * other.z) + m_elements.origin;
			//return Vector2f((m_elements.xdir.x * other.x) + (m_elements.ydir.x * other.y) + (m_elements.zdir.x * other.z), (m_elements.xdir.y * other.y) + (m_elements.ydir.y * other.y) + (m_elements.zdir.x * other.z)) + m_elements.origin;
			return Vector3f(
				(m_elements.xdir.x * other.x) + (m_elements.ydir.x * other.y) + (m_elements.zdir.x * other.z) + m_elements.origin.x,
				(m_elements.xdir.y * other.x) + (m_elements.ydir.y * other.y) + (m_elements.zdir.y * other.z) + m_elements.origin.y,
				(m_elements.xdir.z * other.x) + (m_elements.ydir.z * other.y) + (m_elements.zdir.z * other.z) + m_elements.origin.z
			);
		}

		// returns a transform that when it's applied to a point returns the same as applying the two original transforms
		// t1 * (t2 * v) = (t1 * t2) * v
		inline constexpr Transform3D operator*(const Transform3D &other) const
		{
			return Transform3D(this->operator*(other.m_elements.xdir), this->operator*(other.m_elements.ydir), this->operator*(other.m_elements.zdir), this->operator*(other.m_elements.origin));
		}

		inline const transform_element4x3 &get_data()
		{
			return m_elements;
		}


	private:
		transform_element4x3 m_elements;
	};

}
#pragma warning(pop)
