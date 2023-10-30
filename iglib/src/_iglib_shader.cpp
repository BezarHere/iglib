#include "_iglib_shader.h"
#include "pch.h"
#include "draw_internal.h"

constexpr auto VertexDefaultSrc =
"#version 330 core\n"
"layout (location = 0) in vec2 pos;\n"
"layout (location = 1) in vec4 clr;\n"
"layout (location = 2) in vec2 uv;\n"
"layout (location = 3) in vec2 normal;\n"
"out vec4 out_color;\n"
"uniform vec2 screen_size;\n"
"void main() {\n"
"vec2 native_pos = vec2(pos.x / screen_size.x, 1.0 - (pos.y / screen_size.y)) * 2.0 - vec2(1.0);"
"gl_Position = vec4(native_pos, 0.0, 1.0);\n"
//"out_color = vec4(1.0, 1.0, 0.0, 1.0);\n"
//"out_color = clr;\n"
"out_color = vec4(uv, 0.0, 1.0) * clr;\n"
"}\n"
;

constexpr auto FragmentDefaultSrc =
"#version 330 core\n"
"out vec4 fColor;\n"
"in vec4 vColor;\n"
"void main() {\n"
"fColor = vColor;\n"
"}\n"
;

//static Shader g_Default( VertexDefaultSrc, FragmentDefaultSrc );

GLuint compileShaders(std::string shader, GLenum type)
{

	const char *shaderCode = shader.c_str();
	GLuint shaderId = glCreateShader(type);

	if (shaderId == 0) { // Error: Cannot create shader object
		std::cout << "Error creating shaders";
		return 0;
	}

	// Attach source code to this object
	glShaderSource(shaderId, 1, &shaderCode, NULL);
	glCompileShader(shaderId); // compile the shader object

	GLint compileStatus;

	// check for compilation status
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

	if (!compileStatus) { // If compilation was not successful
		int length;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
		char *cMessage = new char[ length ];

		// Get additional information
		glGetShaderInfoLog(shaderId, length, &length, cMessage);
		std::cout << "Cannot Compile Shader: " << cMessage;
		delete[] cMessage;
		glDeleteShader(shaderId);
		return 0;
	}

	return shaderId;
}

FORCEINLINE GLuint gen_shader(const std::string &src, const GLuint type)
{
	GLuint id = glCreateShader(type);
	const char *cstr = src.c_str();

	assert(id != NULL);

	glShaderSource(id, 1, &cstr, NULL);
	glCompileShader(id);

	GLint compileStatus;

	// check for compilation status
	glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);

	if (!compileStatus) { // If compilation was not successful
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *cMessage = new char[ length ];

		// Get additional information
		glGetShaderInfoLog(id, length, &length, cMessage);
		std::cerr << "Shader Error: " << cMessage << '\n';
		delete[] cMessage;
		glDeleteShader(id);
		return 0;
	}
	//std::cout << "shader id " << id << " reported: " << success << '\n';

	return id;
}

FORCEINLINE ShaderId_t gen_program(const GLuint vertex, const GLuint fragment)
{
	if (!vertex || !fragment)
	{
		if (vertex)
			glDeleteShader(vertex);
		else if (fragment)
			glDeleteShader(fragment);

		return 0;
	}


	ShaderId_t id = glCreateProgram();

	if (id == NULL)
	{
		std::cerr << "couldn't create shader program\n";
		return 0;
	}
	
	//if (!vertex.log.code)
	//{
	//	raise("vertex subshader: " + vertex.log.msg);
	//}
	//


	//if (!fragment.log.code)
	//{
	//	raise("fragment subshader: " + fragment.log.msg);
	//}

	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	
	glLinkProgram(id);

	glDetachShader(id, vertex);
	glDetachShader(id, fragment);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	int success;
	constexpr int log_length = 512;
	int log_ml;
	char msg[ log_length ]{};
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	glGetProgramInfoLog(id, log_length, &log_ml, msg);
	
	if (success == GL_FALSE)
	{
		glDeleteProgram(id);
		std::cerr << "shader program error: " << msg << '\n';
		return 0;
	}

	return id;
}

namespace ig
{
	std::shared_ptr<Shader> Shader::get_default()
	{
		return create(VertexDefaultSrc, FragmentDefaultSrc);
	}

	std::shared_ptr<Shader> Shader::create(const std::string &vertex_src, const std::string &fragment_src)
	{
		return std::shared_ptr<Shader>(new Shader(gen_program(gen_shader(vertex_src, GL_VERTEX_SHADER), gen_shader(fragment_src, GL_FRAGMENT_SHADER))));
	}

	Shader::Shader()
		: m_id{ 0 }
	{
	}

	Shader::Shader(ShaderId_t id)
		: m_id{ id }
	{
	}

	Shader::~Shader()
	{
		if (m_id)
			glDeleteProgram(m_id);
	}

	ShaderId_t Shader::get_id() const noexcept
	{
		return m_id;
	}

	bool Shader::is_valid() const noexcept
	{
		return m_id;
	}

	bool Shader::_is_current() const
	{
		GLuint p;
		glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *) & p);
		return p == m_id;
	}


}
