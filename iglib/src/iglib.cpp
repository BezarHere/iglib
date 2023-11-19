#include "pch.h"
#include "iglib.h"
#include "internal.h"

constexpr Version CurrentVersion{ 1, 1, 0 };

namespace ig
{

	Error init()
	{

		//PEEK(init_glew);



		//GLuint pid = glCreateProgram();

		//const char *vsSrc =
		//	"#version 330 core\nlayout (location = 0) in vec3 inPosition;void main() {gl_Position = vec4(inPosition, 1.0);}";

		//GLuint vertexsh = glCreateShader(GL_VERTEX_SHADER);
		//glShaderSource(vertexsh, 1, &vsSrc, nullptr);
		//glCompileShader(vertexsh);


		/*GLfloat verts[]
		{
			0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f
		};

		GLfloat colors[]
		{
			1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
		};

		GLuint vbo{ 0 };
		GLuint vao{ 0 };

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6, verts, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

		glBindBuffer(0, vbo);
		glBindVertexArray(0);

		while (!glfwWindowShouldClose(window))
		{
			glClearColor(0.0f, 0.0f, 0.7f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}*/

		return Error();
	}

	Version IGlib::version() noexcept
	{
		return CurrentVersion;
	}

	
	std::string get_opengl_version()
	{
		auto c = glGetString(GL_VERSION);
		// glew not init
		if (!c)
		{
			warn("start ig::Application to get the opengl version\n");
			return std::string();
		}

		return std::string((const char *)c);
	}

}
