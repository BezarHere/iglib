#pragma once
#include "_iglib_base.h"

namespace ig
{
	typedef uint32_t ShaderName;

	enum class ShaderUsage
	{
		Usage2D,
		Usage3D,
		ScreenSpace, // <- isn't transformable but it's very light
		_Max
	};

	enum class ShaderSourcePart
	{
		VertexInputs,
		VertexOutputs,
		VertexUniforms,
		VertexUtilityMethods,
		VertexMain,
		FragmentInputs,
		FragmentOutputs,
		FragmentUniforms,
		FragmentUtilityMethods,
		FragmentMain
	};

	class Shader;
	typedef std::unique_ptr<Shader> ShaderInstance_t;
	class Shader
	{
	public:
		/// @brief Creates an invalid shader, use other constructors to create a valid shader
		Shader();
		/// @brief compiles shader from source
		/// @param vertex the vertex shader source (nullptr to use the default vertex shader)
		/// @param fragment the fragment shader source (nullptr to use the default fragment shader)
		/// @param usage the shader usage (for optimization)
		Shader( const char *vertex, const char *fragment, ShaderUsage usage = ShaderUsage::Usage3D );

		~Shader();

		/// @warn THIS WILL REBUILD A NEW 'DEFAULT' SHADER, WHICH IS EXPENSIVE
		static ShaderInstance_t get_default( ShaderUsage usage = ShaderUsage::Usage3D );
		static ShaderInstance_t compile( const char *vertex_src, const char *fragment_src, ShaderUsage usage = ShaderUsage::Usage3D );

		static ShaderInstance_t compile_raw( const char *vertex_src, const char *fragment_src, ShaderUsage usage = ShaderUsage::Usage3D );

		static std::string get_default_source_part( ShaderUsage usage, ShaderSourcePart part );

		ShaderName get_name() const noexcept;
		bool is_valid() const noexcept;
		bool is_current() const;

		void bind() const;

		static void clear_bound();
		static ShaderName get_bound();

		ShaderUsage get_usage() const noexcept;

		/// @return -1 on fail, uniform location on success
		_NODISCARD int get_uniform_location( const std::string &name ) const noexcept;
		_NODISCARD static int max_uniform_location();

		static_assert(sizeof( unsigned ) == sizeof( int ), "Funky OS Error: unsigned and int aren't the same size");

	private:
		Shader( ShaderName id, ShaderUsage usage );
		Shader( const Shader &copy ) = delete;
		Shader( Shader &&move ) = delete;
		Shader &operator=( const Shader &copy ) = delete;
		Shader &operator=( Shader &&move ) = delete;
	private:
		const ShaderUsage m_usage;
		const ShaderName m_name;
	};
}

