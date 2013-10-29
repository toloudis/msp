#include "oglTexture2d.h"

#include <assert.h>
#include "FreeImage/Dist/FreeImage.h"
#include "Core/Dbg/dbgMsg.hpp"

#pragma comment(lib, "FreeImage.lib")

oglTexture2d::oglTexture2d(oglDevice* i_pDevice,
	int w, int h, GLenum iFormat, 
		void* i_pInitialData, GLenum dataFormat, GLenum dataType)
		: mBuffer(0)
{
	glGenTextures(1, &mBuffer);
	glBindTexture(GL_TEXTURE_2D, mBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, iFormat, w, h, 0, dataFormat, dataType, i_pInitialData);
	CHECKGLERROR();
	mFormat = iFormat;
}

oglTexture2d::~oglTexture2d(void)
{
	glDeleteTextures(1, &mBuffer);
}

namespace {
	size_t bitsPerPixel(GLenum format)
	{
		switch(format) {
		case GL_RGBA8: return 32;
		default:
			DBG_ERROR("Bad format in bitsPerPixel " << format);
		}
		return 0;
	}
	size_t sizeBytes(size_t w, size_t h, GLenum format)
	{
		// danger of overflow?
		return w*h*bitsPerPixel(format) / 8;
	}
}

size_t oglTexture2d::sizeBytes()
{
	// danger of overflow?
	return mWidth*mHeight*bitsPerPixel(mFormat) / 8;
}

void oglTexture2d::saveToFile(oglTexture2d* t, const std::string& iFilePath, oglDevice* iDevice)
{
	// load with FreeImage
	FreeImage_Initialise();

	// assume rgba image
	BYTE* pixels = new BYTE[4*t->width()*t->height()];
	glGetTexImage(	GL_TEXTURE_2D,
 		0,
 		GL_RGBA,
 		GL_UNSIGNED_BYTE,
 		pixels);

	FIBITMAP* Image = FreeImage_ConvertFromRawBits(pixels, t->width(), t->height(), t->width()*4, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE); 
	// use file extension to determine save fmt?
	FreeImage_Save(FIF_UNKNOWN, Image, iFilePath.c_str(), 0);
	delete [] pixels;
	FreeImage_DeInitialise();
}

oglTexture2d* oglTexture2d::createFromFile(const std::string& iFilePath, oglDevice* iDevice)
{
	oglTexture2d* texture = NULL;

	std::string filePath = iFilePath;

	// load with FreeImage
	FreeImage_Initialise();
		
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	// check the file signature and deduce its format
	// (the second argument is currently not used by FreeImage)
	fif = FreeImage_GetFileType(filePath.c_str(), 0);
	if(fif == FIF_UNKNOWN) 
	{
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(filePath.c_str());
	}
	// check that the plugin has reading capabilities ...
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) 
	{
		const int flag = 0;
		// ok, let's load the file
		FIBITMAP *dib = FreeImage_Load(fif, filePath.c_str(), flag);
		// unless a bad file format, we are done !


		//get the image width and height
		unsigned width = FreeImage_GetWidth(dib);
		unsigned height = FreeImage_GetHeight(dib);
		unsigned bpp = FreeImage_GetBPP(dib);
		FREE_IMAGE_TYPE type = FreeImage_GetImageType(dib);

		GLenum glformat;
		GLenum gldataformat;
		GLenum gldatatype;
		switch (type)
		{
			case FIT_BITMAP:  	// standard image			: 1-, 4-, 8-, 16-, 24-, 32-bit
				switch(bpp) 
				{
				case 32:
					gldataformat = GL_RGBA;
					gldatatype = GL_UNSIGNED_BYTE;
					glformat = GL_RGBA8;
					break;
				case 24:
					{
					gldataformat = GL_RGB;
					gldatatype = GL_UNSIGNED_BYTE;
					glformat = GL_RGBA8;
					break;
					}
				case 16:
					gldataformat = GL_RGB;
					gldatatype = GL_UNSIGNED_SHORT_5_6_5;
					glformat = GL_RGB5_A1;
					break;
				case 8:
					gldataformat = GL_RED;
					gldatatype = GL_UNSIGNED_BYTE;
					glformat = GL_RGBA8;
					break;
				default:
					throw("Unknown bpp in FIT_BITMAP texture file");
				}
				break;
			case FIT_UINT16:	// array of unsigned short	: unsigned 16-bit
				assert(bpp == 16);
				gldataformat = GL_RED;
				gldatatype = GL_UNSIGNED_SHORT;
				glformat = GL_R16UI;
				break;
			case FIT_INT16:		// array of short			: signed 16-bit
				assert(bpp == 16);
				gldataformat = GL_RED;
				gldatatype = GL_SHORT;
				glformat = GL_R16I;
				break;
			case FIT_UINT32:	// array of unsigned long	: unsigned 32-bit
				assert(bpp == 32);
				gldataformat = GL_RED;
				gldatatype = GL_UNSIGNED_INT;
				glformat = GL_R32UI;
				break;
			case FIT_INT32:		// array of long			: signed 32-bit
				assert(bpp == 32);
				gldataformat = GL_RED;
				gldatatype = GL_INT;
				glformat = GL_R32I;
				break;
			case FIT_FLOAT:		// array of float			: 32-bit IEEE floating point
				assert(bpp == 32);
				gldataformat = GL_RED;
				gldatatype = GL_FLOAT;
				glformat = GL_R32F;
				break;
			case FIT_RGB16:		// 48-bit RGB image			: 3 x 16-bit
				{
					FIBITMAP* dib32 = FreeImage_ConvertTo32Bits(dib);
					FreeImage_Unload(dib);
					dib = dib32;
					gldataformat = GL_BGRA;
					gldatatype = GL_UNSIGNED_BYTE;
					glformat = GL_RGBA8;
					break;
				}
			case FIT_RGBA16:	// 64-bit RGBA image		: 4 x 16-bit
				assert(bpp == 64);
				gldataformat = GL_RGBA;
				gldatatype = GL_UNSIGNED_SHORT;
				glformat = GL_RGBA16F;
				break;
			case FIT_RGBF:		// 96-bit RGB float image	: 3 x 32-bit IEEE floating point
				assert(bpp == 96);
				gldataformat = GL_RGB;
				gldatatype = GL_FLOAT;
				glformat = GL_RGB32F;
				break;
			case FIT_RGBAF:		// 128-bit RGBA float image	: 4 x 32-bit IEEE floating point
				assert(bpp == 128);
				gldataformat = GL_RGBA;
				gldatatype = GL_FLOAT;
				glformat = GL_RGBA32F;
				break;
			default:
				DBG_LOG("Unknown image format from FreeImage");
				break;
		}

		if (dib) {
			//retrieve the image data
			BYTE* data = FreeImage_GetBits(dib);

			texture = new oglTexture2d(iDevice,
				width, height, glformat, data, gldataformat, gldatatype);
		}
		else {
			texture = NULL;
		}
	}
	else
	{
		// failed to load!
	}
	FreeImage_DeInitialise();

	return texture;
}


oglTexture2dHandle oglTextureManager::loadTexture(std::string id, std::string path)
{
	oglTexture2dHandle texture;

	InstanceMap::iterator i = mTextureInstances.find(id);
	if (i != mTextureInstances.end())
	{
		texture = i->second.lock();
		if (texture) 
		{
			return texture;
		}
	}

	// load the texture from file
	//TODO: break loading into load vs tex create, for mem checking.
	oglTexture2d* t = oglTexture2d::createFromFile(path, NULL);
	size_t texSize = t->sizeBytes();
	if (mMem + texSize > mMemLimit) {
		delete t;
		return mOutOfMemoryTexture;
	}

	// register it here and return the shared ptr
	texture.reset(t, oglTextureFinalizer(this));
	mMem += texSize;
	mTextureInstances[id] = texture;
	return texture;
}

oglTexture2dHandle oglTextureManager::createTexture(std::string id, size_t w, size_t h, GLenum internalFormat)
{
	oglTexture2dHandle texture;

	InstanceMap::iterator i = mTextureInstances.find(id);
	if (i != mTextureInstances.end())
	{
		texture = i->second.lock();
		if (texture) 
		{
			return texture;
		}
	}

	size_t texSize = sizeBytes(w, h, internalFormat);
	if (mMem + texSize > mMemLimit) {
		return mOutOfMemoryTexture;
	}
	// register it here and return the shared ptr
	texture.reset(new oglTexture2d(NULL, w, h, internalFormat, NULL, 0, 0), oglTextureFinalizer(this));
	mMem += texSize;
	mTextureInstances[id] = texture;
	return texture;
}

oglTexture2dHandle oglTextureManager::createTexture(size_t w, size_t h, GLenum internalFormat, 
	void* data, GLenum dataFormat, GLenum dataType)
{
	size_t texSize = sizeBytes(w, h, internalFormat);
	if (mMem + texSize > mMemLimit) {
		return mOutOfMemoryTexture;
	}

	oglTexture2dHandle texture;

	// register it here and return the shared ptr
	texture.reset(new oglTexture2d(NULL, w, h, internalFormat, data, dataFormat, dataType), oglTextureFinalizer(this));
	mMem += texSize;
	return texture;
}
