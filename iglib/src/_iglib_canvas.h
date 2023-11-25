#pragma once
#include "_iglib_vertex.h"
#include "_iglib_indexbuffer.h"
#include "_iglib_batchdraw.h"
#include "_iglib_image.h"
#include "_iglib_texture.h"
#include "_iglib_shader.h"
#include "_iglib_transform.h"
#include "_iglib_text.h"
#include "_iglib_camera.h"

namespace ig
{
	enum class TextureSlot
	{
		Slot0, Slot1, Slot2, Slot3,
		Slot4, Slot5, Slot6, Slot7,
		_MAX
	};

	enum class DrawType
	{
		Drawing2D,
		Drawing3D,
		DirectDrawing,
		Raw,
	};

	class Window;

	class Canvas
	{
		friend Window;
	public:
		~Canvas();
		
		// what will the canvas expect to draw in later calls? and what default shader will it resort to?
		void set_draw_type( const DrawType type );

		void quad(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, const Colorf &clr);
		void rect(Vector2f start, Vector2f end, const Colorf &clr);
		void traingle(Vector2f p0, Vector2f p1, Vector2f p2, const Colorf &clr);

		void line(Vector2f start, Vector2f end, const Colorb clr);
		void line(Vector2f start, Vector2f end, float_t width, const Colorb clr);
		void line(Vector3f start, Vector3f end, const Colorf &clr);

		void cube(Vector3f start, Vector3f end, const Colorf &clr);
		
		void circle(float radius, Vector2f center, const Colorf clr, const uint16_t vertcies_count = 32);
		
		void draw(Vertex2D *vert, size_t count, PrimitiveType draw_type);
		void draw(const Vertex2DBuffer &buf, int start = 0, int count = -1);
		void draw(const Vertex2DBuffer &buf, const IndexBuffer &indcies);

		void draw(const Vertex3DBuffer &buf, int start = 0, int count = -1);

		void bind_shader(const ShaderInstance_t &shader);
		void unbind_shader();

		ShaderId_t get_shader_id() const noexcept;

		// will update the _screensize and _time
		void update_shader_uniforms();

		const Window &get_window() const;

		inline Transform2D &transform2d() noexcept { return m_transform2d; }
		inline const Transform2D &transform2d() const noexcept { return m_transform2d; }

		inline Transform3D &transform3d() noexcept { return m_transform3d; }
		inline const Transform3D &transform3d() const noexcept { return m_transform3d; }

		// USE ONLY IF YOU WANT TO INTEGRATE OPENGL CODE WITH IGLIB OR YOU KNOW WHAT ARE YOU DOING
		void set_texture(const TextureId_t tex, const TextureSlot slot = TextureSlot::Slot0);
		TextureId_t get_texture(const TextureSlot slot = TextureSlot::Slot0) const noexcept;

		void set_active_textures_count(int count);
		int get_active_textures_count() const noexcept;

		Camera &camera();
		const Camera &camera() const;
		void update_camera();

		/// \note used to set samplers slots
		void set_shader_uniform(int location, int value);
		void set_shader_uniform(int location, float value);
		void set_shader_uniform(int location, Vector2i value);
		void set_shader_uniform(int location, Vector2f value);
		void set_shader_uniform(int location, Vector3i value);
		void set_shader_uniform(int location, Vector3f value);
		void set_shader_uniform(int location, const Vector4i &value);
		void set_shader_uniform(int location, const Vector4f &value);
		
		// array uniform setters
		void set_shader_uniform(int location, int count, const int *value);
		void set_shader_uniform(int location, int count, const float *value);
		void set_shader_uniform(int location, int count, const Vector2i *value);
		void set_shader_uniform(int location, int count, const Vector2f *value);
		void set_shader_uniform(int location, int count, const Vector3i *value);
		void set_shader_uniform(int location, int count, const Vector3f *value);
		void set_shader_uniform(int location, int count, const Vector4i *value);
		void set_shader_uniform(int location, int count, const Vector4f *value);

	private:
		Canvas(const Window &wnd);
		Canvas(const Canvas &) = delete;
		Canvas(Canvas &&) = delete;
		void operator=(const Canvas &) = delete;
		void operator=(Canvas &&) = delete;

	private:
		const Window &m_wnd;

		std::shared_ptr<const Shader> m_shader;
		DrawType m_draw_type = DrawType::Drawing2D;
		ShaderUsage m_shading_usage = ShaderUsage::Usage2D;
		
		TextureId_t m_textures[int(TextureSlot::_MAX)];
		int m_active_textrues_count = 1; // will upload all textures from 0 to m_active_textrues_count - 1
		
		Transform2D m_transform2d{};
		Transform3D m_transform3d;

		Camera m_camera;
		struct CameraCache
		{
			Matrix4x4 m_proj_matrix;
		} m_camera_cache;
	};

	typedef void(*DrawCallback)(Canvas &canvas);

}
