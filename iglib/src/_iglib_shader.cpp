#include "_iglib_shader.h"
#include "pch.h"
#include "draw_internal.h"

constexpr int DefaultShaderVersion = 330;

enum class CustomCodeState
{
	None,
	MainBody,
	AfterUniforms,
	AfterVersion,
	Whole
};

struct ShaderTemplate
{
	int version = 440; // compat versions are negative
	GLuint type;
	ShaderUsage usage;
	struct {
		CustomCodeState state = CustomCodeState::None;
		const char *src;
	} custom_code;
};

//static Shader g_Default( VertexDefaultSrc2D, FragmentDefaultSrc2D );

//GLuint compile_template_shader(const std::string &src, GLenum type, const ShaderTemplate &t)
//{
//	
//}

FORCEINLINE std::string generate_shader_code( const ShaderTemplate &temp ) {
	if (temp.custom_code.state == CustomCodeState::Whole)
		return temp.custom_code.src;

	std::ostringstream ss{};

	ss << "#version " << std::abs( temp.version );
	if (temp.version < 0)
	{
		ss << ' ' << "compatibly";
	}
	ss << '\n';

	if (temp.custom_code.state == CustomCodeState::AfterVersion)
	{
		ss << temp.custom_code.src;
		return ss.str();
	}

	if (temp.type == GL_VERTEX_SHADER)
	{
		if (temp.usage == ShaderUsage::Usage3D)
		{
			ss << "layout (location = 0) in vec3 pos;";
		}
		else
		{
			ss << "layout (location = 0) in vec2 pos;";
		}

		ss << "layout (location = 1) in vec4 clr;"
			"layout (location = 2) in vec2 texcoord;";

		if (temp.usage == ShaderUsage::Usage3D)
			ss << "layout (location = 3) in vec3 normal;";

		ss << "out vec4 FragColor;";
		ss << "out vec2 UV;";

		//if (temp.fragtime)
		//	ss << "out float FragTime;";


		ss << "uniform vec2 _screensize;"
			"uniform vec2 _time;";


		if (temp.usage == ShaderUsage::Usage3D)
		{
			ss << "uniform mat3 _trans;";
			ss << "uniform vec3 _offset;";
			ss << "uniform mat4 _proj;";
			ss << "uniform mat3 _view_transform;";
			ss << "uniform vec3 _view_position;";
		}
		else if (temp.usage == ShaderUsage::Usage2D)
		{
			ss << "uniform mat2 _trans;";
			ss << "uniform vec2 _offset;";
		}

		ss << "\nvec2 to_native_space(in vec2 p) { return vec2(p.x / _screensize.x, 1.0 - (p.y / _screensize.y)) * 2.0 - vec2(1.0); }\n";


		if (temp.custom_code.state == CustomCodeState::AfterUniforms)
		{
			ss << temp.custom_code.src;
			return ss.str();
		}

		ss << "void main() {";


		if (temp.custom_code.state == CustomCodeState::MainBody)
		{
			ss << temp.custom_code.src << '}';
			return ss.str();
		}


		if (temp.usage != ShaderUsage::ScreenSpace)
		{

			if (temp.usage == ShaderUsage::Usage3D)
			{

				ss << "gl_Position = _proj * vec4((pos * _trans + _offset + _view_position) * inverse(_view_transform), 1.0);";
				/*ss << "vec2 tpos = ";
				ss << "pos;";
				ss << "gl_Position = vec4(vec2(tpos.x / _screensize.x, 1.0 - (tpos.y / _screensize.y)) * 2.0 - vec2(1.0), 0.0, 1.0);";*/
			}
			else
			{
				ss << "vec2 tpos = ";
				ss << "(pos * _trans) + _offset;";
				ss << "gl_Position = vec4(vec2(tpos.x / _screensize.x, 1.0 - (tpos.y / _screensize.y)) * 2.0 - vec2(1.0), 0.0, 1.0);";
			}
		}
		else
		{
			ss << "gl_Position = vec4(vec2(pos.x / _screensize.x, 1.0 - (pos.y / _screensize.y)) * 2.0 - vec2(1.0), 0.0, 1.0);";
		}

		//if (temp.usage == ShaderUsage::Usage2D)
		//	ss << "FragColor = vec4(_trans[0].xyz, 1.0) * clr;";
		//else
		ss << "FragColor = clr;";
		ss << "UV = texcoord;";

		//if (temp.fragtime)
		//{
		//	ss << "FragTime = _time;";
		//}

		ss << "}";
	}
	else if (temp.type == GL_FRAGMENT_SHADER)
	{
		ss << "layout (location = 0) out vec4 Color;"
			"layout (location = 1) out vec4 OverColor;"
			"in vec4 FragColor;"
			"in vec2 UV;";

		ss << "uniform sampler2D uTex0;";
		ss << "uniform sampler2D uTex1;";
		ss << "uniform sampler2D uTex2;";
		ss << "uniform sampler2D uTex3;";
		//if (temp.fragtime)
		//{
		//	ss << "in float FragTime;";
		//}

		if (temp.custom_code.state == CustomCodeState::AfterUniforms)
		{
			ss << temp.custom_code.src;
			return ss.str();
		}

		ss << "void main() {";

		if (temp.custom_code.state == CustomCodeState::MainBody)
		{
			ss << temp.custom_code.src << '}';
			return ss.str();
		}

		//ss << "Color = (texture(_tex, UV) * FragColor) - ((texture(_tex, UV) * FragColor) * UV.x) + (FragColor * UV.x);";
		//if (temp.usage == ShaderUsage::Usage3D)
		//	ss << "Color = vec4(UV, 1.0, 1.0);";
		//else
		ss << "Color = texture(uTex0, UV) * FragColor;";
		ss << "OverColor.rgb = vec3(1.0);";
		ss << "OverColor.a = 1.0;";
		//ss << "Color = vec4(UV, 1.0, 1.0) * FragColor;";
		//ss << "Color = texture(uTex0, UV) * FragColor;";
		//ss << "Color = vec4(1.0, 1.0, 1.0, 1.0);";
		ss << "}";
	}

	return ss.str();
}

GLuint compile_shaders( const std::string &shader, GLenum type ) {

	const char *shaderCode = shader.c_str();
	GLuint shaderId = glCreateShader( type );

	if (shaderId == 0)
	{ // Error: Cannot create shader object
		std::cout << "Error creating shaders";
		return 0;
	}

	// Attach source code to this object
	glShaderSource( shaderId, 1, &shaderCode, NULL );
	glCompileShader( shaderId ); // compile the shader object

	GLint compileStatus;

	// check for compilation status
	glGetShaderiv( shaderId, GL_COMPILE_STATUS, &compileStatus );

	if (!compileStatus)
	{ // If compilation was not successful
		int length;
		glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &length );
		char *cMessage = new char[ length ];

		// Get additional information
		glGetShaderInfoLog( shaderId, length, &length, cMessage );
		std::cout << "Cannot Compile Shader: " << cMessage;
		delete[] cMessage;
		glDeleteShader( shaderId );
		return 0;
	}

	return shaderId;
}

FORCEINLINE GLuint gen_shader( const std::string &src, const GLuint type ) {
	GLuint id = glCreateShader( type );
	const char *cstr = src.c_str();

	ASSERT( id != NULL );

	glShaderSource( id, 1, &cstr, NULL );
	glCompileShader( id );

	GLint compileStatus;

	// check for compilation status
	glGetShaderiv( id, GL_COMPILE_STATUS, &compileStatus );

	if (!compileStatus)
	{ // If compilation was not successful
		int length;
		glGetShaderiv( id, GL_INFO_LOG_LENGTH, &length );
		char *cMessage = new char[ length ];

		// Get additional information
		glGetShaderInfoLog( id, length, &length, cMessage );
		std::cerr << "Shader Error: " << cMessage << '\n';
		delete[] cMessage;
		glDeleteShader( id );
		return 0;
	}
	//std::cout << "shader id " << id << " reported: " << success << '\n';

	return id;
}

FORCEINLINE ShaderId_t gen_program( const GLuint vertex, const GLuint fragment ) {
	if (!vertex || !fragment)
	{
		if (vertex)
			glDeleteShader( vertex );
		else if (fragment)
			glDeleteShader( fragment );

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

	glAttachShader( id, vertex );
	glAttachShader( id, fragment );

	glLinkProgram( id );

	glDetachShader( id, vertex );
	glDetachShader( id, fragment );

	glDeleteShader( vertex );
	glDeleteShader( fragment );

	int success;
	constexpr int log_length = 512;
	int log_ml;
	char msg[ log_length ]{};
	glGetProgramiv( id, GL_LINK_STATUS, &success );
	glGetProgramInfoLog( id, log_length, &log_ml, msg );

	if (success == GL_FALSE)
	{
		glDeleteProgram( id );
		std::cerr << "shader program error: " << msg << '\n';
		return 0;
	}

	return id;
}

static inline ShaderId_t generate_shader_prog( const char *vertex, const char *fragment, const ShaderUsage usage ) {

	const std::string vert = generate_shader_code( { DefaultShaderVersion,
																						 GL_VERTEX_SHADER,
																						 usage,
																						 { vertex ? CustomCodeState::AfterUniforms : CustomCodeState::None, vertex } } );

	const std::string frag = generate_shader_code( { DefaultShaderVersion,
																						 GL_FRAGMENT_SHADER,
																						 usage,
																						 { fragment ? CustomCodeState::AfterUniforms : CustomCodeState::None, fragment } } );
	return gen_program( gen_shader( vert, GL_VERTEX_SHADER ),
											gen_shader( frag, GL_FRAGMENT_SHADER ) );
}

namespace ig
{

	ShaderInstance_t Shader::get_default( ShaderUsage usage ) {
		struct
		{
			bool inited = false;
			std::string vertex, fragment;
		} static cache_defaults[ (int)ShaderUsage::_Max ]{};

		if (!cache_defaults[ (int)usage ].inited)
		{
			cache_defaults[ (int)usage ].vertex = generate_shader_code( { DefaultShaderVersion, GL_VERTEX_SHADER, usage } );
			cache_defaults[ (int)usage ].fragment = generate_shader_code( { DefaultShaderVersion, GL_FRAGMENT_SHADER, usage } );
			cache_defaults[ (int)usage ].inited = true;
		}

		return compile_raw(
			cache_defaults[ (int)usage ].vertex,
			cache_defaults[ (int)usage ].fragment,
			usage
		);
	}

	ShaderInstance_t Shader::compile( const std::string &vertex_src, const std::string &fragment_src, ShaderUsage usage ) {
		return compile_raw(
			generate_shader_code( {
					DefaultShaderVersion,
					GL_VERTEX_SHADER,
					usage,
					{ CustomCodeState::AfterUniforms, vertex_src.c_str() }
														} ),
			generate_shader_code( {
					DefaultShaderVersion,
					GL_FRAGMENT_SHADER,
					usage,
					{ CustomCodeState::AfterUniforms, fragment_src.c_str() }
														} ),
			usage
		);
	}

	ShaderInstance_t Shader::compile_raw( const std::string &vertex_src, const std::string &fragment_src, ShaderUsage usage ) {
		return ShaderInstance_t(
			new Shader(
				gen_program( gen_shader( vertex_src, GL_VERTEX_SHADER ),
										 gen_shader( fragment_src, GL_FRAGMENT_SHADER ) ),
				usage )
		);
	}

	Shader::Shader()
		: m_id{ 0 }, m_usage{ ShaderUsage::Usage3D } {
	}

	Shader::Shader( const char *vertex, const char *fragment, ShaderUsage usage )
		: m_id{ generate_shader_prog( vertex, fragment, usage ) }, m_usage{ usage } {
	}

	Shader::Shader( ShaderId_t id, ShaderUsage usage )
		: m_id{ id }, m_usage{ usage } {
	}

	Shader::~Shader() {
		if (m_id)
			glDeleteProgram( m_id );
	}

	ShaderId_t Shader::get_id() const noexcept {
		return m_id;
	}

	bool Shader::is_valid() const noexcept {
		return m_id;
	}

	bool Shader::_is_current() const {
		GLuint p;
		glGetIntegerv( GL_CURRENT_PROGRAM, (GLint *)&p );
		return p == m_id;
	}

	ShaderUsage Shader::get_usage() const noexcept {
		return m_usage;
	}

	_NODISCARD int Shader::get_uniform_location( const std::string &name ) const noexcept {
		return glGetUniformLocation( get_id(), name.c_str() );
	}

	int Shader::max_uniform_location() {
		int result;
		glGetIntegerv( GL_MAX_UNIFORM_LOCATIONS, &result );
		return result;
	}
}
