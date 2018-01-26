#include <glm\gtc\type_ptr.hpp>
#include <glm\ext.hpp>
#include <cassert>
#include <iostream>
#include "ShaderProgram.h"
#include "Utility.h"



ShaderProgram::ShaderProgram(const char *_vertexShaderPath, const char *_fragmentShaderPath, const char *_geometryShaderPath)
{
	const char *vertexShaderCode = readTextResourceFile(_vertexShaderPath);
	const char *fragmentShaderCode = readTextResourceFile(_fragmentShaderPath);
	const char *geometryShaderCode = nullptr;
	if (_geometryShaderPath)
	{
		geometryShaderCode = readTextResourceFile(_geometryShaderPath);
	}

	unsigned int vertex, fragment, geometry;
	int success;
	char infoLog[512];

	// vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << _vertexShaderPath << "\n" << infoLog << std::endl;
		assert(false);
	};

	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << _fragmentShaderPath << "\n" << infoLog << std::endl;
		assert(false);
	};

	if (_geometryShaderPath)
	{
		// geometry Shader
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &geometryShaderCode, NULL);
		glCompileShader(geometry);
		// print compile errors if any
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << _geometryShaderPath << "\n" << infoLog << std::endl;
			assert(false);
		};
	}
	

	// shader Program
	programId = glCreateProgram();
	glAttachShader(programId, vertex);
	glAttachShader(programId, fragment);
	if (_geometryShaderPath)
	{
		glAttachShader(programId, geometry);
	}
	glLinkProgram(programId);
	// print linking errors if any
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// delete the shaders as they're linked into our program now and no longer necessery
	glDetachShader(programId, vertex);
	glDetachShader(programId, fragment);
	if (_geometryShaderPath)
	{
		glDetachShader(programId, geometry);
		glDeleteShader(geometry);
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// validate program
	glValidateProgram(programId);
	glGetProgramiv(programId, GL_VALIDATE_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
		std::cout << "WARNING::SHADER::PROGRAM::VALIDATION_FAILED\n" << infoLog << std::endl;
	}

	delete[] vertexShaderCode;
	delete[] fragmentShaderCode;
	if (_geometryShaderPath)
	{
		delete[] geometryShaderCode;
	}
}

std::shared_ptr<ShaderProgram> ShaderProgram::createShaderProgram(const char *_vertexShaderPath, const char *_fragmentShaderPath, const char *_geometryShaderPath)
{
	return std::shared_ptr<ShaderProgram>(new ShaderProgram(_vertexShaderPath, _fragmentShaderPath, _geometryShaderPath));
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(programId);
}

void ShaderProgram::bind()
{
	glUseProgram(programId);
}

const GLint ShaderProgram::createUniform(const std::string &_name)
{
	const GLint id = glGetUniformLocation(programId, _name.c_str());

#ifdef _DEBUG
	// make sure we have not already created this uniform
	assert(uniforms.find(_name) == uniforms.end());
	uniforms[_name] = id;
#endif // DEBUG

	return id;
}

void ShaderProgram::setUniform(const GLint &_location, const bool &_value)
{
	glUniform1i(_location, _value);
}

void ShaderProgram::setUniform(const GLint &_location, const int &_value)
{
	glUniform1i(_location, _value);
}

void ShaderProgram::setUniform(const GLint &_location, const float &_value)
{
	glUniform1f(_location, _value);
}

void ShaderProgram::setUniform(const GLint &_location, const glm::mat3 &_value)
{
	glUniformMatrix3fv(_location, 1, GL_FALSE, glm::value_ptr(_value));
}

void ShaderProgram::setUniform(const GLint &_location, const glm::mat4 &_value)
{
	glUniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(_value));
}

void ShaderProgram::setUniform(const GLint &_location, const glm::vec2 &_value)
{
	glUniform2f(_location, _value.x, _value.y);
}

void ShaderProgram::setUniform(const GLint &_location, const glm::vec3 &_value)
{
	glUniform3f(_location, _value.x, _value.y, _value.z);
}

void ShaderProgram::setUniform(const GLint &_location, const glm::vec4 &_value)
{
	glUniform4f(_location, _value.x, _value.y, _value.z, _value.w);
}