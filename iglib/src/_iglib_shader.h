#pragma once
#include "_iglib_base.h"

namespace ig
{

	typedef uint32_t SubshaderId_t;
	typedef uint32_t ShaderId_t;

	enum class SubshaderType
	{
		Vertex,
		Fragment
	};

	class Shader
	{
	public:
		struct Subshader
		{
			~Subshader();

			const SubshaderType type;
			SubshaderId_t id;
			std::string src;
			Report log;
		};

		Shader();
		Shader(const std::string &vert_src, const std::string &frag_src);
		~Shader();

		ShaderId_t get_id() const noexcept;
		bool is_valid() const noexcept;
		const Report &get_log() const noexcept;

		const Subshader &get_subshader(const SubshaderType type) const noexcept;

		bool _is_current() const;

	private:
		const ShaderId_t m_id;
		Subshader m_v, m_f;
		Report m_log;
	};
}

