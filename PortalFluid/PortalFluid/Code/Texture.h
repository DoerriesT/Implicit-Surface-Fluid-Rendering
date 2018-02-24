#pragma once
#include <glad\glad.h>
#include <string>
#include <vector>
#include <memory>


class Texture
{
public:
	/*
	 * Returns a shared_ptr to a new Texture instance.
	 */
	static std::shared_ptr<Texture> createTexture(const std::string &_filename);

	/*
	 *	copy constructor and copy assignment are deleted functions;
	 *	new instances of Texture my only be created through createTexture
	 */
	Texture(const Texture &other) = delete;
	Texture& operator= (Texture other) = delete;

	/*
	 * Destructor
	 */
	~Texture();

	/*
	 * Returns a reference to the OpenGL issued id of this texture
	 */
	const GLuint &getId() const;

	/*
	 * Returns a reference to the OpenGL texture type of this texture
	 */
	const GLenum &getType() const;

private:
	// OpenGL issued texture id
	GLuint id;
	// OpenGL texture type
	GLenum type;

	/*
	 * Constructs a new Texture from the texture found at the given filepath
	 */
	explicit Texture(const std::string &_filename);

	/*
	 * Uploads and initializes this texture with OpenGL
	 */
	void initOpenGL(const std::string &_filename);
};