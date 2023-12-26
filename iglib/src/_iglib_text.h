#pragma once
#include "_iglib_font.h"
#include "_iglib_vertex.h"

namespace ig
{
	template <typename _VERTBUF, typename _STR = std::string>
	class BaseTextTemplate
	{
	private:
		template <bool _IN_3D>
		struct DummyVectorSetterZ0;

		template <> struct DummyVectorSetterZ0<true>
		{
			static inline Vector3f Set( float x, float y )
			{
				return { x, y, 0.f };
			}
		};

		template <> struct DummyVectorSetterZ0<false>
		{
			static inline Vector2f Set( float x, float y )
			{
				return { x, y };
			}
		};
	public:
		using vertex_buffer_type = _VERTBUF;
		using string_type = _STR;
		using vertex_type = typename _VERTBUF::vertex_type;
		static constexpr bool Is3D = std::is_same_v<vertex_type, Vertex3>;

		inline BaseTextTemplate()
		{
			m_buffer.set_primitive( PrimitiveType::Quad );
		}

		inline BaseTextTemplate( const string_type &str )
			: m_str{ str }
		{
			m_buffer.set_primitive( PrimitiveType::Quad );
		}

		inline BaseTextTemplate( const string_type &str, const Font &font )
			: m_str{ str }, m_font{ font }
		{
			m_buffer.set_primitive( PrimitiveType::Quad );
		}

		inline const string_type &get_string() const
		{
			return m_str;
		}

		inline void set_string( const string_type &str )
		{
			m_str = str;
			m_dirty = true;
		}

		// call 'rebuild' after editing the buffer
		inline vertex_buffer_type &get_buffer()
		{
			return m_buffer;
		}

		inline const vertex_buffer_type &get_buffer() const
		{
			return m_buffer;
		}

		// call 'rebuild' after editing the font
		inline Font &get_font()
		{
			return m_font;
		}

		inline const Font &get_font() const
		{
			return m_font;
		}

		inline void set_font( const Font &font )
		{
			m_font = font;
			m_dirty = true;
		}

		inline bool is_dirty() const
		{
			return m_dirty;
		}

		inline Colorf get_color() const
		{
			return m_clr;
		}

		inline void set_color( const Colorf &clr )
		{
			m_clr = clr;
		}

		inline Vector2f get_scale() const
		{
			return m_scale;
		}

		inline void set_scale( const Vector2f &scale )
		{
			m_scale = scale;
		}

		// TODO: make this able to rebuild a specifiy part of the str
		inline void rebuild()
		{
			if (!m_font.valid())
				return;

			const size_t str_sz = m_str.size();
			const size_t bufsz = str_sz * 4ull;
			vertex_type *vertcies = new vertex_type[ bufsz ];
			Vector2f pos = { 0.f, 0.f };
			float line_height = 0.0f;
			size_t notdraw_count = 0;

			// TODO: 3D and things
			for (size_t i = 0; i < str_sz; i++)
			{
				const size_t gly_index = m_font.get_glyph_index( m_str[ i ] );

				if (m_str[ i ] == '\n')
				{
					pos.x = 0.f;
					pos.y += line_height + m_font.get_line_spacing();
					line_height = 0.0f;
					notdraw_count++;
					continue;
				}
				else if (m_str[ i ] == '\t')
				{
					pos.x += (m_tab_size * m_font.get_space_width()) * m_scale.x;
					notdraw_count++;
					continue;
				}
				else if (m_str[ i ] == ' ')
				{
					pos.x += m_font.get_space_width() * m_scale.x;
					notdraw_count++;
					continue;
				}

				const size_t j = (i - notdraw_count) * 4;
				const Font::Glyph &gly = m_font.get_glyphs()[ gly_index != Font::NPos ? gly_index : 0 ];
				const Vector2f hs = Vector2f( gly.size ) * m_scale;
				const Vector2f offsetss = Vector2f( gly.offset ) * m_scale;

				vertcies[ j + 0 ].pos = DummyVectorSetterZ0<Is3D>::Set( pos.x + hs.x + offsetss.x, pos.y + hs.y + offsetss.y );
				vertcies[ j + 0 ].uv = gly.atlas_uvbox.origin + gly.atlas_uvbox.left + gly.atlas_uvbox.bottom;
				vertcies[ j + 0 ].clr = m_clr;
				vertcies[ j + 1 ].pos = DummyVectorSetterZ0<Is3D>::Set( pos.x + offsetss.x, pos.y + hs.y + offsetss.y );
				vertcies[ j + 1 ].uv = gly.atlas_uvbox.origin + gly.atlas_uvbox.left;
				vertcies[ j + 1 ].clr = m_clr;
				vertcies[ j + 2 ].pos = DummyVectorSetterZ0<Is3D>::Set( pos.x + offsetss.x, pos.y + offsetss.y );
				vertcies[ j + 2 ].uv = gly.atlas_uvbox.origin;
				vertcies[ j + 2 ].clr = m_clr;
				vertcies[ j + 3 ].pos = DummyVectorSetterZ0<Is3D>::Set( pos.x + hs.x + offsetss.x, pos.y + offsetss.y );
				vertcies[ j + 3 ].uv = gly.atlas_uvbox.origin + gly.atlas_uvbox.bottom;
				vertcies[ j + 3 ].clr = m_clr;

				pos.x += hs.x + float( m_font.get_char_spacing() * m_scale.x ) + gly.advance;
				if (hs.y > line_height)
					line_height = hs.y;
			}

			if (m_buffer.size() != bufsz - notdraw_count)
				m_buffer.create( bufsz - notdraw_count, vertcies );
			else
				m_buffer.update( vertcies );

			m_dirty = false;
			delete[] vertcies;
		}

	private:
		vertex_buffer_type m_buffer = {};
		string_type m_str = {};
		Font m_font = {};
		Colorf m_clr = { 1.f, 1.f, 1.f };
		Vector2f m_scale = { 1.f, 1.f };
		uint32_t m_tab_size = 4;
		bool m_dirty = true;
	};

	using Text2D = BaseTextTemplate<Vertex2DBuffer, std::string>;
	using Text3D = BaseTextTemplate<Vertex3DBuffer, std::string>;

	using wText2D = BaseTextTemplate<Vertex2DBuffer, std::wstring>;
	using wText3D = BaseTextTemplate<Vertex3DBuffer, std::wstring>;

	using u16Text2D = BaseTextTemplate<Vertex2DBuffer, std::u16string>;
	using u16Text3D = BaseTextTemplate<Vertex3DBuffer, std::u16string>;
}
