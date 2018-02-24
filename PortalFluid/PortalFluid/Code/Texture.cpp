#include "Texture.h"
#include <algorithm>
#include <cassert>
#include <gli\texture.hpp>
#include <gli\load.hpp>

std::shared_ptr<Texture> Texture::createTexture(const std::string &_filename)
{
	return std::shared_ptr<Texture>(new Texture(_filename));
}

Texture::Texture(const std::string &_filename)
{
	initOpenGL(_filename);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

const GLuint &Texture::getId() const
{
	return id;
}

const GLenum &Texture::getType() const
{
	return type;
}

void Texture::initOpenGL(const std::string &_filename)
{
	// load the texture from the given filepath
	gli::texture texture(gli::load(_filename));
	// assert that the texture actually contains data
	assert(!texture.empty());

	// translate gli texture format to OpenGL texture format
	gli::gl glTranslator(gli::gl::PROFILE_GL33);
	gli::gl::format const format = glTranslator.translate(texture.format(), texture.swizzles());

	// translate gli texture typ to OpenGL texture type
	GLenum textureType = glTranslator.translate(texture.target());
	type = textureType;
	id = 0;

	// generate texture handle
	glGenTextures(1, &id);
	glBindTexture(textureType, id);
	// Base and max level are not supported by OpenGL ES 2.0
	glTexParameteri(textureType, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(textureType, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(texture.levels() - 1));
	//Texture swizzle is not supported by OpenGL ES 2.0 and OpenGL 3.2
	glTexParameteri(textureType, GL_TEXTURE_SWIZZLE_R, format.Swizzles[0]);
	glTexParameteri(textureType, GL_TEXTURE_SWIZZLE_G, format.Swizzles[1]);
	glTexParameteri(textureType, GL_TEXTURE_SWIZZLE_B, format.Swizzles[2]);
	glTexParameteri(textureType, GL_TEXTURE_SWIZZLE_A, format.Swizzles[3]);
	glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glm::tvec3<GLsizei> const extent(texture.extent());
	GLsizei const totalFaces = static_cast<GLsizei>(texture.layers() * texture.faces());

	// upload texture data to OpenGL
	for (std::size_t layer = 0; layer < texture.layers(); ++layer)
	{
		for (std::size_t level = 0; level < texture.levels(); ++level)
		{
			for (std::size_t face = 0; face < texture.faces(); ++face)
			{
				GLsizei const LayerGL = static_cast<GLsizei>(layer);
				glm::tvec3<GLsizei> extent(texture.extent(level));
				textureType = gli::is_target_cube(texture.target()) ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face) : textureType;

				switch (texture.target())
				{
				case gli::TARGET_1D:
				{
					if (gli::is_compressed(texture.format()))
					{
						glCompressedTexImage1D(textureType, format.Internal, static_cast<GLint>(level), 0, extent.x, static_cast<GLsizei>(texture.size(level)), texture.data(layer, face, level));
					}
					else
					{
						glTexImage1D(textureType, static_cast<GLint>(level), format.Internal, extent.x, 0, format.External, format.Type, texture.data(layer, face, level));
					}
					break;
				}
				case gli::TARGET_1D_ARRAY:
				case gli::TARGET_2D:
				case gli::TARGET_CUBE:
				{
					if (gli::is_compressed(texture.format()))
					{
						glCompressedTexImage2D(textureType, static_cast<GLint>(level), format.Internal, extent.x, texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : extent.y, 0, static_cast<GLsizei>(texture.size(level)), texture.data(layer, face, level));
					}
					else
					{
						glTexImage2D(textureType, static_cast<GLint>(level), format.Internal, extent.x, texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : extent.y, 0, format.External, format.Type, texture.data(layer, face, level));
					}
					break;
				}
				case gli::TARGET_2D_ARRAY:
				case gli::TARGET_3D:
				case gli::TARGET_CUBE_ARRAY:
				{
					if (gli::is_compressed(texture.format()))
					{
						glCompressedTexImage3D(textureType, static_cast<GLint>(level), format.Internal, extent.x, extent.y, texture.target() == gli::TARGET_3D ? extent.z : LayerGL, 0, static_cast<GLsizei>(texture.size(level)), texture.data(layer, face, level));
					}
					else
					{
						glTexImage3D(textureType, static_cast<GLint>(level), format.Internal, extent.x, extent.y, texture.target() == gli::TARGET_3D ? extent.z : LayerGL, 0, format.External, format.Type, texture.data(layer, face, level));
					}
					break;
				}
				default:
				{
					assert(false);
					break;
				}
				}
			}
		}
	}
}