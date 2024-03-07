#include "pch.h"
#include "_iglib_vbuffer.h"

template RawVertexBuffer;
template class VBuffer<Vertex2, GL_ARRAY_BUFFER>;
template class VBuffer<Vertex3, GL_ARRAY_BUFFER>;

template class VBuffer<uint32_t, GL_ELEMENT_ARRAY_BUFFER>;
template class VBuffer<uint16_t, GL_ELEMENT_ARRAY_BUFFER>;
template class VBuffer<uint8_t, GL_ELEMENT_ARRAY_BUFFER>;

template class VBuffer<void, GL_UNIFORM_BUFFER>;
template class VBuffer<void, GL_TEXTURE_BUFFER>;

namespace vbuffer_indirection
{
	// why not a switch statement? idk, maybe later
	template <int GLType>
	constexpr GLint get_binding_name();

	template <> constexpr GLint get_binding_name<GL_ARRAY_BUFFER>() {
		return GL_ARRAY_BUFFER_BINDING;
	}

	template <> constexpr GLint get_binding_name<GL_ELEMENT_ARRAY_BUFFER>() {
		return GL_ELEMENT_ARRAY_BUFFER_BINDING;
	}

	template <> constexpr GLint get_binding_name<GL_UNIFORM_BUFFER>() {
		return GL_UNIFORM_BUFFER_BINDING;
	}

	template <> constexpr GLint get_binding_name<GL_TEXTURE_BUFFER>() {
		return GL_TEXTURE_BUFFER_BINDING;
	}

	template <int GLType>
	static std::stack<VBufferName_t, std::vector<VBufferName_t>> &get_vbuffer_names_stack() {
		using stack_type = std::stack<VBufferName_t, std::vector<VBufferName_t>>;
		static stack_type s{};
		return s;
	}

}



template <typename element_type, int GLType>
static inline VBufferName_t generate_vbuffer( size_t size, VBufferUsage usage, const element_type *data ) {
	VBufferName_t name = 0;
	glGenBuffers( 1, &name );
	glBindBuffer( GLType, name );

	if constexpr (std::is_void_v<element_type>)
	{
		glBufferData( GLType, 1 * size, reinterpret_cast<const void *>(data), static_cast<GLenum>(usage) );
	}
	else
	{
		glBufferData( GLType, sizeof( element_type ) * size, reinterpret_cast<const void *>(data), static_cast<GLenum>(usage) );
	}

	glBindBuffer( GLType, NULL );
	return name;
}

template <typename element_type, int GLType>
FORCEINLINE VBufferName_t duplicate_buffer( const VBufferName_t copy, const VBufferUsage usage ) {
	if (!copy)
		return NULL;
	glBindBuffer( GLType, copy );
	uint32_t bsize = 0;
	glGetBufferParameteriv( GLType, GL_BUFFER_SIZE, reinterpret_cast<int *>(&bsize) );

#ifdef _DEBUG
	// something might be fucked up
	if constexpr (!std::is_void_v<element_type>)
	{
		if (bsize % sizeof( element_type ))
		{
			// i know, this isn't C
			char msg[ 1024 ] = { 0 };
			sprintf_s( msg, std::size( msg ),
								 "VBuffer<%s, %d> holds a buffer of size %u bytes, which isn't divisible by the VBuffer::value_type's size (%lld); discarding excess",
								 // is typeid() standard?
								 typeid(element_type).raw_name(),
								 GLType, bsize, sizeof( element_type ) );

			bite::warn( msg );

		}
	}
#endif // _DEBUG

	constexpr auto SafeSize = []( const size_t buffer_size ) {
		(void)buffer_size;
		if constexpr (std::is_void_v<element_type>) return size_t( 1 ); else return buffer_size / sizeof( element_type );
		};

	const size_t buffer_obj_size = SafeSize( bsize );

	uint8_t *buffer = new uint8_t[ bsize ];
	glGetBufferSubData( GLType, 0, bsize, buffer );
	glBindBuffer( GLType, NULL );

	VBufferName_t new_id = generate_vbuffer<element_type, GLType>( buffer_obj_size, usage, reinterpret_cast<element_type *>(buffer) );

	delete[] buffer;
	return new_id;
}

namespace ig
{
	template<typename _T, int _GL_TYPE>
	VBuffer<_T, _GL_TYPE>::VBuffer() : m_name{} {
	}

	template<typename _T, int _GL_TYPE>
	VBuffer<_T, _GL_TYPE>::VBuffer( size_t size, VBufferUsage usage, const element_type *data )
		: m_name{ generate_vbuffer<_T, _GL_TYPE>( size, usage, data ) } {
	}

	template<typename _T, int _GL_TYPE>
	VBuffer<_T, _GL_TYPE>::~VBuffer() {
		if (m_name)
		{
			free();
		}
	}

	template<typename _T, int _GL_TYPE>
	VBuffer<_T, _GL_TYPE>::VBuffer( const this_type &copy )
		: m_name{ duplicate_buffer<_T, _GL_TYPE>( copy.m_name, copy.usage() ) } {
	}

	template<typename _T, int _GL_TYPE>
	VBuffer<_T, _GL_TYPE>::VBuffer( this_type &&move ) noexcept : m_name{ move.m_name } {
		move.m_name = NULL;
	}

	template<typename _T, int _GL_TYPE>
	VBuffer<_T, _GL_TYPE> &VBuffer<_T, _GL_TYPE>::operator=( const this_type &copy ) {
		if (copy.m_name == m_name)
			return *this;

		if (m_name)
		{
			free();
		}

		m_name = duplicate_buffer<_T, _GL_TYPE>( copy.m_name, copy.usage() );
		return *this;
	}

	template<typename _T, int _GL_TYPE>
	VBuffer<_T, _GL_TYPE> &VBuffer<_T, _GL_TYPE>::operator=( this_type &&move ) noexcept {
		if (m_name)
		{
			free();
		}

		m_name = move.m_name;
		move.m_name = NULL;
		return *this;
	}

	template<typename _T, int _GL_TYPE>
	void VBuffer<_T, _GL_TYPE>::create( size_t size, VBufferUsage usage, const element_type *data ) {
		if (m_name)
		{
			free();
		}

		m_name = generate_vbuffer<_T, _GL_TYPE>( size, usage, data );
	}

	template<typename _T, int _GL_TYPE>
	VBufferUsage VBuffer<_T, _GL_TYPE>::usage() const {
		VBufferUsage usage;
		push_bound_name();
		bind();

		glGetBufferParameteriv( GLType, GL_BUFFER_USAGE, reinterpret_cast<GLint *>(&usage) );

		pop_bound_name();
		return usage;
	}

	template<typename _T, int _GL_TYPE>
	size_t VBuffer<_T, _GL_TYPE>::size() const {
		if constexpr (Generic)
		{
			return size_bytes();
		}
		else
		{
			return size_bytes() / sizeof( element_type );
		}
	}

	template<typename _T, int _GL_TYPE>
	size_t VBuffer<_T, _GL_TYPE>::size_bytes() const {
		push_bound_name();

		bind();
		size_t bsize = 0;
		glGetBufferParameteriv( GLType, GL_BUFFER_SIZE, reinterpret_cast<int *>(&bsize) );

		pop_bound_name();
		return bsize;
	}

	template<typename _T, int _GL_TYPE>
	void VBuffer<_T, _GL_TYPE>::update( const element_type *data, size_t start, size_t end ) {
		if constexpr (Generic)
		{
			update_bytes( data, start, end );
		}
		else
		{
			update_bytes( data, start * sizeof( element_type ), end * sizeof( element_type ) );
		}
	}

	template<typename _T, int _GL_TYPE>
	void VBuffer<_T, _GL_TYPE>::update_bytes( const void *data, size_t start_byte, size_t end_byte ) {
		const size_t buffer_size = size_bytes();

		// no data );
		if (!buffer_size)
		{
			std::cerr << "VBUFFER: buffer " << this << " can't be updated, it's empty.\n";
			return;
		}

		if (start_byte >= buffer_size)
		{
			std::cerr << "VBUFFER: buffer "
				<< this
				<< " can't be updated starting from byte "
				<< start_byte
				<< ", reason is the buffer is only "
				<< buffer_size
				<< " bytes long\n";
			return;
		}

		if (start_byte >= end_byte)
		{
			// why? warping indexes baby
			end_byte = buffer_size - end_byte;
		}

		if (end_byte >= buffer_size)
		{
			std::cerr << "VBUFFER: buffer "
				<< this
				<< " can't be updated to byte "
				<< end_byte
				<< ", reason is the buffer is only "
				<< buffer_size
				<< " bytes long\n";
			return;
		}

		glBindBuffer( GLType, m_name );

		glBufferSubData( GLType, start_byte, (end_byte - start_byte), data );

		glBindBuffer( GLType, NULL );
	}

	template<typename _T, int _GL_TYPE>
	void VBuffer<_T, _GL_TYPE>::bind() const {
		glBindBuffer( GLType, m_name );
	}

	template<typename _T, int _GL_TYPE>
	bool VBuffer<_T, _GL_TYPE>::is_bound() const {
		return get_bound() == m_name;
	}

	template<typename _T, int _GL_TYPE>
	VBufferName_t VBuffer<_T, _GL_TYPE>::get_bound() {
		GLint name = NULL;
		glGetIntegerv( vbuffer_indirection::get_binding_name<GLType>(), &name );
		return static_cast<VBufferName_t>(name);
	}

	template<typename _T, int _GL_TYPE>
	void VBuffer<_T, _GL_TYPE>::clear_bound() {
		glBindBuffer( GLType, 0 );
	}

	template<typename _T, int _GL_TYPE>
	void VBuffer<_T, _GL_TYPE>::push_bound_name() {
		vbuffer_indirection::get_vbuffer_names_stack<GLType>().push( get_bound() );
		clear_bound();
	}

	template<typename _T, int _GL_TYPE>
	void VBuffer<_T, _GL_TYPE>::pop_bound_name() {
		glBindBuffer( GLType, vbuffer_indirection::get_vbuffer_names_stack<GLType>().top() );
		vbuffer_indirection::get_vbuffer_names_stack<GLType>().pop();
	}

	template<typename _T, int _GL_TYPE>
	void VBuffer<_T, _GL_TYPE>::free() {
		glDeleteBuffers( 1, &m_name );
		m_name = 0;
	}

}
