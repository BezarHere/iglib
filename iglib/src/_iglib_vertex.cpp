#include "pch.h"
#include "_iglib_vertex.h"
#include "draw_internal.h"

// template instancing:
template class BaseVertexBuffer<Vertex2>;
template class BaseVertexBuffer<Vertex3>;


FORCEINLINE VertexBufferName_t create_buffer() {
	VertexBufferName_t i = 0;
	glGenBuffers( 1, &i );
	if (!i)
	{
		bite::warn( "Warning: Creating vertex buffer failed with error code " + std::to_string( glGetError() ) );
	}
	return i;
}

FORCEINLINE static void free_buffer( VertexBufferName_t &id ) {
	glDeleteBuffers( 1, &id );
	id = 0;
}

FORCEINLINE VertexBufferName_t duplicate_buffer( const VertexBufferName_t copy, const int usage ) {
	if (!copy)
		return NULL;
	glBindBuffer( GL_ARRAY_BUFFER, copy );
	int bsize;
	glGetBufferParameteriv( GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize );
	uint8_t *buffer = new uint8_t[ bsize ];
	glGetBufferSubData( GL_ARRAY_BUFFER, 0, bsize, buffer );
	glBindBuffer( GL_ARRAY_BUFFER, NULL );

	VertexBufferName_t new_id = create_buffer();
	glBindBuffer( GL_ARRAY_BUFFER, new_id );
	glBufferData( GL_ARRAY_BUFFER, bsize, buffer, usage );
	glBindBuffer( GL_ARRAY_BUFFER, NULL );

	delete[] buffer;
	return new_id;
}

namespace ig
{

	template <typename _VRT>
	BaseVertexBuffer<_VRT>::BaseVertexBuffer( VertexBufferName_t id, size_t size, BufferUsage usage, PrimitiveType type )
		: m_name{ id }, m_size{ size }, m_usage{ usage }, m_type{ type } {
	}

	template<typename _VRT>
	BaseVertexBuffer<_VRT>::BaseVertexBuffer()
		: m_name{ 0 }, m_size{ 0 }, m_usage{ BufferUsage::Static }, m_type{ PrimitiveType::TriangleFan } {
	}

	template<typename _VRT>
	BaseVertexBuffer<_VRT>::BaseVertexBuffer( size_t size ) : BaseVertexBuffer() {
		create( size );
	}

	template<typename _VRT>
	BaseVertexBuffer<_VRT>::BaseVertexBuffer( PrimitiveType type, size_t size, BufferUsage usage )
		: m_name{ 0 }, m_type{ type }, m_usage{ usage }, m_size{ size } {
		if (size)
			create( size );
	}

	template<typename _VRT>
	BaseVertexBuffer<_VRT>::~BaseVertexBuffer() noexcept {
		if (m_name)
			free_buffer( m_name );
	}

	template <typename _VRT>
	BaseVertexBuffer<_VRT>::BaseVertexBuffer( const BaseVertexBuffer &copy )
		: m_name{ duplicate_buffer( copy.m_name, to_gldrawusage( copy.m_usage ) ) }, m_size{ copy.m_size }, m_usage{ copy.m_usage }, m_type{ copy.m_type } {

	}

	template <typename _VRT>
	BaseVertexBuffer<_VRT>::BaseVertexBuffer( BaseVertexBuffer &&move ) noexcept
		: m_name{ move.m_name }, m_size{ move.m_size }, m_usage{ move.m_usage }, m_type{ move.m_type } {
		move.m_name = NULL;
	}

	template <typename _VRT>
	BaseVertexBuffer<_VRT> &BaseVertexBuffer<_VRT>::operator=( const BaseVertexBuffer &copy ) {
		if (copy.m_name == m_name)
			return *this;

		if (m_name)
		{
			free_buffer( m_name );
		}

		m_name = duplicate_buffer( copy.m_name, to_gldrawusage( copy.m_usage ) );
		m_size = copy.m_size;
		m_type = copy.m_type;
		m_usage = copy.m_usage;
		return *this;
	}

	template <typename _VRT>
	BaseVertexBuffer<_VRT> &BaseVertexBuffer<_VRT>::operator=( BaseVertexBuffer &&move ) noexcept {
		if (m_name)
		{
			free_buffer( m_name );
		}

		m_name = move.m_name;
		m_size = move.m_size;
		m_type = move.m_type;
		m_usage = move.m_usage;
		move.m_name = NULL;
		return *this;
	}


	template <typename _VRT>
	size_t BaseVertexBuffer<_VRT>::size() const noexcept {
		return m_size;
	}

	template <typename _VRT>
	BufferUsage BaseVertexBuffer<_VRT>::get_usage() const noexcept {
		return m_usage;
	}

	template <typename _VRT>
	PrimitiveType BaseVertexBuffer<_VRT>::get_primitive() const noexcept {
		return m_type;
	}

	template <typename _VRT>
	void BaseVertexBuffer<_VRT>::set_primitive( PrimitiveType prim ) {
		m_type = prim;
	}

	template <typename _VRT>
	void BaseVertexBuffer<_VRT>::set_usage( BufferUsage usage ) {
		m_usage = usage;
	}

	template<typename _VRT>
	void BaseVertexBuffer<_VRT>::create( const size_t size, const vertex_type *vertices ) {
		if (m_name)
			free_buffer( m_name );

		m_size = size;
		m_name = create_buffer();
		bind();


		glBufferData( GL_ARRAY_BUFFER, size * sizeof( vertex_type ), vertices, to_gldrawusage( m_usage ) );

		clear_bound();
	}

	template<typename _VRT>
	void BaseVertexBuffer<_VRT>::update( const vertex_type *vertices, const size_t vertices_count, const uint32_t offset ) {
		if (offset + vertices_count > m_size)
			raise( "overflowing a vertex buffer 2d" );

		bind();

		glBufferSubData( GL_ARRAY_BUFFER, offset * sizeof( vertex_type ), vertices_count * sizeof( vertex_type ), vertices );

		clear_bound();
	}

	template<typename _VRT>
	void BaseVertexBuffer<_VRT>::update( const vertex_type *vertices ) {
		return update( vertices, size(), 0 );
	}

	template<typename _VRT>
	void BaseVertexBuffer<_VRT>::bind() const {
		glBindBuffer( GL_ARRAY_BUFFER, m_name );
	}

	template<typename _VRT>
	void BaseVertexBuffer<_VRT>::clear_bound() {
		glBindBuffer( GL_ARRAY_BUFFER, NULL );
	}

	template<typename _VRT>
	VertexBufferName_t BaseVertexBuffer<_VRT>::get_bound() {
		GLint val = 0;
		glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &val );
		return val;
	}

}
