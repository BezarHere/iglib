#pragma once
#include "_iglib_transform.h"
#include "_iglib_matrix.h"

namespace ig
{
	struct Camera
	{

		Matrix4x4 projection() const
		{
			constexpr float HalfDag2RadFactor = Pi / 90.f;
			const float d = 1.f / std::tan(fov * HalfDag2RadFactor);
			return { {	d,		0.f,	0.f,	0.f,
								0.f,	d,		0.f,	0.f,
								0.f,	0.f,	d,		0.f,
								0.f,	0.f,	0.f,	0.f, } };
		}

		Transform2D transform2d;
		Transform3D transform3d;
		float fov = 120.f;
	};
}
