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

FORCEINLINE VertexBufferId_t duplicate_buffer( const VertexBufferId_t copy, const int usage )
{
	if (!copy)
		return NULL;
	glBindBuffer( GL_ARRAY_BUFFER, copy );
	int bsize;
	glGetBufferParameteriv( GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize );
	uint8_t *buffer = new uint8_t[ bsize ];
	glGetBufferSubData( GL_ARRAY_BUFFER, 0, bsize, buffer );
	glBindBuffer( GL_ARRAY_BUFFER, NULL );

	VertexBufferId_t new_id = create_buffer();
	glBindBuffer( GL_ARRAY_BUFFER, new_id );
	glBufferData( GL_ARRAY_BUFFER, bsize, buffer, usage );
	glBindBuffer( GL_ARRAY_BUFFER, NULL );

	delete[] buffer;
	return new_id;
}

namespace ig
{

	BaseVertexBuffer::BaseVertexBuffer(VertexBufferId_t id, size_t size, BufferUsage usage, PrimitiveType type)
		: m_id{ id }, m_size{ size }, m_usage{ usage }, m_type{ type }
	{
	}

	BaseVertexBuffer::BaseVertexBuffer( const BaseVertexBuffer &copy )
		: m_id{ duplicate_buffer( copy.m_id, to_gldrawusage( copy.m_usage ) ) }, m_size{ copy.m_size }, m_usage{ copy.m_usage }, m_type{ copy.m_type }
	{
		
	}

	BaseVertexBuffer::BaseVertexBuffer( BaseVertexBuffer &&move ) noexcept
		: m_id{ move.m_id }, m_size{ move.m_size }, m_usage{ move.m_usage }, m_type{ move.m_type }
	{
		move.m_id = NULL;
	}

	BaseVertexBuffer &BaseVertexBuffer::operator=( const BaseVertexBuffer &copy )
	{
		m_id = duplicate_buffer( copy.m_id, to_gldrawusage( copy.m_usage ) );
		m_size = copy.m_size;
		m_type = copy.m_type;
		m_usage = copy.m_usage;
		return *this;
	}

	BaseVertexBuffer &BaseVertexBuffer::operator=( BaseVertexBuffer &&move ) noexcept
	{
		m_id = move.m_id;
		m_size = move.m_size;
		m_type = move.m_type;
		m_usage = move.m_usage;
		move.m_id = NULL;
		return *this;
	}

	size_t BaseVertexBuffer::size() const noexcept
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

	void BaseVertexBuffer::set_usage(BufferUsage usage)
	{
		m_usage = usage;
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
		//int current_ab;
		//glGetIntegerv(GL_ARRAY_BUFFER, &current_ab);
		//if (current_ab == m_id)
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		return true;
	}
	
	Vertex2DBuffer::Vertex2DBuffer()
		: BaseVertexBuffer(NULL)
	{
	}

	Vertex2DBuffer::Vertex2DBuffer(size_t size)
		: BaseVertexBuffer(NULL)
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
		
		
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(vertex_type), vertices, to_gldrawusage(m_usage));

		if (!_unbind_array_buffer())
			raise(format("POSSIBLE RACE COND: at 'create': last bound vertex buffer 2d (id {}) was unbounded mid process", m_id));
	}

	void Vertex2DBuffer::update(const vertex_type *vertcies, const size_t vertices_count, const uint32_t offset)
	{
		if (offset + vertices_count > m_size)
			raise("overflowing a vertex buffer 2d");

		_bind_array_buffer();

		glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(vertex_type), vertices_count * sizeof(vertex_type), vertcies);

		if (!_unbind_array_buffer())
			raise(format("POSSIBLE RACE COND: at 'update': last bound vertex buffer 2d (id {}) was unbounded mid process", m_id));
	}

	void Vertex2DBuffer::update(const vertex_type *vertcies)
	{
		update(vertcies, size(), 0);
	}

	Vertex3DBuffer::Vertex3DBuffer()
		: BaseVertexBuffer(NULL)
	{
	}

	Vertex3DBuffer::Vertex3DBuffer(size_t size)
		: BaseVertexBuffer(NULL)
	{
		create(size);
	}

	Vertex3DBuffer::~Vertex3DBuffer()
	{
		if (m_id)
			free_buffer(m_id);
	}

	void Vertex3DBuffer::create(const size_t size, const vertex_type *vertices)
	{
		if (m_id)
			free_buffer(m_id);

		m_size = size;
		m_id = create_buffer();
		_bind_array_buffer();


		glBufferData(GL_ARRAY_BUFFER, size * sizeof(vertex_type), vertices, to_gldrawusage(m_usage));

		if (!_unbind_array_buffer())
			raise(format("POSSIBLE RACE COND: at 'create': last bound vertex buffer 3d (id {}) was unbounded mid process", m_id));
	}

	void Vertex3DBuffer::update(const vertex_type *vertcies, const size_t vertices_count, const uint32_t offset)
	{
		if (offset + vertices_count > m_size)
			raise("overflowing a vertex buffer 2d");

		_bind_array_buffer();

		glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(vertex_type), vertices_count * sizeof(vertex_type), vertcies);

		if (!_unbind_array_buffer())
			raise(format("POSSIBLE RACE COND: at 'update': last bound vertex buffer 3d (id {}) was unbounded mid process", m_id));
	}

	void Vertex3DBuffer::update(const vertex_type *vertcies)
	{
		update(vertcies, size(), 0);
	}

}
