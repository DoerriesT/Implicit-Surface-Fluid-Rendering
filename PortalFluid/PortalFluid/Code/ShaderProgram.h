#pragma once
#include <glad\glad.h>
#include <string>
#ifdef _DEBUG
#include <map>
#endif // DEBUG
#include <vector>
#include <glm\mat4x4.hpp>
#include <memory>

class ShaderProgram
{
public:
	static std::shared_ptr<ShaderProgram> createShaderProgram(const char *_vertexShaderPath, const char *_fragmentShaderPath, const char *_geometryShaderPath = nullptr);
	ShaderProgram(const ShaderProgram &) = delete;
	ShaderProgram &operator= (const ShaderProgram &) = delete;
	~ShaderProgram();
	void bind();
	const GLint createUniform(const std::string &_name);
	void setUniform(const GLint &_location, const bool &_value);
	void setUniform(const GLint &_location, const int &_value);
	void setUniform(const GLint &_location, const float &value);
	void setUniform(const GLint &_location, const glm::mat3 &_value);
	void setUniform(const GLint &_location, const glm::mat4 &_value);
	void setUniform(const GLint &_location, const glm::vec2 &_value);
	void setUniform(const GLint &_location, const glm::vec3 &_value);
	void setUniform(const GLint &_location, const glm::vec4 &_value);

private:
	GLuint programId;
#ifdef _DEBUG
	std::map<const std::string, GLint> uniforms;
#endif // DEBUG

	explicit ShaderProgram(const char *_vertexShaderPath, const char *_fragmentShaderPath, const char *_geometryShaderPath = nullptr);
};