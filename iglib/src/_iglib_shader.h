#pragma once
#include "_iglib_base.h"

namespace ig
{
	typedef uint32_t ShaderId_t;


	class Shader
	{
	public:
		// WILL CREATE AN INVALID SHADER
		Shader();
		~Shader();

		static std::shared_ptr<Shader> get_default();
		static std::shared_ptr<Shader> compile(const std::string &vertex_src, const std::string &fragment_src);

		// will create a raw shader, it's hard to be complaient with the current implementation so try to not use it
		static std::shared_ptr<Shader> compile_raw(const std::string &vertex_src, const std::string &fragment_src);
		

		ShaderId_t get_id() const noexcept;
		bool is_valid() const noexcept;

		bool _is_current() const;


	private:
		Shader(ShaderId_t id);
		Shader(const Shader &copy) = delete;
		Shader(Shader &&move) = delete;
		Shader &operator=(const Shader &copy) = delete;
		Shader &operator=(Shader &&move) = delete;
	private:
		const ShaderId_t m_id;
	};
}

