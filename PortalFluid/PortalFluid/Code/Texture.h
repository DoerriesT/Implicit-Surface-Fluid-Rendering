#pragma once
#include <glad\glad.h>
#include <string>
#include <vector>
#include <memory>


class Texture
{
public:
	static std::shared_ptr<Texture> createTexture(const std::string &_filename);
	Texture(const Texture &other) = delete;
	Texture& operator= (Texture other) = delete;
	~Texture();
	const GLuint &getId() const;
	const GLenum &getType() const;

private:
	GLuint id;
	GLenum type;

	explicit Texture(const std::string &_filename);
	void initOpenGL(const std::string &_filename);
};