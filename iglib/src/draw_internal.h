#pragma once
#include "pch.h"
#include "_iglib_vertex.h"
#include "_iglib_shader.h"

FORCEINLINE void glColor3(const Colorb clr)
{
  glColor3ub(clr.r, clr.g, clr.b);
}

FORCEINLINE void glTexCoord2f(const Vector2f v)
{
  glTexCoord2f(v.x, v.y);
}

FORCEINLINE void glVertex2i(const Vector2i v)
{
  glVertex2i(v.x, v.y);
}

FORCEINLINE void glVertex2f(const Vector2f f)
{
  glVertex2f(f.x, f.y);
}

constexpr FORCEINLINE int to_gldraw_type(const VertexDrawType type)
{
  switch (type)
  {
  case ig::VertexDrawType::Quad:
    return GL_QUADS;
  case ig::VertexDrawType::Triangle:
    return GL_TRIANGLES;
  case ig::VertexDrawType::Line:
    return GL_LINES;
  case ig::VertexDrawType::LineStript:
    return GL_LINE_STRIP;
  case ig::VertexDrawType::TriangleStrip:
    return GL_TRIANGLE_STRIP;
  case ig::VertexDrawType::TriangleFan:
    return GL_TRIANGLE_FAN;
  case ig::VertexDrawType::QuadStrip:
    return GL_QUAD_STRIP;
  default:
		raise(format("invalid draw type value {}", (int)type));
		return -1;
  }
}

constexpr FORCEINLINE int to_glshader_type(const SubshaderType type)
{
  switch (type)
  {
	case SubshaderType::Vertex:
    return GL_VERTEX_SHADER;
	case SubshaderType::Fragment:
    return GL_FRAGMENT_SHADER;
  default:
		raise(format("invalid shader type value {}", (int)type));
		return -1;
  }
}

FORCEINLINE void glVertex(const Vertex &_Vert)
{
  glColor3(_Vert.clr);
  glTexCoord2f(_Vert.tex_coord);
  glVertex2i(_Vert.pos);
}

FORCEINLINE void flip_v(unsigned char *data, const size_t ww, const size_t hh, const size_t ch_count)
{
	assert(ch_count <= 4 && ch_count > 0);
	assert(ww * hh > 0);
	NOTNULL(data);
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

FORCEINLINE void flip_h(unsigned char *data, const size_t ww, const size_t hh, const size_t ch_count)
{
	assert(ch_count <= 4 && ch_count > 0);
	assert(ww * hh > 0);
	NOTNULL(data);
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

FORCEINLINE void use_shader(const Shader &shader)
{
	if (!shader.is_valid())
		warn("Shader with an error code used: " + std::to_string(shader.get_log().code) + " with massege \"" + shader.get_log().msg + "\"");
	if (shader._is_current())
		return;
	glUseProgram(shader.get_id());
}

