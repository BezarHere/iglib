#pragma once
#include "_iglib_transform.h"
#include "_iglib_matrix.h"

namespace ig
{
	struct Camera
	{

		inline Matrix4x4 projection() const
		{
			constexpr float HalfDag2RadFactor = Pi / 90.f;
			const float d = 1.f / std::tan(fov * HalfDag2RadFactor);
			return { {d,  	0.f,	0.f,  0.f,
								0.f,	d,  	0.f,  0.f,
								0.f,	0.f,	1.f,  0.f,
								0.f,  0.f,  1.f,  1.f } };
		}

		Transform3D transform{};
		float fov = 120.f;
	};
}
