#pragma once
#include "_iglib_vertex.h"
#include "_iglib_indexbuffer.h"
#include "_iglib_batchdraw.h"
#include "_iglib_image.h"
#include "_iglib_texture.h"
#include "_iglib_shader.h"
#include "_iglib_transform.h"

namespace ig
{
	class Window;

	class Canvas
	{
	public:
		Canvas(const Window &wnd);
		~Canvas();

		void quad(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, const Colorf &clr);
		void rect(Vector2f start, Vector2f end, const Colorf &clr);
		void traingle(Vector2f p0, Vector2f p1, Vector2f p2, const Colorf &clr);

		void line(Vector2f start, Vector2f end, const Colorb clr);
		void line(Vector2f start, Vector2f end, float_t width, const Colorb clr);
		void line(Vector3f start, Vector3f end, const Colorf &clr);

		void cube(Vector3f start, Vector3f end, const Colorf &clr);
		
		void circle(float radius, Vector2f center, const Colorb clr, const uint16_t vertcies_count = 32);
		
		void draw(Vertex2D *vert, size_t count, PrimitiveType draw_type);
		void draw(const Vertex2DBuffer &buf);
		void draw(const Vertex2DBuffer &buf, const IndexBuffer &indcies);

		void draw(const Vertex3DBuffer &buf);

		void demo();

		void bind_shader(const ShaderInstance_t &shader);
		void unbind_shader();

		ShaderId_t get_shader_id() const noexcept;

		// will update the _screensize and _time
		void update_shader_uniforms();

		const Window &get_window() const;

		inline Transform2D &transform2d() noexcept { return m_trans2d; }
		inline const Transform2D &transform2d() const noexcept { return m_trans2d; }

		inline Transform3D &transform3d() noexcept { return m_trans3d; }
		inline const Transform3D &transform3d() const noexcept { return m_trans3d; }

		void set_texture(const Texture &tex);

		// USE ONLY IF YOU WANT TO INTEGRATE OPENGL CODE WITH IGLIB OR YOU KNOW WHAT ARE YOU DOING
		void set_texture(const TextureId_t tex);

		TextureId_t get_texture() const noexcept;

	private:
		const Window &m_wnd;
		std::shared_ptr<const Shader> m_shader;
		TextureId_t m_tex;
		Transform2D m_trans2d{};
		Transform3D m_trans3d;
	};

	typedef void(*DrawCallback)(Canvas canvas);

}
