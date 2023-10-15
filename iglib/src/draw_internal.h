#pragma once
#include "pch.h"
#include "_iglib_vertex.h"

FORCEINLINE void glColor3b(const Colorb clr)
{
  glColor3b(clr.r, clr.g, clr.b);
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

constexpr FORCEINLINE int to_gldraw_v(const VertexDrawType type)
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
    break;
  }
  raise(format("invalid draw type value {}", (int)type));
  return -1;
}

FORCEINLINE void glVertex(const Vertex &_Vert)
{
  glColor3b(_Vert.clr);
  glTexCoord2f(_Vert.tex_coord);
  glVertex2i(_Vert.pos);
}
