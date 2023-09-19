#pragma once
#include "_iglib_base.h"
#include "_iglib_vector.h"

namespace ig
{

	class Transform2D
	{
	private:
		union transform_element3x3
		{
			Vector2f v[ 3 ]{ Vector2f(1.0f, 0.0f), Vector2f(0.0f, 1.0f), Vector2f(0.0f, 0.0f) };
			float_t f[ 6 ];
			struct
			{
				Vector2f xdir;
				Vector2f ydir;
				Vector2f origin;
			};
			byte b[ 24 ];
		};

	public:
		static_assert(sizeof(transform_element3x3) == sizeof(float_t) * 6, "Invalid transform elements size");


		inline constexpr Transform2D()
		{}

		inline constexpr Transform2D(Vector2f xdir, Vector2f ydir, Vector2f origin = Vector2f())
			: m_elements{ xdir, ydir, origin }
		{
		}

		inline float_t basis_determinant() const
		{
			return m_elements.xdir.x * m_elements.ydir.y - m_elements.xdir.y * m_elements.ydir.x;
		}

		inline float_t get_rotation() const
		{
			return m_elements.v[ 0 ].angle();
		}

		inline void set_rotation(float_t radians)
		{
			const float_t dif = radians - get_rotation();
			m_elements.xdir = m_elements.xdir.rotated(dif);
			m_elements.ydir = m_elements.ydir.rotated(dif);
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
		/// FIXME
		inline constexpr Transform2D operator*(const Transform2D &other) const
		{
			return Transform2D(this->operator*(other.m_elements.xdir), this->operator*(other.m_elements.ydir), this->operator*(other.m_elements.origin));
		}

	private:
		transform_element3x3 m_elements;
	};

}
