#include "_iglib_shader.h"
#include "pch.h"
#include "draw_internal.h"

//Shader g_Default;

FORCEINLINE Shader::Subshader gen_subshader(const std::string &src, const SubshaderType type)
{
	SubshaderId_t id = glCreateShader(to_glshader_type(type));
	const char *cstr = src.c_str();

	glShaderSource(id, 1, &cstr, NULL);
	glCompileShader(id);

	int report_code, report_str_len;
	glGetShaderiv(id, GL_COMPILE_STATUS, &report_code);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &report_str_len);

	std::unique_ptr<char[]> msg(new char[ report_str_len ]);
	glGetShaderInfoLog(id, report_str_len, NULL, msg.get());

	std::cout << "shader id " << id << " reported: " << report_code << '\n';

	return Shader::Subshader{ type, id, src, Report{ report_code, std::string(msg.get()) } };
}

FORCEINLINE Report gen_shader(ShaderId_t id, const Shader::Subshader &vertex, const Shader::Subshader &fragment)
{
	// error reporting!
	
	if (vertex.log.code)
	{

	}

	if (fragment.log.code)
	{

	}

	glAttachShader(id, fragment.id);
	glAttachShader(id, vertex.id);

	int success;
	constexpr int log_length = 512;
	char msg[ log_length ];
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	glGetProgramInfoLog(id, log_length, NULL, msg);

	return { success, success ? std::string() : std::string(msg) };
}

namespace ig
{
	Shader::Shader(const std::string &vert_src, const std::string &frag_src)
		: m_id{ glCreateProgram() },
			m_v{ gen_subshader(vert_src, SubshaderType::Vertex) }, m_f{ gen_subshader(frag_src, SubshaderType::Fragment) },
			m_log{ gen_shader(m_id, m_v, m_f) }
	{
	}

	Shader::~Shader()
	{
		// TODO: add ability to make program id span multible object refs
		glDeleteProgram(m_id);
	}


	ShaderId_t Shader::get_id() const noexcept
	{
		return m_id;
	}

	bool Shader::is_valid() const noexcept
	{
		return m_log.code;
	}

	const Report &Shader::get_log() const noexcept
	{
		return m_log;
	}

	const Shader::Subshader &Shader::get_subshader(const SubshaderType type) const noexcept
	{
		if (type == SubshaderType::Fragment)
			return m_f;
		return m_v;
	}

	bool Shader::_is_current() const
	{
		int p;
		glGetIntegerv(GL_CURRENT_PROGRAM, &p);
		return p == m_id;
	}

	Shader::Subshader::~Subshader()
	{
		glDeleteShader(id);
	}
}
