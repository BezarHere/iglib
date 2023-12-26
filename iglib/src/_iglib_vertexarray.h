#pragma once
#include "_iglib_base.h"
#include "_iglib_vertex.h"

namespace ig
{
	template <typename _VRT>
	class BaseVertexArray
	{
	public:
		static constexpr size_t npos = static_cast<size_t>(-1);

		using vertex_type = _VRT;
		using vertex_buffer = BaseVertexBuffer<vertex_type>;
		using verticies_container = std::vector<vertex_type>;

		inline BaseVertexArray()
			: m_buffer{}, m_vertices{} {
		}

		inline BaseVertexArray( PrimitiveType type, size_t size = 0, BufferUsage usage = BufferUsage::Static )
			: m_buffer{ type, size, usage }, m_vertices{} {
			m_vertices.resize( size, vertex_type() );
		}

		inline verticies_container &get_vertices() {
			return m_vertices;
		}

		inline const verticies_container &get_vertices() const {
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

			if (m_buffer.size() < to)
			{
				m_buffer.create( to );
			}

			m_buffer.update( m_vertices.data() + from, to - from, from );
		}

	private:
		std::vector<vertex_type> m_vertices;
		vertex_buffer m_buffer;
	};


	using Vertex2Array = BaseVertexArray<Vertex2>;
	using Vertex3Array = BaseVertexArray<Vertex3>;
}
