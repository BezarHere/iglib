#pragma once
#include "_iglib_base.h"

namespace ig
{

	typedef uint32_t ShaderId_t;
	typedef uint32_t ShaderProgramId_t;

	enum class ShaderType
	{
		Vertex,
		Fragment
	};

	class ShaderProgram
	{
	public:
		struct Shader
		{
			const ShaderType type;
			//ShaderId_t id;
			std::string src;
			Report log;
		};

		ShaderProgram(const std::string &vert_src, const std::string &frag_src);

		ShaderProgramId_t get_id() const noexcept;

	private:
		const ShaderProgramId_t m_id;
		Shader m_v, m_f;
		Report m_log;
	};
}

