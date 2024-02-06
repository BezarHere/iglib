#pragma once
#include "_iglib_base.h"
#include "_iglib_vertex.h"

namespace ig
{
	template <typename _VRT, typename _CNT = std::vector<_VRT>>
	class BaseVertexArray
	{
	public:
		static constexpr size_t npos = static_cast<size_t>(-1);

		using vertex_type = _VRT;
		using vertex_buffer = BaseVertexBuffer<vertex_type>;
		using container_type = _CNT;

		inline BaseVertexArray()
			: m_buffer{}, m_vertices{}, m_dirty{ false } {
		}

		inline BaseVertexArray( PrimitiveType type, size_t size = 0, BufferUsage usage = BufferUsage::Static )
			: m_buffer{ type, size, usage }, m_vertices{}, m_dirty{ false } {
			m_vertices.resize( size, vertex_type() );
		}

		inline bool is_dirty() const noexcept {
			return m_dirty;
		}

		inline container_type &get_vertices() {
			return m_vertices;
		}

		inline const container_type &get_vertices() const {
			return m_vertices;
		}

		inline vertex_buffer &get_buffer() {
			return m_buffer;
		}

		inline const vertex_buffer &get_buffer() const {
			return m_buffer;
		}

		inline void update( size_t from = 0, size_t to = npos ) {
			// should we assert that 'from' < 'to'?
			_ASSERT( from < to ); // <- removed when not in debug (_DEBUG not defined) [crtdbg.h]

			if (to == npos)
			{
				to = m_vertices.size();
			}

			if (m_buffer.size() != to)
			{
				m_buffer.create( to );
			}

			if (m_dirty)
				m_dirty = false;

			m_buffer.update( m_vertices.data() + from, to - from, static_cast<uint32_t>(from) );
		}

		inline void push_back( const vertex_type &vertex ) {
			if (!m_dirty)
				m_dirty = true;
			return m_vertices.push_back( vertex );
		}

		template <typename ..._VALTY>
		inline void emplace_back( _VALTY &&... args ) {
			if (!m_dirty)
				m_dirty = true;
			return m_vertices.emplace_back( args );
		}

		inline vertex_type &operator[]( size_t index ) {
			if (!m_dirty)
				m_dirty = true;
			return m_vertices[index];
		}

		inline const vertex_type &at( size_t index ) const {
			return m_vertices.at( index );
		}

	private:
		container_type m_vertices;
		vertex_buffer m_buffer;
		bool m_dirty;
	};


	using Vertex2Array = BaseVertexArray<Vertex2, std::vector<Vertex2>>;
	using Vertex3Array = BaseVertexArray<Vertex3, std::vector<Vertex3>>;
}
