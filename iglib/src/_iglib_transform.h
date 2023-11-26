#pragma once
#include "_iglib_base.h"
#include "_iglib_vector.h"

#pragma warning(push)
#pragma warning(disable: 4201)
namespace ig
{
	// TODO: add equal aprox functions for floating-point errors

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

	// a fancy 3x3 matrix
	struct Basis
	{

		inline constexpr Basis()
		{
		}

		inline constexpr Basis(const Vector3f xx, const Vector3f yy, const Vector3f zz)
			: xdir{ xx }, ydir{ yy }, zdir{ zz }
		{
		}

		inline Basis(const Vector3f euler)
		{
			set_angle(euler);
		}

		inline constexpr Basis(const float xx, const float xy, const float xz,
					                 const float yx, const float yy, const float yz,
					                 const float zx, const float zy, const float zz )
			: xdir{ xx, xy, xz }, ydir{ yx, yy, yz }, zdir{ zx, zy, zz }
		{
		}

		inline constexpr void transpose() noexcept
		{
			const float xy = xdir.y, xz = xdir.z, yz = ydir.z;
			xdir.y = ydir.x;
			xdir.z = zdir.x;
			ydir.z = zdir.y;
			ydir.x = xy;
			zdir.x = xz;
			zdir.y = yz;
		}

		inline constexpr Basis transposed() const noexcept
		{
			Basis copy{ *this };
			copy.transpose();
			return copy;
		}

		inline constexpr float determinant() const
		{
			return (xdir.x * (ydir.y * zdir.z - zdir.y * ydir.z)) - (ydir.x * (xdir.y * zdir.z - zdir.y * xdir.z)) + (zdir.x * (xdir.y * ydir.z - ydir.y * xdir.z));
		}

		inline constexpr bool is_orthogonal() const
		{
			constexpr Basis identity{};
			return this->operator*(transposed()) == identity;
		}

		inline constexpr bool is_rotation() const
		{
			return determinant() == 1 && is_orthogonal();
		}

		/// !MAJOR FAULTY!
		inline Vector3f get_rotation() const
		{
			Vector3f euler;

			float m12 = ydir.z;

			if (m12 < (1 - Epsilon)) {
				if (m12 > -(1 - Epsilon)) {
					// 1 > m12 > -1 (e.g. not looking directly up or down)

					// is this a pure X rotation?
					if (ydir.x == 0 && xdir.y == 0 && xdir.z == 0 && ydir.x == 0 && xdir.x == 1) {
						// return the simplest form (human friendlier in editor and scripts)
						
						euler.x = atan2(-m12, ydir.y);
						euler.y = 0;
						euler.z = 0;
					}
					else {
						euler.x = asin(-m12);
						//euler.y = atan2(xdir.z, ydir.z);
						euler.y = atan2(zdir.x, zdir.z);
						euler.z = atan2(ydir.x, ydir.y);
					}
				}
				else { // m12 == -1
					euler.x = Pi * 0.5;
					euler.y = atan2(xdir.y, xdir.x);
					euler.z = 0;
				}
			}
			else { // m12 == 1
				euler.x = -Pi * 0.5;
				euler.y = -atan2(xdir.y, xdir.x);
				euler.z = 0;
			}


			return euler;
		}

		inline void set_angle(const Vector3f euler)
		{
			float c, s;

			c = std::cos(euler.x);
			s = std::sin(euler.x);
			Basis xmat(1, 0, 0, 0, c, -s, 0, s, c);

			c = std::cos(euler.y);
			s = std::sin(euler.y);
			Basis ymat(c, 0, s, 0, 1, 0, -s, 0, c);

			c = std::cos(euler.z);
			s = std::sin(euler.z);
			Basis zmat(c, -s, 0, s, c, 0, 0, 0, 1);

			//optimizer will optimize away all this anyway
			*this = ymat * xmat * zmat;
			//std::cout << get_rotation() << '\n';
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

		inline void rotate(const Vector3f p_euler)
		{
			(*this) = Basis(p_euler) * (*this);
		}

		inline constexpr bool operator==(const Basis &other) const noexcept
		{
			return xdir == other.xdir && ydir == other.ydir && zdir == other.zdir;
		}

		inline constexpr bool operator!=(const Basis &other) const noexcept
		{
			return xdir != other.xdir || ydir != other.ydir || zdir != other.zdir;
		}

		inline constexpr Basis operator*(const Basis &other) const noexcept
		{
			return { other.xdir.dot(xdir), other.ydir.dot(xdir), other.zdir.dot(xdir),
							 other.xdir.dot(ydir), other.ydir.dot(ydir), other.zdir.dot(ydir),
							 other.xdir.dot(zdir), other.ydir.dot(zdir), other.zdir.dot(zdir) };
			
		}

		union
		{
			struct
			{
				Vector3f xdir;
				Vector3f ydir;
				Vector3f zdir;
			};
			Vector3f v[ 3 ]{ { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f } };
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

		inline constexpr Vector3f get_back_dir() const noexcept
		{
			return m_elements.zdir;
		}

		inline constexpr Vector3f get_down_dir() const noexcept
		{
			return m_elements.ydir;
		}

		inline constexpr Vector3f get_left_dir() const noexcept
		{
			return m_elements.xdir;
		}

		inline constexpr Vector3f operator*(const Vector3f &other) const
		{
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

		inline Basis &get_basis()
		{
			return m_elements.basis;
		}

		inline const Basis &get_basis() const
		{
			return m_elements.basis;
		}


	private:
		transform_element4x3 m_elements;
	};

}
#pragma warning(pop)
