#include "pch.h"
#include "_iglib_vertex.h"
#include "draw_internal.h"

// template instancing:
template class BaseVertexBuffer<Vertex2>;
template class BaseVertexBuffer<Vertex3>;


FORCEINLINE VertexBufferId_t create_buffer()
{
	VertexBufferId_t i = 0;
	glGenBuffers(1, &i);
	if (!i)
	{
		bite::warn("Warning: Creating vertex buffer failed with error code " + std::to_string(glGetError()));
	}
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

	template <typename _VRT>
	BaseVertexBuffer<_VRT>::BaseVertexBuffer(VertexBufferId_t id, size_t size, BufferUsage usage, PrimitiveType type)
		: m_id{ id }, m_size{ size }, m_usage{ usage }, m_type{ type }
	{
	}

	template<typename _VRT>
	BaseVertexBuffer<_VRT>::BaseVertexBuffer()
		: m_id{ 0 }, m_size{ 0 }, m_usage{ BufferUsage::Static }, m_type{ PrimitiveType::TriangleFan } {
	}

	template<typename _VRT>
	BaseVertexBuffer<_VRT>::BaseVertexBuffer( size_t size ) : BaseVertexBuffer() {
		create( size );
	}

	template<typename _VRT>
	BaseVertexBuffer<_VRT>::BaseVertexBuffer( PrimitiveType type, size_t size, BufferUsage usage )
		: m_id{ 0 }, m_type{ type }, m_usage{ usage }, m_size{ size } {
		if (size)
			create( size );
	}

	template<typename _VRT>
	BaseVertexBuffer<_VRT>::~BaseVertexBuffer() noexcept {
		if (m_id)
			free_buffer( m_id );
	}

	template <typename _VRT>
	BaseVertexBuffer<_VRT>::BaseVertexBuffer( const BaseVertexBuffer &copy )
		: m_id{ duplicate_buffer( copy.m_id, to_gldrawusage( copy.m_usage ) ) }, m_size{ copy.m_size }, m_usage{ copy.m_usage }, m_type{ copy.m_type }
	{
		
	}

	template <typename _VRT>
	BaseVertexBuffer<_VRT>::BaseVertexBuffer( BaseVertexBuffer &&move ) noexcept
		: m_id{ move.m_id }, m_size{ move.m_size }, m_usage{ move.m_usage }, m_type{ move.m_type }
	{
		move.m_id = NULL;
	}

	template <typename _VRT>
	BaseVertexBuffer<_VRT> &BaseVertexBuffer<_VRT>::operator=( const BaseVertexBuffer &copy )
	{
		m_id = duplicate_buffer( copy.m_id, to_gldrawusage( copy.m_usage ) );
		m_size = copy.m_size;
		m_type = copy.m_type;
		m_usage = copy.m_usage;
		return *this;
	}

	template <typename _VRT>
	BaseVertexBuffer<_VRT> &BaseVertexBuffer<_VRT>::operator=( BaseVertexBuffer &&move ) noexcept
	{
		m_id = move.m_id;
		m_size = move.m_size;
		m_type = move.m_type;
		m_usage = move.m_usage;
		move.m_id = NULL;
		return *this;
	}


	template <typename _VRT>
	size_t BaseVertexBuffer<_VRT>::size() const noexcept
	{
		return m_size;
	}

	template <typename _VRT>
	BufferUsage BaseVertexBuffer<_VRT>::get_usage() const noexcept
	{
		return m_usage;
	}

	template <typename _VRT>
	PrimitiveType BaseVertexBuffer<_VRT>::get_primitive() const noexcept
	{
		return m_type;
	}

	template <typename _VRT>
	void BaseVertexBuffer<_VRT>::set_primitive(PrimitiveType prim)
	{
		m_type = prim;
	}

	template <typename _VRT>
	void BaseVertexBuffer<_VRT>::set_usage(BufferUsage usage)
	{
		m_usage = usage;
	}

	template<typename _VRT>
	void BaseVertexBuffer<_VRT>::create( const size_t size, const vertex_type *vertices ) {
		if (m_id)
			free_buffer( m_id );

		m_size = size;
		m_id = create_buffer();
		_bind_array_buffer();


		glBufferData( GL_ARRAY_BUFFER, size * sizeof( vertex_type ), vertices, to_gldrawusage( m_usage ) );

		if (!_unbind_array_buffer())
			raise( format( "POSSIBLE RACE COND: at 'create': last bound vertex buffer 2d (id {}) was unbounded mid process", m_id ) );
	}

	template<typename _VRT>
	void BaseVertexBuffer<_VRT>::update( const vertex_type *vertices, const size_t vertices_count, const uint32_t offset ) {
		if (offset + vertices_count > m_size)
			raise( "overflowing a vertex buffer 2d" );

		_bind_array_buffer();

		glBufferSubData( GL_ARRAY_BUFFER, offset * sizeof( vertex_type ), vertices_count * sizeof( vertex_type ), vertices );

		if (!_unbind_array_buffer())
			raise( format( "POSSIBLE RACE COND: at 'update': last bound vertex buffer 2d (id {}) was unbounded mid process", m_id ) );
	}

	template<typename _VRT>
	void BaseVertexBuffer<_VRT>::update( const vertex_type *vertices ) {
		return update( vertices, size(), 0 );
	}

	template <typename _VRT>
	VertexBufferId_t BaseVertexBuffer<_VRT>::get_id() const noexcept
	{
		return m_id;
	}

	template <typename _VRT>
	void BaseVertexBuffer<_VRT>::_bind_array_buffer() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	template <typename _VRT>
	bool BaseVertexBuffer<_VRT>::_unbind_array_buffer() const
	{
		//int current_ab;
		//glGetIntegerv(GL_ARRAY_BUFFER, &current_ab);
		//if (current_ab == m_id)
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		return true;
	}

}
