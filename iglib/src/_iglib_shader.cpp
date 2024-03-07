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

static inline std::string get_default_shader_code( GLint type, ShaderUsage usage );

FORCEINLINE static std::string generate_shader_code( const ShaderTemplate &temp ) {
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
		ss << Shader::get_default_source_part( temp.usage, ShaderSourcePart::VertexInputs );
		ss << Shader::get_default_source_part( temp.usage, ShaderSourcePart::VertexOutputs );
		ss << Shader::get_default_source_part( temp.usage, ShaderSourcePart::VertexUniforms );
		ss << Shader::get_default_source_part( temp.usage, ShaderSourcePart::VertexUtilityMethods );

		if (temp.custom_code.state == CustomCodeState::AfterUniforms)
		{
			ss << temp.custom_code.src;
			return ss.str();
		}


		if (temp.custom_code.state == CustomCodeState::MainBody)
		{
			ss << "void main(){" << temp.custom_code.src << '}';
			return ss.str();
		}

		ss << Shader::get_default_source_part( temp.usage, ShaderSourcePart::VertexMain );

	}
	else if (temp.type == GL_FRAGMENT_SHADER)
	{
		ss << Shader::get_default_source_part( temp.usage, ShaderSourcePart::FragmentInputs );
		ss << Shader::get_default_source_part( temp.usage, ShaderSourcePart::FragmentOutputs );
		ss << Shader::get_default_source_part( temp.usage, ShaderSourcePart::FragmentUniforms );
		ss << Shader::get_default_source_part( temp.usage, ShaderSourcePart::FragmentUtilityMethods );

		if (temp.custom_code.state == CustomCodeState::AfterUniforms)
		{
			ss << temp.custom_code.src;
			return ss.str();
		}


		if (temp.custom_code.state == CustomCodeState::MainBody)
		{
			ss << "void main() {" << temp.custom_code.src << '}';
			return ss.str();
		}

		ss << Shader::get_default_source_part( temp.usage, ShaderSourcePart::FragmentMain );
	}

	return ss.str();
}

static GLuint compile_shaders( const std::string &shader, GLenum type ) {

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

FORCEINLINE static GLuint gen_shader( const char *psrc, const GLuint type, const ShaderUsage usage ) {
	GLuint id = glCreateShader( type );

	ASSERT( id != NULL );

	if (psrc)
	{
		glShaderSource( id, 1, &psrc, NULL );
	}
	else
	{
		const std::string default_src = get_default_shader_code( type, usage );
		psrc = default_src.c_str();
		glShaderSource( id, 1, &psrc, NULL );
		psrc = nullptr;
	}

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

FORCEINLINE static ShaderName gen_program( const GLuint vertex, const GLuint fragment ) {
	if (!vertex || !fragment)
	{
		if (vertex)
			glDeleteShader( vertex );
		else if (fragment)
			glDeleteShader( fragment );

		return 0;
	}


	ShaderName id = glCreateProgram();

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

static inline ShaderName generate_shader_prog( const char *vertex, const char *fragment, const ShaderUsage usage ) {

	const std::string vert = generate_shader_code(
		{
			DefaultShaderVersion,
			GL_VERTEX_SHADER,
			usage,
			{ vertex ? CustomCodeState::AfterUniforms : CustomCodeState::None, vertex }
		}
	);


	const std::string frag = generate_shader_code(
		{
			DefaultShaderVersion,
			GL_FRAGMENT_SHADER,
			usage,
			{ fragment ? CustomCodeState::AfterUniforms : CustomCodeState::None, fragment }
		}
	);

	const auto prog = gen_program(
		gen_shader( vert.c_str(), GL_VERTEX_SHADER, usage ),
		gen_shader( frag.c_str(), GL_FRAGMENT_SHADER, usage )
	);

	return prog;
}

static inline std::string get_default_shader_code( GLint type, ShaderUsage usage ) {
	std::ostringstream ss{};
	ss << "#version " << "330" << '\n';
	switch (type)
	{
	case GL_VERTEX_SHADER:
		{
			ss << Shader::get_default_source_part( usage, ShaderSourcePart::VertexInputs );
			ss << Shader::get_default_source_part( usage, ShaderSourcePart::VertexOutputs );
			ss << Shader::get_default_source_part( usage, ShaderSourcePart::VertexUniforms );
			ss << Shader::get_default_source_part( usage, ShaderSourcePart::VertexUtilityMethods );
			ss << Shader::get_default_source_part( usage, ShaderSourcePart::VertexMain );
		}
		break;
	case GL_FRAGMENT_SHADER:
		{
			ss << Shader::get_default_source_part( usage, ShaderSourcePart::FragmentInputs );
			ss << Shader::get_default_source_part( usage, ShaderSourcePart::FragmentOutputs );
			ss << Shader::get_default_source_part( usage, ShaderSourcePart::FragmentUniforms );
			ss << Shader::get_default_source_part( usage, ShaderSourcePart::FragmentUtilityMethods );
			ss << Shader::get_default_source_part( usage, ShaderSourcePart::FragmentMain );
		}
		break;
	default:
		break;
	}
	return ss.str();
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
			cache_defaults[ (int)usage ].vertex.c_str(),
			cache_defaults[ (int)usage ].fragment.c_str(),
			usage
		);
	}

	ShaderInstance_t Shader::compile( const char *vertex_src, const char *fragment_src, ShaderUsage usage ) {
		const std::string vertex_src_processed = generate_shader_code(
			{
					DefaultShaderVersion,
					GL_VERTEX_SHADER,
					usage,
					{ CustomCodeState::AfterUniforms, vertex_src }
			}
		);

		const std::string fragment_src_processed = generate_shader_code(
			{
					DefaultShaderVersion,
					GL_FRAGMENT_SHADER,
					usage,
					{ CustomCodeState::AfterUniforms, fragment_src }
			}
		);

		auto result = compile_raw(
			vertex_src_processed.c_str(),
			fragment_src_processed.c_str(),
			usage
		);

		return result;
	}

	ShaderInstance_t Shader::compile_raw( const char *vertex_src, const char *fragment_src, ShaderUsage usage ) {
		return ShaderInstance_t(
			new Shader(
				gen_program(
					gen_shader( vertex_src, GL_VERTEX_SHADER, usage ),
					gen_shader( fragment_src, GL_FRAGMENT_SHADER, usage )
				),
				usage
			)
		);
	}

	std::string Shader::get_default_source_part( ShaderUsage usage, ShaderSourcePart part ) {
		using SSP = ShaderSourcePart;


		std::ostringstream ss{};

		switch (part)
		{
		case SSP::VertexInputs:
			{

				if (usage == ShaderUsage::Usage3D)
				{
					ss << "layout (location = 0) in vec3 pos;";
				}
				else
				{
					ss << "layout (location = 0) in vec2 pos;";
				}

				ss << "layout (location = 1) in vec4 clr;"
					"layout (location = 2) in vec2 texcoord;";

				if (usage == ShaderUsage::Usage3D)
					ss << "layout (location = 3) in vec3 normal;";
			}
			break;
		case SSP::VertexOutputs:
			{
				ss << "out vec4 FragColor;";
				ss << "out vec2 UV;";
			}
			break;
		case SSP::VertexUniforms:
			{

				ss << "uniform vec2 _screensize;"
					"uniform float _time;";


				if (usage == ShaderUsage::Usage3D)
				{
					ss << "uniform mat3 _trans;";
					ss << "uniform vec3 _offset;";
					ss << "uniform mat4 _proj;";
					ss << "uniform mat3 _view_transform;";
					ss << "uniform vec3 _view_position;";
				}
				else if (usage == ShaderUsage::Usage2D)
				{
					ss << "uniform mat2 _trans;";
					ss << "uniform vec2 _offset;";
				}
			}
			break;
		case SSP::VertexUtilityMethods:
			{
				ss << "\nvec2 to_native_space(in vec2 p) { return vec2(p.x / _screensize.x, 1.0 - (p.y / _screensize.y)) * 2.0 - vec2(1.0); }\n";
			}
			break;
		case SSP::VertexMain:
			{
				ss << "void main() {";
				if (usage != ShaderUsage::ScreenSpace)
				{
					if (usage == ShaderUsage::Usage3D)
					{
						ss << "gl_Position = _proj * vec4((_trans * pos + _offset + _view_position) * inverse(_view_transform), 1.0);";
					}
					else
					{
						ss << "vec2 tpos = ";
						ss << "(_trans * pos) + _offset;";
						ss << "gl_Position = vec4(vec2(tpos.x / _screensize.x, 1.0 - (tpos.y / _screensize.y)) * 2.0 - vec2(1.0), 0.0, 1.0);";
					}
				}
				else
				{
					ss << "gl_Position = vec4(vec2(pos.x / _screensize.x, 1.0 - (pos.y / _screensize.y)) * 2.0 - vec2(1.0), 0.0, 1.0);";
				}

				ss << "FragColor = clr;";
				ss << "UV = texcoord;";

				ss << "}";
			}
			break;
		case SSP::FragmentInputs:
			{
				ss << "in vec4 FragColor;"
					"in vec2 UV;";
			}
			break;
		case SSP::FragmentOutputs:
			{
				ss << "layout (location = 0) out vec4 Color;"
					"layout (location = 1) out vec4 OverColor;";
			}
			break;
		case SSP::FragmentUniforms:
			{
				ss << "uniform sampler2D uTex0;";
				ss << "uniform sampler2D uTex1;";
				ss << "uniform sampler2D uTex2;";
				ss << "uniform sampler2D uTex3;";
			}
			break;
		case SSP::FragmentUtilityMethods:
			{

			}
			break;
		case SSP::FragmentMain:
			{
				ss << "void main() {";
				ss << "Color = texture(uTex0, UV) * FragColor;";
				ss << "OverColor.rgb = vec3(1.0);";
				ss << "OverColor.a = 1.0;";
				ss << "}";
			}
			break;
		default:
			break;
		}
		return ss.str();
	}

	Shader::Shader()
		: m_name{ 0 }, m_usage{ ShaderUsage::Usage3D } {
	}

	Shader::Shader( const char *vertex, const char *fragment, ShaderUsage usage )
		: m_name{ generate_shader_prog( vertex, fragment, usage ) }, m_usage{ usage } {
	}

	Shader::Shader( ShaderName id, ShaderUsage usage )
		: m_name{ id }, m_usage{ usage } {
	}

	Shader::~Shader() {
		if (m_name)
			glDeleteProgram( m_name );
	}

	ShaderName Shader::get_name() const noexcept {
		return m_name;
	}

	bool Shader::is_valid() const noexcept {
		return m_name != NULL;
	}

	void Shader::bind() const {
		glUseProgram( m_name );
	}

	void Shader::clear_bound() {
		glUseProgram( NULL );
	}

	ShaderName Shader::get_bound() {
		GLint value;
		glGetIntegerv( GL_CURRENT_PROGRAM, &value );
		return value;
	}

	bool Shader::is_current() const {
		return get_bound() == m_name;
	}

	ShaderUsage Shader::get_usage() const noexcept {
		return m_usage;
	}

	_NODISCARD int Shader::get_uniform_location( const std::string &name ) const noexcept {
		return glGetUniformLocation( get_name(), name.c_str() );
	}

	int Shader::max_uniform_location() {
		int result;
		glGetIntegerv( GL_MAX_UNIFORM_LOCATIONS, &result );
		return result;
	}
}
