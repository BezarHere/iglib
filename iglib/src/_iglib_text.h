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
			static inline Vector3f Set( float x, float y ) {
				return { x, y, 0.f };
			}
		};

		template <> struct DummyVectorSetterZ0<false>
		{
			static inline Vector2f Set( float x, float y ) {
				return { x, y };
			}
		};
	public:
		using vertex_buffer_type = _VERTBUF;
		using string_type = _STR;
		using vertex_type = typename _VERTBUF::vertex_type;
		static constexpr bool Is3D = std::is_same_v<vertex_type, Vertex3>;
		static constexpr size_t VerticesPerGlyph = 6; // 2 triangles

		inline BaseTextTemplate() {
			m_buffer.set_primitive( PrimitiveType::Triangle );
		}

		inline BaseTextTemplate( const string_type &str )
			: m_str{ str } {
			m_buffer.set_primitive( PrimitiveType::Triangle );
		}

		inline BaseTextTemplate( const string_type &str, const Font &font )
			: m_str{ str }, m_font{ font } {
			m_buffer.set_primitive( PrimitiveType::Triangle );
		}

		/// @brief builds a text mesh
		/// @param vertices the vertex array to be built to, unsuccessful build will not modify/update this
		/// @param count count of vertices (only used for bounds checking, the vertex array should optimally have a count of string.size() * VerticesPerGlyph)
		/// @param string that the mesh will represent
		/// @param font the font used
		/// @return vertices updated, greater then zero if the building was successful, zero if it's unsuccessful
		inline static size_t build( vertex_type *vertices, size_t count, const string_type &string, const Font &font, Vector2f scale = { 1.f, 1.f }, float tab_size = 4.f );

		/// @brief builds a text mesh
		/// @param count of 
		/// @param string that the mesh will represent
		/// @param font the font used
		/// @return weather the building was successful
		inline static std::shared_ptr<vertex_type> build( size_t &count_out, const string_type &string, const Font &font, Vector2f scale = { 1.f, 1.f }, float tab_size = 4.f ) {
			const size_t v_count = string.length() * VerticesPerGlyph;
			vertex_type *const verts = new vertex_type[ v_count ];
			count_out = build( verts, v_count, string, font, scale, tab_size );
			return std::shared_ptr<vertex_type>( verts );
		}

		inline const string_type &get_string() const {
			return m_str;
		}

		inline void set_string( const string_type &str ) {
			m_str = str;
			m_dirty = true;
		}

		// call 'rebuild' after editing the buffer
		inline vertex_buffer_type &get_buffer() {
			return m_buffer;
		}

		inline const vertex_buffer_type &get_buffer() const {
			return m_buffer;
		}

		// call 'rebuild' after editing the font
		inline Font &get_font() {
			return m_font;
		}

		inline const Font &get_font() const {
			return m_font;
		}

		inline void set_font( const Font &font ) {
			m_font = font;
			m_dirty = true;
		}

		inline bool is_dirty() const {
			return m_dirty;
		}

		inline Colorf get_color() const {
			return m_clr;
		}

		inline void set_color( const Colorf &clr ) {
			m_clr = clr;
		}

		inline Vector2f get_scale() const {
			return m_scale;
		}

		inline void set_scale( const Vector2f &scale ) {
			m_scale = scale;
		}

		// TODO: make this able to rebuild a specify part of the str
		inline void rebuild() {

			const size_t str_sz = m_str.size();
			const size_t buffer_size = str_sz * VerticesPerGlyph;
			vertex_type *vertices = new vertex_type[ buffer_size ];
			const size_t count = build( vertices, buffer_size, m_str, m_font, m_scale, m_tab_size );

			if (!count)
			{
				delete[] vertices;
				return;
			}

			// TODO: 3D and things
			for (size_t i = 0; i < count; i++)
			{
				vertices[ i ].clr = m_clr;
			}

			// if the buffer is too small for bigger then count + count / 2 then recreate the buffer
			if (m_buffer.size() < count || m_buffer.size() > count + (count >> 1))
				m_buffer.create( count, vertices );
			else
				m_buffer.update( vertices );

			m_dirty = false;
			delete[] vertices;
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

	using Text2D = BaseTextTemplate<Vertex2Buffer, std::string>;
	using Text3D = BaseTextTemplate<Vertex3Buffer, std::string>;

	using wText2D = BaseTextTemplate<Vertex2Buffer, std::wstring>;
	using wText3D = BaseTextTemplate<Vertex3Buffer, std::wstring>;

	using u16Text2D = BaseTextTemplate<Vertex2Buffer, std::u16string>;
	using u16Text3D = BaseTextTemplate<Vertex3Buffer, std::u16string>;

	template<typename _VERTBUF, typename _STR>
	inline size_t BaseTextTemplate<_VERTBUF, _STR>::build( vertex_type *vertices, const size_t count,
																												 const string_type &string, const Font &font,
																												 Vector2f scale, float tab_size ) {
		if (!font.valid())
			return 0;

		const size_t str_sz = string.size();
		Vector2f pos = { 0.f, 0.f };
		float line_height = 0.0f;
		size_t notdraw_count = 0;

		// TODO: 3D and things
		for (size_t i = 0; i < str_sz; i++)
		{
			const size_t gly_index = font.get_glyph_index( string[ i ] );


			if (string[ i ] == '\n')
			{
				pos.x = 0.f;
				pos.y += line_height + font.get_line_spacing();
				line_height = 0.0f;
				notdraw_count++;
				continue;
			}
			else if (string[ i ] == '\t')
			{
				pos.x += (tab_size * font.get_space_width()) * scale.x;
				notdraw_count++;
				continue;
			}
			else if (string[ i ] == ' ')
			{
				pos.x += font.get_space_width() * scale.x;
				notdraw_count++;
				continue;
			}

			const size_t j = (i - notdraw_count) * VerticesPerGlyph;

			if (count < j + VerticesPerGlyph)
				return 0;

			const Font::Glyph &gly = font.get_glyphs()[ gly_index != Font::NPos ? gly_index : 0 ];
			const Vector2f hs = Vector2f( gly.size ) * scale;
			const Vector2f offsets_scaled = Vector2f( gly.offset ) * scale;

			vertices[ j + 0 ].pos = DummyVectorSetterZ0<Is3D>::Set( pos.x + hs.x + offsets_scaled.x, pos.y + hs.y + offsets_scaled.y );
			vertices[ j + 0 ].uv = gly.atlas_uvbox.origin + gly.atlas_uvbox.left + gly.atlas_uvbox.bottom;
			vertices[ j + 0 ].clr = ColorfTable::White;

			vertices[ j + 1 ].pos = DummyVectorSetterZ0<Is3D>::Set( pos.x + offsets_scaled.x, pos.y + hs.y + offsets_scaled.y );
			vertices[ j + 1 ].uv = gly.atlas_uvbox.origin + gly.atlas_uvbox.left;
			vertices[ j + 1 ].clr = ColorfTable::White;

			vertices[ j + 2 ].pos = DummyVectorSetterZ0<Is3D>::Set( pos.x + offsets_scaled.x, pos.y + offsets_scaled.y );
			vertices[ j + 2 ].uv = gly.atlas_uvbox.origin;
			vertices[ j + 2 ].clr = ColorfTable::White;

			vertices[ j + 3 ].pos = DummyVectorSetterZ0<Is3D>::Set( pos.x + hs.x + offsets_scaled.x, pos.y + offsets_scaled.y );
			vertices[ j + 3 ].uv = gly.atlas_uvbox.origin + gly.atlas_uvbox.bottom;
			vertices[ j + 3 ].clr = ColorfTable::White;

			vertices[ j + 4 ] = vertices[ j + 0 ];
			vertices[ j + 5 ] = vertices[ j + 2 ];

			pos.x += hs.x + float( font.get_char_spacing() * scale.x ) + gly.advance;
			if (hs.y > line_height)
				line_height = hs.y;
		}

		return (string.size() - notdraw_count) * VerticesPerGlyph;
	}

}
