#pragma once
#include "_iglib_vertex.h"

namespace ig
{

	typedef unsigned int MeshDescName_t;

	template <typename _VERTBUF>
	class TMeshDescriptor
	{
	public:
		using vertex_buffer_type = _VERTBUF;
		using vertex_type = typename vertex_buffer_type::vertex_type;
		using this_type = TMeshDescriptor;

		TMeshDescriptor();
		~TMeshDescriptor();

		TMeshDescriptor(const this_type &copy);
		TMeshDescriptor(this_type &&move) noexcept;
		TMeshDescriptor &operator=(const this_type &copy);
		TMeshDescriptor &operator=(this_type &&move) noexcept;

		inline MeshDescName_t get_name() const noexcept {
			return m_name;
		}

	private:
		MeshDescName_t m_name;
		PrimitiveType m_primitive;
	};

	using MeshDescriptor2 = TMeshDescriptor<Vertex2Buffer>;
	using MeshDescriptor3 = TMeshDescriptor<Vertex3Buffer>;
}
