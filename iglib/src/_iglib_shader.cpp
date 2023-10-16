#include "_iglib_shader.h"
#include "pch.h"
#include "draw_internal.h"

//ShaderProgram g_Default;

FORCEINLINE ShaderProgram::Shader gen_shader(const std::string &src, const ShaderType type)
{
	ShaderId_t id = glCreateShader(to_glshader_type(type));
	const char *cstr = src.c_str();

	glShaderSource(id, 1, &cstr, NULL);
	glCompileShader(id);

	int report_code, report_str_len;
	glGetShaderiv(id, GL_COMPILE_STATUS, &report_code);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &report_str_len);

	char *msg = new char[ report_str_len ];
	glGetShaderInfoLog(id, report_str_len, NULL, msg);

	return ShaderProgram::Shader{ type, src, Report{ report_code, std::string(msg) } };
}

FORCEINLINE Report gen_program(ShaderProgramId_t id, const ShaderProgram::Shader &vert, const ShaderProgram::Shader &frag)
{

}

namespace ig
{
	ShaderProgram::ShaderProgram(const std::string &vert_src, const std::string &frag_src)
		: m_id{ glCreateProgram() },
			m_v{ gen_shader(vert_src, ShaderType::Vertex) }, m_f{ gen_shader(frag_src, ShaderType::Fragment) },
			m_log{  }
	{
	}


	ShaderProgramId_t ShaderProgram::get_id() const noexcept
	{
		return m_id;
	}
}
