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
	/*
	 * Returns a shared_ptr to a new ShaderProgram instance. The last parameter, "const char *_geometryShaderPath" is optional
	 */
	static std::shared_ptr<ShaderProgram> createShaderProgram(const char *_vertexShaderPath, const char *_fragmentShaderPath, const char *_geometryShaderPath = nullptr);

	/*
	 *	copy constructor and copy assignment are deleted functions;
	 *	new instances of ShaderProgram my only be created through createShaderProgram
	 */
	ShaderProgram(const ShaderProgram &) = delete;
	ShaderProgram &operator= (const ShaderProgram &) = delete;

	/*
	 * Destructor
	 */
	~ShaderProgram();

	/*
	 * Binds this ShaderProgram to be used in subsequent draw calls
	 */
	void bind();

	/*
	 * Queries the location of the uniform with the given name and returns its location
	 */
	const GLint createUniform(const std::string &_name);

	/*
	 * Sets the uniform with the given location to the specified value
	 */
	void setUniform(const GLint &_location, const bool &_value);
	void setUniform(const GLint &_location, const int &_value);
	void setUniform(const GLint &_location, const float &value);
	void setUniform(const GLint &_location, const glm::mat3 &_value);
	void setUniform(const GLint &_location, const glm::mat4 &_value);
	void setUniform(const GLint &_location, const glm::vec2 &_value);
	void setUniform(const GLint &_location, const glm::vec3 &_value);
	void setUniform(const GLint &_location, const glm::vec4 &_value);

private:
	// the shader program id given by OpenGL
	GLuint programId;
#ifdef _DEBUG
	// debug map to assert that we only create a uniform once
	std::map<const std::string, GLint> uniforms;
#endif // DEBUG

	/*
	 * Constructs a ShaderProgram from the vertex, fragment and optionally geometry shaders given by their filepaths
	 */
	explicit ShaderProgram(const char *_vertexShaderPath, const char *_fragmentShaderPath, const char *_geometryShaderPath = nullptr);
};