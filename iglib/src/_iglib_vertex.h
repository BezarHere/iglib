#pragma once
#include "_iglib_base.h"
#include "_iglib_vector.h"
#include "_iglib_color.h"

namespace ig
{
	enum class PrimitiveType
	{
		Quad,
		Triangle,
		Line,
		LineStript, // connected lines
		TriangleStrip,
		TriangleFan,
		QuadStrip
	};

	enum class BufferUsage
	{
		Static = 0,
		Dynamic = 1,
		Stream = 2
	};

	struct Vertex2
	{
		Vector2f pos;
		Colorf clr;
		Vector2f uv; // <- also called 'tex_cord'
	};

	struct Vertex3
	{
		Vector3f pos;
		Colorf clr;
		Vector2f uv; // <- also called 'tex_cord'
		Vector3f normal;
	};

	typedef unsigned VertexBufferId_t;

	template <typename _VRT>
	class BaseVertexBuffer
	{
		// internal impl of a template? yes but... it works
	public:
		using vertex_type = _VRT;

		BaseVertexBuffer();
		BaseVertexBuffer( size_t size );
		~BaseVertexBuffer() noexcept;

		BaseVertexBuffer( const BaseVertexBuffer &copy );
		BaseVertexBuffer( BaseVertexBuffer &&move ) noexcept;
		BaseVertexBuffer &operator =( const BaseVertexBuffer &copy );
		BaseVertexBuffer &operator =( BaseVertexBuffer &&move ) noexcept;

		size_t size() const noexcept;
		BufferUsage get_usage() const noexcept;
		PrimitiveType get_primitive() const noexcept;

		void set_primitive(PrimitiveType prim);
		void set_usage(BufferUsage usage);

		void create( const size_t size, const vertex_type *vertices = nullptr );

		/// updates the vertex buffer
		/// \param vertices: the vertices that are loaded
		/// \param vertices_count: the vertices count to be loaded
		/// \param offset: where should the vertices be put (e.g. 0 will put the vertices at the beginning of the buffer and forward and 2 will put them at index [2] and forward)
		void update( const vertex_type *vertices, const size_t vertices_count, const uint32_t offset );

		/// will update the entire current buffer with \p vertices, \p vertices should be an array with size not smaller then the buffer size
		///
		/// same as vert_buffer.update(vertices, vert_buffer.get_size(), 0)
		void update( const vertex_type *vertices );

		/// OpenGL function
		VertexBufferId_t get_id() const noexcept;

		/// Internal OpenGL function
		void _bind_array_buffer() const;

		/// Internal OpenGL function
		bool _unbind_array_buffer() const;



	private:
		BaseVertexBuffer(VertexBufferId_t id, size_t size = 0, BufferUsage usage = BufferUsage::Static, PrimitiveType type = PrimitiveType::TriangleStrip);

	private:
		VertexBufferId_t m_id;
		size_t m_size;
		BufferUsage m_usage;
		PrimitiveType m_type;
	};

	using Vertex2DBuffer = BaseVertexBuffer<Vertex2>;
	using Vertex3DBuffer = BaseVertexBuffer<Vertex3>;

}
