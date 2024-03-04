#pragma once
#include "_iglib_base.h"
#include "_iglib_color.h"
#include "_iglib_framebuffer.h"
#include "_iglib_renderbuffer.h"
#include "_iglib_vector.h"
#include "_iglib_window.h"

namespace ig
{
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

	// VALUES COPIED FROM GLEW.H
	enum class DepthTestComparison
	{
		// drawing never passes the depth test
		Never = 0x200,
		LessThen = 0x201,
		EqualTo = 0x202,
		LessThenEqual = 0x203,
		GreaterThen = 0x204,
		NotEqual = 0x205,
		GreaterThenEqual = 0x206,
		// drawing always passes the depth test
		Always = 0x207,
	};

	enum class VertexIndexType
	{
		Byte = 0x1401,
		Short = 0x1403,
		Int = 0x1405,
	};

	struct RenderEnvironment
	{
		bool enabled_postprocessing = true;
		Colorf ambient_light = { 1.f, 1.f, 1.f };
		bool hdr = true;
	};

	class Renderer;

	typedef void(*RenderCallback)(Renderer &renderer);

	class Renderer
	{
	public:
		Renderer( const Window &window, RenderCallback render_callback = nullptr );

		void clear();

		/// @brief binds this renderer to the draw pipeline
		/// @param canvas pointer to a canvas instance
		/// @return weather it's bound to the draw pipeline successfully
		bool draw();

		void set_callback( RenderCallback callback );
		RenderCallback get_callback() const;

		Canvas &get_canvas();

		bool is_active() const;

		const Window &get_window() const;

		// what will the canvas expect to draw in later calls? and what default shader will it resort to?
		void set_draw_type( const DrawType type );

		void bind_shader( const ShaderInstance_t &shader );
		void bind_default_shader( ShaderUsage usage );
		void unbind_shader();

		void set_cullwinding( CullWinding winding );
		void set_cullface( CullFace face );

		void enable_feature( Feature feature );
		void disable_feature( Feature feature );
		bool is_feature_enabled( Feature feature ) const;

		void set_depth_test_comparison( DepthTestComparison comparison );
		DepthTestComparison get_depth_test_comparison() const;

		ShaderId_t get_shader_id() const noexcept;

		void bind_texture( const TextureId tex, const TextureSlot slot = TextureSlot::Slot0 );
		TextureId get_texture( const TextureSlot slot = TextureSlot::Slot0 ) const noexcept;

		void set_active_textures_count( int count );
		int get_active_textures_count() const noexcept;

		void set_environment( const RenderEnvironment &env );
		const RenderEnvironment &get_environment();

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

		/// @brief executes a draw call using the bound objects (X.bind()/unbind())
		/// @param primitive the primitive to be constructed
		/// @param begin the index of the first vertex
		/// @param count the vertex count (end = begin + count)
		static void render( PrimitiveType primitive, int begin, int count );


		/// @brief executes an indexed draw call using the bound objects (X.bind()/unbind()), clever indexed draw calls can be more performant
		/// @param primitive the primitive to be constructed
		/// @param begin the index of the first vertex
		/// @param vertex_index_type the vertex index type...
		/// @param index_offset the offset of the first index to be used
		static void render( PrimitiveType primitive, int count, VertexIndexType vertex_index_type, unsigned index_offset );

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

			FramebufferId framebuffer_object = 0;
			RenderbufferId renderbuffer_object = 0;
			std::array<TextureId, 2> color_buffers = { 0 };
			Vector2i color_buffer_size{};

			struct Regenerator;
		};

		const Window &m_window;
		Colorf m_background_clr = Colorf( 0.f, 0.f, 0.f );
		RenderBuffersState m_buffers_state;
		RenderEnvironment m_environment;

		RenderCallback m_callback;

		struct RenderState
		{
			bool m_dirty = false;

			std::shared_ptr<const Shader> bound_shader;
			DrawType draw_type = DrawType::Drawing2D;
			ShaderUsage shading_usage = ShaderUsage::Usage2D;

			TextureId textures[ int( TextureSlot::_MAX ) ];
			int active_textures_count = 1; // will upload all textures from 0 to m_active_textures_count - 1


		} m_state;
		Canvas m_active_canvas;
	};

}
