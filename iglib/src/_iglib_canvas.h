#pragma once
#include "_iglib_vertex.h"
#include "_iglib_vertexarray.h"
#include "_iglib_meshdesc.h"
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
		void quad(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, const Colorf &clr);
		void quad(const Vector3f p[], const Colorf &clr);

		void rect(Vector2f start, Vector2f end, const Colorf &clr);
		void triangle(Vector2f p0, Vector2f p1, Vector2f p2, const Colorf &clr);

		void plane(Vector3f center, Vector2f extent, const Colorf &clr);

		void line(Vector2f start, Vector2f end, const Colorf clr);
		void line(Vector2f start, Vector2f end, float_t width, const Colorf clr);
		void line(Vector3f start, Vector3f end, const Colorf &clr);

		void cube(Vector3f center, Vector3f extent, const Colorf &clr);
		
		void circle(float radius, Vector2f center, const Colorf clr, const uint16_t vertcies_count = 32);
		
		// better to use a managed vertex buffer
		// discourage the use for vertex count > 32
		// watch out for when the count is greater then the 32 bit signed limit (2^31)
		void draw(const Vertex2 *vertices, size_t count, PrimitiveType draw_type);

		void draw(const Vertex2Buffer &buf, int start = 0, int count = -1);
		void draw(const Vertex2Buffer &buf, const IndexBuffer &indcies);

		void draw(const Vertex3Buffer &buf, int start = 0, int count = -1);

		// for convenient sake only
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
		this->get_renderer()->bind_texture( txt.get_font().get_atlas() );
		this->get_renderer()->bind_shader( Font::get_shader() );
		this->draw( txt.get_buffer() );
		this->get_renderer()->bind_texture( NULL );
		this->get_renderer()->bind_default_shader( ShaderUsage::Usage2D );
	}

}
