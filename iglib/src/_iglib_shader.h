#pragma once
#include "_iglib_base.h"

namespace ig
{
	typedef uint32_t ShaderId_t;

	enum class ShaderUsage
	{
		Usage2D,
		Usage3D,
		ScreenSpace, // <- isn't transformable but it's very light
		_Max
	};

	class Shader;
	typedef std::shared_ptr<Shader> ShaderInstance_t;
	class Shader
	{
	public:
		// WILL CREATE AN INVALID SHADER
		Shader();
		~Shader();

		/// @warn THIS WILL REBUILD A NEW 'DEFAULT' SHADER, WICH IS EXPENSIVE
		static ShaderInstance_t get_default(ShaderUsage usage = ShaderUsage::Usage3D);
		static ShaderInstance_t compile(const std::string &vertex_src, const std::string &fragment_src, ShaderUsage usage = ShaderUsage::Usage3D);

		// will create a raw shader, it's hard to be complaient with the current implementation so try to not use it
		static ShaderInstance_t compile_raw(const std::string &vertex_src, const std::string &fragment_src, ShaderUsage usage = ShaderUsage::Usage3D);
		

		ShaderId_t get_id() const noexcept;
		bool is_valid() const noexcept;

		bool _is_current() const;

		ShaderUsage get_usage() const noexcept;

		/// @return -1 on fail, uniform location on success
		_NODISCARD int get_uniform_location(const std::string &name) const noexcept;
		_NODISCARD static int max_uniform_location();

		static_assert(sizeof(unsigned) == sizeof(int), "Funky OS Error: unsigned and int aren't the same size");

	private:
		Shader(ShaderId_t id, ShaderUsage usage);
		Shader(const Shader &copy) = delete;
		Shader(Shader &&move) = delete;
		Shader &operator=(const Shader &copy) = delete;
		Shader &operator=(Shader &&move) = delete;
	private:
		const ShaderUsage m_usage;
		const ShaderId_t m_id;
	};
}

