#include <GLEW/glew.h>
#include "TTK/Texture2D.h"
#include "FreeImage/FreeImage.h"
#include <iostream>

TTK::Texture2D::Texture2D()
	: m_pTexWidth(0),
	m_pTexHeight(0),
	m_pTexID(0), m_pDataPtr(0)
{
}

TTK::Texture2D::Texture2D(std::string filePath)
	: m_pTexWidth(0),
	m_pTexHeight(0),
	m_pTexID(0), m_pDataPtr(0)
{
	loadTextureFromFile(filePath, true, false, false);
}

TTK::Texture2D::~Texture2D()
{
	deleteTexture();
}

int TTK::Texture2D::width()
{
	return m_pTexWidth;
}

int TTK::Texture2D::height()
{
	return m_pTexHeight;
}

GLenum TTK::Texture2D::internalFormat()
{
	return m_pInternalFormat;
}

GLenum TTK::Texture2D::textureFormat()
{
	return m_pTextureFormat;
}

GLenum TTK::Texture2D::dataType()
{
	return m_pDataType;
}

void TTK::Texture2D::bind(GLenum textureUnit /* = GL_TEXTURE0 */)
{
	glActiveTexture(textureUnit);
	glBindTexture(m_pTarget, m_pTexID);
}

void TTK::Texture2D::unbind(GLenum textureUnit /* = GL_TEXTURE0 */)
{
	glActiveTexture(textureUnit);
	glBindTexture(m_pTarget, 0);
}

void TTK::Texture2D::loadTextureFromFile(std::string filePath, bool createGLTexture, bool flipY, bool keepTextureInMemory)
{
	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// bytes per pixel
	unsigned int bpp(0);

	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(filePath.c_str(), 0);

	//if still unknown, try to guess the file format from the file extension
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(filePath.c_str());
	//if still unknown, return failure
	if (fif == FIF_UNKNOWN)
	{
		std::cout << "Unable to load texture: " + filePath << std::endl;
		return;
	}

	//check that the plugin has reading capabilities and load the file
	if (FreeImage_FIFSupportsReading(fif))
		m_pFreeImageData = FreeImage_Load(fif, filePath.c_str());
	
	//if the image failed to load, return failure
	if (!m_pFreeImageData)
	{
		std::cout << "Unable to load texture: " + filePath << std::endl;
		return;
	}

	//retrieve the image data
	FIBITMAP *dib = (FIBITMAP*)m_pFreeImageData;
	m_pDataPtr = FreeImage_GetBits(dib);

	//get the image width and height
	m_pTexWidth = FreeImage_GetWidth(dib);
	m_pTexHeight = FreeImage_GetHeight(dib);
	bpp = FreeImage_GetBPP(dib);

	//if this somehow one of these failed (they shouldn't), return failure
	if ((m_pDataPtr == 0) || (m_pTexWidth == 0) || (m_pTexHeight == 0))
	{
		std::cout << "Unable to load texture: " + filePath << std::endl;
		return;
	}

	switch (bpp) // note: freeimage uses bgr
	{
	case 24:
		m_pInternalFormat = GL_RGB8;
		m_pTextureFormat = GL_BGR;
		break;

	case 32:
		m_pInternalFormat = GL_RGBA8;
		m_pTextureFormat = GL_BGRA;
		break;

	default:
		std::cout << "Unsupported texture format. Texture may look strange." + filePath << std::endl;
		m_pInternalFormat = GL_RGB8;
		m_pTextureFormat = GL_BGR;
		break;
	}

	if (flipY)
		FreeImage_FlipVertical(dib);

	if (createGLTexture)
		createTexture(m_pTexWidth, m_pTexHeight, GL_TEXTURE_2D, GL_LINEAR, GL_CLAMP_TO_EDGE, m_pInternalFormat, m_pTextureFormat, GL_UNSIGNED_BYTE, m_pDataPtr);

	//Free FreeImage's copy of the data
	if (!keepTextureInMemory)
		freeCpuMemory();
}

void TTK::Texture2D::createTexture(int w, int h, GLenum target, GLenum filtering, GLenum edgeBehaviour, GLenum internalFormat, GLenum textureFormat, GLenum dataType, void* newDataPtr)
{
	m_pTexWidth = w;
	m_pTexHeight = h;
	m_pFiltering = filtering;
	m_pEdgeBehaviour = edgeBehaviour;
	m_pInternalFormat = internalFormat;
	m_pTextureFormat = textureFormat;
	m_pDataType = dataType;
	m_pTarget = target;

	GLenum error = 0;

	// Not necessary to enable GL_TEXTURE_* in modern context.
//	glEnable(m_pTarget);
//	error = glGetError();

	if (m_pTexID)
		deleteTexture();

	glGenTextures(1, &m_pTexID);
	glBindTexture(target, m_pTexID);
	error = glGetError();

	glTexParameteri(m_pTarget, GL_TEXTURE_MIN_FILTER, filtering);
	glTexParameteri(m_pTarget, GL_TEXTURE_MAG_FILTER, filtering);
	glTexParameteri(m_pTarget, GL_TEXTURE_WRAP_S, edgeBehaviour);
	glTexParameteri(m_pTarget, GL_TEXTURE_WRAP_T, edgeBehaviour);
	error = glGetError();

	glTexImage2D(m_pTarget, 0, internalFormat, w, h, 0, textureFormat, dataType, newDataPtr);
	error = glGetError();

	if (error != 0)
		std::cout << "There was an error somewhere when creating texture. " << std::endl;

	glBindTexture(m_pTarget, 0);
}


void TTK::Texture2D::deleteTexture()
{
	glDeleteTextures(1, &m_pTexID);
}

unsigned int TTK::Texture2D::id()
{
	return m_pTexID;
}

void * TTK::Texture2D::data()
{
	return m_pDataPtr;
}

void TTK::Texture2D::freeCpuMemory()
{
	FreeImage_Unload((FIBITMAP*)m_pFreeImageData);
}
