#include "pch.h"
#include "_iglib_indexbuffer.h"
#include "draw_internal.h"

namespace ig
{
  IndexBuffer::IndexBuffer()
    : m_buf{ new _buffer{} }, m_size{0}, m_usage{ BufferUsage::Static }
  {
  }

  IndexBuffer::IndexBuffer(size_t size, const index_type *indcies)
    : IndexBuffer()
  {
    generate(size, indcies);
  }

  size_t IndexBuffer::size() const
  {
    return m_size;
  }

  uint32_t IndexBuffer::get_id() const
  {
    return m_buf->id;
  }

  void IndexBuffer::generate(size_t size, const index_type *indcies)
  {
    m_buf.reset(new _buffer());
    m_size = size;

    glGenBuffers(1, &m_buf->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buf->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(index_type), indcies, to_gldrawusage(m_usage));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  void IndexBuffer::update(const index_type *indcies, const size_t indcies_count, const uint32_t offset)
  {
    if (!m_buf->id)
      return;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buf->id);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(index_type), indcies_count * sizeof(index_type), indcies);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  void IndexBuffer::update(const index_type *indcies)
  {
    update(indcies, m_size, 0);
  }

  BufferUsage IndexBuffer::get_usage() const
  {
    return m_usage;
  }

  void IndexBuffer::set_usage(BufferUsage usage)
  {
    m_usage = usage;
  }

  IndexBuffer::_buffer::~_buffer()
  {
    if (id)
    {
      glDeleteBuffers(1, &id);
    }
  }
}
