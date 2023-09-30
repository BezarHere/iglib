#pragma once
#include "_iglib_vector.h"
#include "_iglib_color.h"

namespace ig
{
	struct PolygonStructure
	{
		inline bool has_area() const
		{
			return data.size() >= 3;
		}

		inline bool is_winding_clockwise() const
		{
			float_t a{};
			const size_t sz = data.size();

			if (sz < 3)
				return true;

			for (size_t i{}; i < sz - 1; i++)
			{
				const Vector2f &v1{ data[ i ] };
				const Vector2f &v2{ data[ i + 1 ] };
				a += (v1.x - v2.x) * (v1.y + v2.y);
			}
			return a > -(data[ sz - 1 ].x - data[ 0 ].x) * (data[ sz - 1 ].y + data[ 0 ].y);
		}

		inline bool is_winding_counter_clockwise() const
		{
			if (data.size() < 3)
				return true;
			return !is_winding_clockwise();
		}

		inline std::shared_ptr<Vector2f[]> to_traingle_fans(_Out_ size_t &count) const
		{
			const size_t sz = data.size();
			
			// no area
			if (sz < 3)
			{
				count = 0;
				return { nullptr };
			}
			count = sz - 2;

			Vector2f *v = new Vector2f[ count * 3 ];
			
			for (size_t i{ 1u }; i < sz - 1; i++)
			{
				const size_t j = (i - 1) * 3;

				v[ j ] = data[ i ];
				v[ j + 1 ] = data[ i + 1 ];
				v[ j + 2 ] = data[ 0 ];
			}

			return std::shared_ptr<Vector2f[]>(v);
		}

		std::vector<Vector2f> data;
	};

	class Polygon : public PolygonStructure
	{


	};

}
