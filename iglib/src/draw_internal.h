#pragma once
#include "pch.h"
#include "_iglib_vertex.h"
#include "_iglib_shader.h"
#include "_iglib_image.h"

constexpr int MaxTextureWidth = 1 << 14;
constexpr int MaxTextureHeight = 1 << 14;
constexpr int MaxTextureArea = 1 << 28;

#include "default_bitmap.inl"

constexpr FORCEINLINE int to_glprimitve(const PrimitiveType type)
{
  switch (type)
  {
  case ig::PrimitiveType::Point:
		return GL_POINTS;
  case ig::PrimitiveType::Triangle:
    return GL_TRIANGLES;
  case ig::PrimitiveType::Line:
    return GL_LINES;
  case ig::PrimitiveType::LineStript:
    return GL_LINE_STRIP;
  case ig::PrimitiveType::TriangleStrip:
  case ig::PrimitiveType::QuadStrip:
    return GL_TRIANGLE_STRIP;
  case ig::PrimitiveType::Quad:
  case ig::PrimitiveType::TriangleFan:
    return GL_TRIANGLE_FAN;
  default:
		raise(format("invalid draw type value {}", (int)type));
  }
}

constexpr FORCEINLINE int to_glinternal_format(const ColorFormat type)
{
  switch (type)
  {
  case ig::ColorFormat::Invalid:
    return NULL;
  case ig::ColorFormat::L:
    return GL_RED;
  case ig::ColorFormat::LA:
    return GL_RG;
  case ig::ColorFormat::RGB:
    return GL_RGB;
  case ig::ColorFormat::RGBA:
    return GL_RGBA;
  default:
		raise(format("invalid draw type value {}", (int)type));
  }
}


constexpr FORCEINLINE int to_glpixel_format(const ColorFormat type)
{
  switch (type)
  {
  case ig::ColorFormat::Invalid:
    return NULL;
  case ig::ColorFormat::L:
    return GL_RED;
  case ig::ColorFormat::LA:
    return GL_RG;
  case ig::ColorFormat::RGB:
    return GL_RGB;
  case ig::ColorFormat::RGBA:
    return GL_RGBA;
  default:
		raise(format("invalid draw type value {}", (int)type));
  }
}

FORCEINLINE constexpr int get_colorformat_size(const ColorFormat format)
{
	constexpr int ColorFormatSizeLookup[]
	{
		-1,
		1,
		2,
		3,
		4
	};
	return ColorFormatSizeLookup[ int(format) ];
}

FORCEINLINE constexpr bool is_colorformat_transparent(const ColorFormat format)
{
	constexpr bool ColorFormatTrLookup[]
	{
		false, false, true, false, true
	};
	return ColorFormatTrLookup[ int(format) ];
}

constexpr FORCEINLINE int to_gldrawusage(const BufferUsage usage)
{
	constexpr int LookupTable[]
	{
		GL_STATIC_DRAW,
		GL_DYNAMIC_DRAW,
		GL_STREAM_DRAW
	};
	return LookupTable[ (int)usage ];
}

FORCEINLINE void flip_v(unsigned char *data, const size_t ww, const size_t hh, const size_t ch_count)
{
	ASSERT( ch_count <= 4 && ch_count > 0 );
	ASSERT( ww * hh > 0 );
	NOTNULL( data );
	const size_t halfwidth = ww >> 1;

	for (size_t x{}; x < halfwidth; x++)
	{
		for (size_t y{}; y < hh; y++)
		{
			// first pixel
			const size_t index1 = (x + (y * ww)) * ch_count;
			// second pixel
			const size_t index2 = ((ww - (x + 1)) + (y * ww)) * ch_count;

			// copy pixel data
			for (size_t i{}; i < ch_count; i++)
			{
				const auto temp = data[ index1 + i ];
				data[ index1 + i ] = data[ index2 + i ];
				data[ index2 + i ] = temp;
			}
		}
	}
}

FORCEINLINE void flip_h(unsigned char *data, const size_t ww, const size_t hh, const size_t ch_count)
{
	ASSERT( ch_count <= 4 && ch_count > 0 );
	ASSERT( ww * hh > 0 );
	NOTNULL( data );
	const size_t halfheight = hh >> 1;


	for (size_t y{}; y < halfheight; y++)
	{
		for (size_t x{}; x < ww; x++)
		{
			// first pixel
			const size_t index1 = (x + (y * ww)) * ch_count;
			// second pixel
			const size_t index2 = (x + ((hh - (y + 1)) * ww)) * ch_count;

			// copy pixel data
			for (size_t i{}; i < ch_count; i++)
			{
				const auto temp = data[ index1 + i ];
				data[ index1 + i ] = data[ index2 + i ];
				data[ index2 + i ] = temp;
			}
		}
	}
}

FORCEINLINE void use_shader(const Shader &shader)
{
	if (!shader.is_valid() || shader._is_current())
		return;
	glUseProgram(shader.get_id());
}

