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
	enum class TextureSlot
	{
		Slot0, Slot1, Slot2, Slot3,
		Slot4, Slot5, Slot6, Slot7,
		_MAX
	};

	class Window;

	class Canvas
	{
		friend Window;
	public:
		~Canvas();

		void quad(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, const Colorf &clr);
		void rect(Vector2f start, Vector2f end, const Colorf &clr);
		void traingle(Vector2f p0, Vector2f p1, Vector2f p2, const Colorf &clr);

		void line(Vector2f start, Vector2f end, const Colorb clr);
		void line(Vector2f start, Vector2f end, float_t width, const Colorb clr);
		void line(Vector3f start, Vector3f end, const Colorf &clr);

		void cube(Vector3f start, Vector3f end, const Colorf &clr);
		
		void circle(float radius, Vector2f center, const Colorf clr, const uint16_t vertcies_count = 32);
		
		void draw(Vertex2D *vert, size_t count, PrimitiveType draw_type);
		void draw(const Vertex2DBuffer &buf, int from = 0, int to = 0);
		void draw(const Vertex2DBuffer &buf, const IndexBuffer &indcies);

		void draw(const Vertex3DBuffer &buf, int from = 0, int to = 0);

		void demo();

		void bind_shader(const ShaderInstance_t &shader);
		void unbind_shader();
		// what will the canvas expect to draw in later calls? and what default shader will it resort to?
		void set_shading_usage(const ShaderUsage usage);

		ShaderId_t get_shader_id() const noexcept;

		// will update the _screensize and _time
		void update_shader_uniforms();

		const Window &get_window() const;

		inline Transform2D &transform2d() noexcept { return m_trans2d; }
		inline const Transform2D &transform2d() const noexcept { return m_trans2d; }

		inline Transform3D &transform3d() noexcept { return m_trans3d; }
		inline const Transform3D &transform3d() const noexcept { return m_trans3d; }

		// USE ONLY IF YOU WANT TO INTEGRATE OPENGL CODE WITH IGLIB OR YOU KNOW WHAT ARE YOU DOING
		void set_texture(const TextureId_t tex, const TextureSlot slot = TextureSlot::Slot0);
		TextureId_t get_texture(const TextureSlot slot = TextureSlot::Slot0) const noexcept;

		void set_active_textures_count(int count);
		int get_active_textures_count() const noexcept;

	private:
		Canvas(const Window &wnd);
		Canvas(const Canvas &) = delete;
		Canvas(Canvas &&) = delete;
		void operator=(const Canvas &) = delete;
		void operator=(Canvas &&) = delete;

	private:
		const Window &m_wnd;
		std::shared_ptr<const Shader> m_shader;
		ShaderUsage m_shading_usage = ShaderUsage::Usage2D;
		TextureId_t m_textures[int(TextureSlot::_MAX)];
		int m_active_textrues_count = 1; // will upload all textures from 0 to m_active_textrues_count - 1
		Transform2D m_trans2d{};
		Transform3D m_trans3d;
	};

	typedef void(*DrawCallback)(Canvas &canvas);

}
