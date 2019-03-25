//////////////////////////////////////////////////////////////////////////
//
// This is a simple OpenGL Texture2D class
//
// Michael Gharbharan 2015 - 2017
//
//////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include <string>
#define GLEW_STATIC
#include "glew/glew.h"
#include <memory>

namespace TTK
{
	class Texture2D
	{
	public:
		typedef std::shared_ptr<Texture2D> Ptr;

		Texture2D();
		Texture2D(std::string filePath); // Loads texture and creates gl texture
		~Texture2D();

		// Returns width / height of texture in pixels
		int width();
		int height();

		// Returns texture pixel internal format
		GLenum internalFormat();
		GLenum textureFormat();
		GLenum dataType();

		// Binds / unbinds texture
		void bind(GLenum textureUnit = GL_TEXTURE0);
		void unbind(GLenum textureUnit = GL_TEXTURE0);

		// Loads a texture from file and stores the data in m_pDataPtr
		// If 'createGLTexture' is true, then an OpenGL texture will be created
		// Otherwise the function just loads the data into memory and returns
		void loadTextureFromFile(std::string filePath, bool createGLTexture, bool flipY, bool keepTextureInMemory);

		// Description:
		// Creates the texture, allocates memory and uploads data to GPU
		// If you do not want to upload data to the GPU pass in a nullptr for the dataPtr.
		// For a description on filtering and edgeBehaviour see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexParameter.xml
		// For a description on internalFormat, textureFormat and dataType see https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml
		void createTexture(int w, int h, GLenum target, GLenum filtering, GLenum edgeBehaviour, GLenum internalFormat, GLenum textureFormat, GLenum dataType, void* newDataPtr);

		// Description:
		// Deletes texture allocated on the GPU
		void deleteTexture();

		// Returns OpenGL texture id
		unsigned int id();

		// Returns pointer to data
		void* data();

		// Removes the texture data from system memory
		// If an OpenGL texture was created, then the texture will still exist in GPU memory
		// If 'keepTextureInMemory' was set to true when calling 'loadTextureFromFile' then you'd need to
		// call this at some point to free the memory.
		void freeCpuMemory();

	private:
		unsigned int m_pTexWidth;
		unsigned int m_pTexHeight;
		unsigned int m_pTexID;

		GLenum m_pFiltering;
		GLenum m_pEdgeBehaviour;
		GLenum m_pInternalFormat;
		GLenum m_pTextureFormat;
		GLenum m_pDataType;

		GLenum m_pTextureUnit;

		GLenum m_pTarget; // usually GL_TEXTURE_2D

		void* m_pDataPtr;
		void* m_pFreeImageData; // internal freeimage data
	};
}

#endif
