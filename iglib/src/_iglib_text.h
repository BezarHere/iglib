#pragma once
#include "_iglib_font.h"
#include "_iglib_vertex.h"

namespace ig
{
	template <typename _VERTBUF, typename _STR = std::string>
	class BaseTextTemplate
	{
	public:
		using vertex_buffer_type = _VERTBUF;
		using string_type = _STR;
		using vertex_type = typename _VERTBUF::vertex_type;
		static constexpr bool Is3D = std::is_same_v<vertex_type, Vertex3D>;

		BaseTextTemplate()
		{

		}

		BaseTextTemplate(const std::string &str)
		{

		}

		inline void rebuild()
		{
			vertex_type *vertcies = new vertex_type[m_str.size()];
			for (int i = 0; i < m_str.size(); i++)
			{
				vertcies[ i ].pos.x = i; // TODO: ?
			}
			delete[] vertcies;
		}

	private:
		vertex_buffer_type m_buffer;
		string_type m_str;
		Font m_font;
	};

	using Text2D = BaseTextTemplate<Vertex2DBuffer, std::string>;
	using Text3D = BaseTextTemplate<Vertex3DBuffer, std::string>;

	using wText2D = BaseTextTemplate<Vertex2DBuffer, std::wstring>;
	using wText3D = BaseTextTemplate<Vertex3DBuffer, std::wstring>;

	using u16Text2D = BaseTextTemplate<Vertex2DBuffer, std::u16string>;
	using u16Text3D = BaseTextTemplate<Vertex3DBuffer, std::u16string>;
}
