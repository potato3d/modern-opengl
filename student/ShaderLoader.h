#pragma once
#include <GL/glew.h>
#include <fstream>
#include <iostream>

class ShaderLoader
{
public:
	ShaderLoader()
	{
		_program = 0;
	}

	bool addFile(GLenum shaderType, const char* shaderFilename, const char* dataFilename = nullptr, int glslVersion = 450, const char* extensions = nullptr)
	{
		std::string line;
		std::string source;

		// add version string
		source += "#version " + std::to_string(glslVersion) + "\n";

		// add extensions
		if(extensions)
		{
			source += std::string(extensions) + "\n";
		}

		// load shader data
		if(dataFilename)
		{
			std::ifstream bindings(dataFilename);
			if(!bindings)
			{
				return false;
			}

			while(std::getline(bindings, line))
			{
				source += line + "\n";
			}
		}

		// load given file
		std::ifstream file(shaderFilename);
		if(!file)
		{
			return false;
		}

		while(std::getline(file, line))
		{
			source += line + "\n";
		}

		// Create and compile shader in OpenGL
		GLuint shader = glCreateShader(shaderType);

		const GLchar* contents = source.data();

		glShaderSource(shader, 1, &contents, nullptr);
		glCompileShader(shader);

		// Check compilation
		int ok = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

		if(ok == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			char* infoLog = new char[maxLength];
			glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);

			std::cout << "Shader compilation error:" << "(" << shaderFilename << ")" << std::endl << infoLog << std::endl;

			delete[] infoLog;
			glDeleteShader(shader);
			return false;
		}

		if(_program == 0)
		{
			_program = glCreateProgram();
		}

		glAttachShader(_program, shader);
		glDeleteShader(shader);
		return true;
	}

	bool link(GLuint& program)
	{
		// Link program
		glLinkProgram(_program);

		// Check link status
		int ok = GL_FALSE;
		glGetProgramiv(_program, GL_LINK_STATUS, &ok);

		if(ok == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &maxLength);

			char* infoLog = new char[maxLength];
			glGetProgramInfoLog(_program, maxLength, &maxLength, infoLog);

			std::cout << "Program link error:" << std::endl << infoLog << std::endl;

			delete[] infoLog;
			glDeleteProgram(_program);
			_program = 0;
			return false;
		}

		// Validate program
		glValidateProgram(_program);

		// Check validation status
		ok = GL_FALSE;
		glGetProgramiv(_program, GL_VALIDATE_STATUS, &ok);

		if(!ok)
		{
			GLint maxLength = 0;
			glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &maxLength);

			char* infoLog = new char[maxLength];
			glGetProgramInfoLog(_program, maxLength, &maxLength, infoLog);

			delete[] infoLog;
			glDeleteProgram(_program);
			_program = 0;
			return false;
		}

		// Everything OK
		program = _program;
		_program = 0;
		return true;
	}

private:
	GLuint _program;
};
