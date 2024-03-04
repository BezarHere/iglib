#pragma once
#include "_iglib_base.h"
#include "_iglib_vector.h"
#include "_iglib_color.h"

namespace ig
{
	enum class PrimitiveType
	{
		Point,
		Quad, // <- DEPRECATED
		Triangle,
		Line,
		LineStrip,
		TriangleStrip,
		TriangleFan,
		QuadStrip // <- DEPRECATED
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

	typedef unsigned VertexBufferName_t;

	template <typename _VRT>
	class BaseVertexBuffer
	{
		// internal impl of a template? yes but it works
	public:
		using vertex_type = _VRT;

		BaseVertexBuffer();
		BaseVertexBuffer( size_t size );

		// setting 'size' to 0 will not create the buffer
		BaseVertexBuffer( PrimitiveType type, size_t size = 0, BufferUsage usage = BufferUsage::Static );
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
		/// \param vertices: the vertices that are used
		/// \param vertices_count: the vertices count to be read
		/// \param offset: where to update (passing 0 will update from the beginning of the buffer while passing N will update from the vertex N)
		void update( const vertex_type *vertices, const size_t vertices_count, const uint32_t offset );

		/// will update the entire current buffer with \p vertices, \p vertices should be an array with size not smaller then the buffer size
		///
		/// same as vert_buffer.update(vertices, vert_buffer.get_size(), 0)
		void update( const vertex_type *vertices );

		/// OpenGL function
		inline VertexBufferName_t get_name() const noexcept;

		void bind() const;

		static void clear_bound();
		static VertexBufferName_t get_bound();


	private:
		BaseVertexBuffer(VertexBufferName_t id, size_t size = 0, BufferUsage usage = BufferUsage::Static, PrimitiveType type = PrimitiveType::TriangleStrip);

	private:
		VertexBufferName_t m_name;
		size_t m_size;
		BufferUsage m_usage;
		PrimitiveType m_type;
	};

	template <typename _VRT>
	inline VertexBufferName_t BaseVertexBuffer<_VRT>::get_name() const noexcept {
		return m_name;
	}

	
	using Vertex2Buffer = BaseVertexBuffer<Vertex2>;
	using Vertex3Buffer = BaseVertexBuffer<Vertex3>;


}
