#pragma once
#include "_iglib_vertex.h"
#include "_iglib_indexbuffer.h"
#include "_iglib_image.h"
#include "_iglib_texture.h"
#include "_iglib_shader.h"
#include "_iglib_transform.h"
#include "_iglib_text.h"
#include "_iglib_camera.h"

namespace ig
{
	class Renderer;

	class Canvas
	{
		friend Renderer;
	public:
		Canvas( Canvas &&move ) noexcept;
		~Canvas();
		

		void quad(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, const Colorf &clr);
		void quad(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, const Colorf &clr); // <- should be counter clockwise winded
		void quad(const Vector3f p[], const Colorf &clr); // <- should be counter clockwise winded

		void rect(Vector2f start, Vector2f end, const Colorf &clr);
		void traingle(Vector2f p0, Vector2f p1, Vector2f p2, const Colorf &clr);

		void plane(Vector3f center, Vector2f extent, const Colorf &clr);

		void line(Vector2f start, Vector2f end, const Colorf clr);
		void line(Vector2f start, Vector2f end, float_t width, const Colorf clr);
		void line(Vector3f start, Vector3f end, const Colorf &clr);

		void cube(Vector3f center, Vector3f extent, const Colorf &clr);
		
		void circle(float radius, Vector2f center, const Colorf clr, const uint16_t vertcies_count = 32);
		
		void draw(Vertex2D *vert, size_t count, PrimitiveType draw_type);
		void draw(const Vertex2DBuffer &buf, int start = 0, int count = -1);
		void draw(const Vertex2DBuffer &buf, const IndexBuffer &indcies);

		void draw(const Vertex3DBuffer &buf, int start = 0, int count = -1);

		// for conveinent sake only
		template <typename _VB, typename _ST>
		void text( const BaseTextTemplate<_VB, _ST> &txt );

		inline Transform2D &transform2d() noexcept { return m_transform2d; }
		inline const Transform2D &transform2d() const noexcept { return m_transform2d; }

		inline Transform3D &transform3d() noexcept { return m_transform3d; }
		inline const Transform3D &transform3d() const noexcept { return m_transform3d; }

		void update_transform_state();

		Camera &camera();
		const Camera &camera() const;
		void update_camera();

		Renderer *get_renderer();
		const Renderer *get_renderer() const;

	private:
		Canvas();
		Canvas( Renderer *renderer );
		void operator=(Canvas &&) noexcept;
		Canvas(const Canvas &) = delete;
		void operator=(const Canvas &) = delete;

	private:
		Renderer *m_renderer;

		Transform2D m_transform2d{};
		Transform3D m_transform3d{};

		Camera m_camera;
		Matrix4x4 m_cam_cache;
	};

	typedef void(*DrawCallback)(Canvas &canvas);

	template<typename _VB, typename _ST>
	inline void Canvas::text( const BaseTextTemplate<_VB, _ST> &txt ) {
		//if (txt.is_dirty())
		//	txt.rebuild();
		this->get_renderer().bind_texture(txt.get_font().get_atlas());
		this->draw( txt.get_buffer() );
	}

}
