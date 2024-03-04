#include "pch.h"
#include "_iglib_vpd.h"

static inline VPDName_t create_mesh_desc() {
	VPDName_t name = 0;
	glGenVertexArrays( 1, &name );
	return name;
}

static inline constexpr size_t sizeof_type( const VPDAttributeType type ) {
	switch (type)
	{
	case VPDAttributeType::Byte:
		return sizeof( int8_t );
	case VPDAttributeType::UnsignedByte:
		return sizeof( uint8_t );
	case VPDAttributeType::Short:
		return sizeof( int16_t );
	case VPDAttributeType::UnsignedShort:
		return sizeof( uint16_t );
	case VPDAttributeType::Int:
		return sizeof( int32_t );
	case VPDAttributeType::UnsignedInt:
		return sizeof( uint32_t );
	case VPDAttributeType::HalfFloat:
		return sizeof( uint16_t );
	case VPDAttributeType::Float:
		return sizeof( float );
	case VPDAttributeType::Double:
		return sizeof( double );
	default:
		return sizeof( float );
	}
}

class VPDSetupRegister
{
public:

	static inline void set_enabled_attrs( const size_t num ) {
		if (num > VertexPipelineDescriptor::MaxVertexAttributes)
		{
			s_enabled_attrs = VertexPipelineDescriptor::MaxVertexAttributes;
			return;
		}
		s_enabled_attrs = num;
	}

	static inline size_t get_enabled_attrs() {
		return s_enabled_attrs;
	}

private:
	static size_t s_enabled_attrs;
};

size_t VPDSetupRegister::s_enabled_attrs = 0;

namespace ig
{

	VertexPipelineDescriptor::VertexPipelineDescriptor()
		: m_name{ create_mesh_desc() }, m_attrs{}, m_offset{ 0 }, m_stride{ 0 } {
	}

	VertexPipelineDescriptor::~VertexPipelineDescriptor() {
		if (m_name)
		{
			glDeleteVertexArrays( 1, &m_name );
		}
	}

	VertexPipelineDescriptor::VertexPipelineDescriptor( const VertexPipelineDescriptor &copy )
		: m_name{ create_mesh_desc() }, m_attrs{ copy.m_attrs }, m_offset{ copy.m_offset }, m_stride{ copy.m_stride } {
	}

	VertexPipelineDescriptor::VertexPipelineDescriptor( VertexPipelineDescriptor &&move ) noexcept
		: m_name{ move.m_name }, m_attrs{ move.m_attrs }, m_offset{ move.m_offset }, m_stride{ move.m_stride } {
		move.m_name = 0;
	}

	VertexPipelineDescriptor &VertexPipelineDescriptor::operator=( const VertexPipelineDescriptor &copy ) {
		if (!m_name)
		{
			m_name = create_mesh_desc();
		}

		m_attrs = copy.m_attrs;
		m_offset = copy.m_offset;
		m_stride = copy.m_stride;
		return *this;
	}

	VertexPipelineDescriptor &VertexPipelineDescriptor::operator=( VertexPipelineDescriptor &&move ) noexcept {
		if (m_name)
		{
			glDeleteVertexArrays( 1, &m_name );
		}

		m_name = move.m_name;

		m_attrs = move.m_attrs;
		m_offset = move.m_offset;
		m_stride = move.m_stride;
		return *this;
	}

	void VertexPipelineDescriptor::bind() const {
		//glBindVertexArray( m_name );
	}

	void VertexPipelineDescriptor::setup() const {
		const size_t count = std::min( m_attrs.size(), MaxVertexAttributes );
		VPDSetupRegister::set_enabled_attrs( count );
		size_t offset = m_offset;
		for (size_t i = 0; i < count; i++)
		{
			glEnableVertexAttribArray( i );
			glVertexAttribPointer(
				i,
				static_cast<GLint>(m_attrs[ i ].size),
				static_cast<GLenum>(m_attrs[ i ].type),
				FALSE,
				m_stride,
				reinterpret_cast<const void *>(offset)
			);
			offset += static_cast<size_t>(m_attrs[ i ].size) * sizeof_type( m_attrs[ i ].type );
		}
	}

	VPDName_t VertexPipelineDescriptor::get_bound() {
		GLint value = 0;
		glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &value );
		return value;
	}

	void VertexPipelineDescriptor::clear_bound() {
		//glBindVertexArray( 0 );
	}

	void VertexPipelineDescriptor::clear_setup() {
		for (size_t i = 0; i < VPDSetupRegister::get_enabled_attrs(); i++)
		{
			glDisableVertexAttribArray( i );
		}
	}

	void VertexPipelineDescriptor::set_stride( uint32_t stride ) {
		m_stride = stride;
	}

	void VertexPipelineDescriptor::set_offset( uint32_t offset ) {
		m_offset = offset;
	}

	VertexPipelineDescriptor::AttributesInterface VertexPipelineDescriptor::create_interface() {
		return { *this };
	}

	size_t VertexPipelineDescriptor::get_vertex_size() const {
		size_t attrs_size = 0;
		for (const auto &attr : m_attrs)
		{
			attrs_size += static_cast<size_t>(attr.size);
		}
		return attrs_size;
	}

	void VertexPipelineDescriptor::_validate_attributes() {
		if (m_attrs.size() > MaxVertexAttributes)
		{
			m_attrs.resize( MaxVertexAttributes );
		}
	}

	VertexPipelineDescriptor::AttributesInterface::~AttributesInterface() {
		m_parent._validate_attributes();
	}

}
