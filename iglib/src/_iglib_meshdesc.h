#pragma once
#include "_iglib_vertex.h"

namespace ig
{

	typedef unsigned int MeshDescName_t;
	class MeshDescriptor
	{
	public:

		inline MeshDescName_t get_name() const noexcept {
			return m_name;
		}

	private:
		MeshDescName_t m_name;
	};

}
