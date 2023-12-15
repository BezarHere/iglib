#pragma once
#include "_iglib_base.h"
#include "_iglib_color.h"
#include "_iglib_vector.h"
#include "_iglib_window.h"

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

	// VALUES COPIED FROM GL.H
	enum class CullWinding
	{
		CW = 0x900,
		CCW = 0x901
	};

	// VALUES COPIED FROM GL.H
	enum class CullFace
	{
		Front = 0x404,
		Back = 0x405,
		FrontAndBack = 0x408
	};

	// VALUES COPIED FROM GL.H
	enum class Feature
	{
		DepthTest = 0xb71,
		Culling = 0xb44
	};

	struct RenderEnviorment
	{
		bool enabled_postprocessing = true;
		Colorf ambient_light = { 1.f, 1.f, 1.f };
		bool hdr = true;
	};

	typedef void(*RenderCallback)(Renderer &renderer);

	class Renderer
	{
	public:
		Renderer( const Window &window, RenderCallback render_callback = nullptr );

		void clear();

		/// @brief binds this renderer to the draw pipeline
		/// @param canvas pointer to a canvas instance
		/// @return wheather it's bound to the draw pipeline successfuly
		bool draw();

		void set_callback( RenderCallback callback );
		RenderCallback get_callback() const;

		Canvas &get_canvas();

		bool is_active() const;

		const Window &get_window() const;

		// what will the canvas expect to draw in later calls? and what default shader will it resort to?
		void set_draw_type( const DrawType type );

		void bind_shader( const ShaderInstance_t &shader );
		void bind_default_shader(ShaderUsage usage);
		void unbind_shader();

		void set_cullwinding( CullWinding winding );
		void set_cullface( CullFace face );

		void enable_feature( Feature feature );
		void disable_feature( Feature feature );

		ShaderId_t get_shader_id() const noexcept;

		void bind_texture( const TextureId_t tex, const TextureSlot slot = TextureSlot::Slot0 );
		TextureId_t get_texture( const TextureSlot slot = TextureSlot::Slot0 ) const noexcept;

		void set_active_textures_count( int count );
		int get_active_textures_count() const noexcept;

		void set_enviorment( const RenderEnviorment &env );
		const RenderEnviorment &get_enviorment();

		void try_update_shader_state();

		/// \note used to set samplers slots
		void set_shader_uniform( int location, int value );
		void set_shader_uniform( int location, float value );
		void set_shader_uniform( int location, Vector2i value );
		void set_shader_uniform( int location, Vector2f value );
		void set_shader_uniform( int location, Vector3i value );
		void set_shader_uniform( int location, Vector3f value );
		void set_shader_uniform( int location, const Vector4i &value );
		void set_shader_uniform( int location, const Vector4f &value );

		// array uniform setters
		void set_shader_uniform( int location, int count, const int *value );
		void set_shader_uniform( int location, int count, const float *value );
		void set_shader_uniform( int location, int count, const Vector2i *value );
		void set_shader_uniform( int location, int count, const Vector2f *value );
		void set_shader_uniform( int location, int count, const Vector3i *value );
		void set_shader_uniform( int location, int count, const Vector3f *value );
		void set_shader_uniform( int location, int count, const Vector4i *value );
		void set_shader_uniform( int location, int count, const Vector4f *value );

	private:
		Renderer( const Renderer & ) = delete;
		Renderer( Renderer && ) = delete;
		void operator=( const Renderer & ) = delete;
		void operator=( Renderer && ) = delete;

	private:
		struct RenderBuffersState
		{
			RenderBuffersState();
			~RenderBuffersState();
			RenderBuffersState( const RenderBuffersState & ) = delete;
			RenderBuffersState( RenderBuffersState && ) = delete;
			void operator=( const RenderBuffersState & ) = delete;
			void operator=( RenderBuffersState && ) = delete;

			unsigned int framebuffer_object = 0, renderbuffer_object = 0, colorbuffer_object = 0;
			Vector2i colorbuffer_size{};

			struct Regenarator;
		};

		const Window &m_window;
		Colorf m_background_clr = Colorf( 0.f, 0.f, 0.f );
		RenderBuffersState m_buffers_state;
		RenderEnviorment m_enviorment;
		
		RenderCallback m_callback;

		struct RenderState
		{
			bool m_dirty = false;

			std::shared_ptr<const Shader> bound_shader;
			DrawType draw_type = DrawType::Drawing2D;
			ShaderUsage shading_usage = ShaderUsage::Usage2D;

			TextureId_t textures[ int( TextureSlot::_MAX ) ];
			int active_textrues_count = 1; // will upload all textures from 0 to m_active_textrues_count - 1


		} m_state;
		Canvas m_canvas;

	};

}
