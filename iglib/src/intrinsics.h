#pragma once
#include <intrin.h>

inline constexpr __m512 m512_s(const float vf)
{
	return { vf, vf,vf, vf, vf, vf, vf, vf, vf, vf,vf, vf, vf, vf, vf, vf };
}

inline constexpr __m256 m256_s(const float vf)
{
	return { vf, vf,vf, vf, vf, vf, vf, vf };
}

inline constexpr __m128 m128_s(const float vf)
{
	return { vf, vf,vf, vf };
}

constexpr __m512 zero512 = m512_s(0.0f);
constexpr __m512 one512 = m512_s(1.0f);
constexpr __m512 two512 = m512_s(2.0f);

constexpr __m256 zero256 = m256_s(0.0f);
constexpr __m256 one256 = m256_s(1.0f);
constexpr __m256 two256 = m256_s(2.0f);

constexpr __m128 zero128 = m128_s(0.0f);
constexpr __m128 one128 = m128_s(1.0f);
constexpr __m128 two128 = m128_s(2.0f);


