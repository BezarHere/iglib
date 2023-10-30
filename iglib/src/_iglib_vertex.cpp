#include "pch.h"
#include "_iglib_vertex.h"
#include "draw_internal.h"

FORCEINLINE VertexBufferId_t create_buffer()
{
	VertexBufferId_t i;
	glGenBuffers(1, &i);
	return i;
}

FORCEINLINE void free_buffer( VertexBufferId_t &id)
{
	glDeleteBuffers(1, &id);
	id = 0;
}

namespace ig
{

	BaseVertexBuffer::BaseVertexBuffer(VertexBufferId_t id, size_t size, BufferUsage usage, PrimitiveType type)
		: m_id{ id }, m_size{ size }, m_usage{ usage }, m_type{ type }
	{
	}

	size_t BaseVertexBuffer::get_size() const noexcept
	{
		return m_size;
	}

	BufferUsage BaseVertexBuffer::get_usage() const noexcept
	{
		return m_usage;
	}

	PrimitiveType BaseVertexBuffer::get_primitive() const noexcept
	{
		return m_type;
	}

	void BaseVertexBuffer::set_primitive(PrimitiveType prim)
	{
		m_type = prim;
	}

	VertexBufferId_t BaseVertexBuffer::get_id() const noexcept
	{
		return m_id;
	}

	void BaseVertexBuffer::_bind_array_buffer() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	bool BaseVertexBuffer::_unbind_array_buffer() const
	{
		int current_ab;
		glGetIntegerv(GL_ARRAY_BUFFER, &current_ab);
		//if (current_ab == m_id)
			glBindBuffer(GL_ARRAY_BUFFER, NULL);
		return true;
	}
	
	Vertex2DBuffer::Vertex2DBuffer()
		: BaseVertexBuffer(create_buffer())
	{
	}

	Vertex2DBuffer::Vertex2DBuffer(size_t size)
		: BaseVertexBuffer(create_buffer())
	{
		create(size);
	}

	Vertex2DBuffer::~Vertex2DBuffer()
	{
		if (m_id)
			free_buffer(m_id);
	}

	void Vertex2DBuffer::create(const size_t size, const vertex_type *vertices)
	{
		if (m_id)
			free_buffer(m_id);

		m_size = size;
		m_id = create_buffer();
		_bind_array_buffer();
		
		if (vertices)
			glBufferData(GL_ARRAY_BUFFER, size * sizeof(vertex_type), vertices, to_gldrawusage(m_usage));

		if (!_unbind_array_buffer())
			raise(format("POSSIBLE RACE COND: at 'create': last bound vertex buffer 2d (id {}) was unbounded mid process", m_id));
	}

	void Vertex2DBuffer::update(const vertex_type *vertcies, const size_t vertices_count, const uint32_t offset)
	{
		_bind_array_buffer();

		glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(vertex_type), vertices_count * sizeof(vertex_type), vertcies);

		if (!_unbind_array_buffer())
			raise(format("POSSIBLE RACE COND: at 'update': last bound vertex buffer 2d (id {}) was unbounded mid process", m_id));
	}

	void Vertex2DBuffer::update(const vertex_type *vertcies)
	{
		update(vertcies, get_size(), 0);
	}

	Vertex3DBuffer::Vertex3DBuffer()
		: BaseVertexBuffer(create_buffer())
	{
	}

	Vertex3DBuffer::~Vertex3DBuffer()
	{
	}
}
