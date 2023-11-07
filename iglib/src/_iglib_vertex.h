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

	struct Vertex2D
	{
		Vector2f pos;
		Colorf clr;
		Vector2f uv; // <- also called 'tex_coord'
	};

	struct Vertex3D
	{
		Vector3f pos;
		Colorf clr;
		Vector2f uv; // <- also called 'tex_coord'
		Vector3f normal;
	};

	typedef unsigned VertexBufferId_t;

	class BaseVertexBuffer
	{
	public:

		size_t get_size() const noexcept;
		BufferUsage get_usage() const noexcept;
		PrimitiveType get_primitive() const noexcept;

		void set_primitive(PrimitiveType prim);
		void set_usage(BufferUsage usage);

		/// Internal OpenGL function
		VertexBufferId_t get_id() const noexcept;

		/// Internal OpenGL function, !ONLY !CALL !IF !YOU !KNOW !WHAT !YOU !ARE !DOING
		void _bind_array_buffer() const;

		/// Internal OpenGL function, !ONLY !CALL !IF !YOU !KNOW !WHAT !YOU !ARE !DOING
		bool _unbind_array_buffer() const;

	protected:
		BaseVertexBuffer(VertexBufferId_t id, size_t size = 0, BufferUsage usage = BufferUsage::Static, PrimitiveType type = PrimitiveType::TriangleStrip);


	private:
		BaseVertexBuffer(const BaseVertexBuffer &copy) = delete;
		BaseVertexBuffer(BaseVertexBuffer &&move) = delete;
		BaseVertexBuffer &operator =(const BaseVertexBuffer &copy) = delete;
		BaseVertexBuffer &operator =(BaseVertexBuffer &&move) = delete;

	protected:
		VertexBufferId_t m_id;
		size_t m_size;
		BufferUsage m_usage;
		PrimitiveType m_type;
	};

	class Vertex2DBuffer : public BaseVertexBuffer
	{
	public:
		using vertex_type = Vertex2D;

		Vertex2DBuffer();
		Vertex2DBuffer(size_t size);

		~Vertex2DBuffer();

		void create(const size_t size, const vertex_type *vertices = nullptr);

		/// updates the vertex buffer
		/// \param vertcies: the vertices that are loaded
		/// \param vertices_count: the vertices count to be loaded
		/// \param offset: where should the vertcies be put (e.g. 0 will put the vertices at the begining of the buffer and forward and 2 will put them at index [2] and forward)
		void update(const vertex_type *vertcies, const size_t vertices_count, const uint32_t offset);

		/// will update the entire current buffer with \p vertcies, \p vertices should be an array with size not smaller then the buffer size
		///
		/// same as vert_buffer.update(vertcies, vert_buffer.get_size(), 0)
		void update(const vertex_type *vertcies);

	};

	class Vertex3DBuffer : public BaseVertexBuffer
	{
	public:
		using vertex_type = Vertex3D;

		Vertex3DBuffer();
		Vertex3DBuffer(size_t size);

		~Vertex3DBuffer();

		void create(const size_t size, const vertex_type *vertices = nullptr);

		/// updates the vertex buffer
		/// \param vertcies: the vertices that are loaded
		/// \param vertices_count: the vertices count to be loaded
		/// \param offset: where should the vertcies be put (e.g. 0 will put the vertices at the begining of the buffer and forward and 2 will put them at index [2] and forward)
		void update(const vertex_type *vertcies, const size_t vertices_count, const uint32_t offset);

		/// will update the entire current buffer with \p vertcies, \p vertices should be an array with size not smaller then the buffer size
		///
		/// same as vert_buffer.update(vertcies, vert_buffer.get_size(), 0)
		void update(const vertex_type *vertcies);
	};

	// TODO: add a Vertex2DBatch and a Vertex3DBatch

}
