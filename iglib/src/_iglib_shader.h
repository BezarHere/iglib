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
		//struct Shader
		//{
		//	ShaderType type;
		//	ShaderId_t id;
		//	std::string src;
		//	Report log;
		//};


	private:
		ShaderProgramId_t m_id;
		Report m_log[2];
	};
}

