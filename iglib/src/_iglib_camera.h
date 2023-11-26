#pragma once
#include "_iglib_transform.h"
#include "_iglib_matrix.h"

namespace ig
{
	struct Camera
	{
		inline Matrix4x4 projection(const float aspect_ratio) const
		{
			constexpr float HalfDag2RadFactor = Pi / 90.f;
			const float d1 = 1.f / std::tan( fov * HalfDag2RadFactor );
			const float d2 = d1 / aspect_ratio;
			const float zrange = znear - zfar;

			const float A = -(zfar + znear) / zrange;
			const float B = 2.f * zfar * znear / zrange;

			return { {d2,  	0.f,	0.f,  0.f,
								0.f,	d1,  	0.f,  0.f,
								0.f,	0.f,	  A,    B,
								0.f,  0.f,  1.f,  1.f, } };
		}

		Transform3D transform{};
		float fov = 120.f;
		float znear = 0.1f, zfar = 100.f;
	};
}