#pragma once

#include "Area18/ogl/oglTypes.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

class oglDevice;

class oglTexture2d
{
public:
	oglTexture2d(oglDevice* i_pDevice,
		int w, int h, GLenum iFormat, 
            void* i_pInitialData, GLenum dataFormat, GLenum dataType);
	virtual ~oglTexture2d(void);

	static oglTexture2d* createFromFile(const std::string& iFilePath, oglDevice* iDevice);
	static void saveToFile(oglTexture2d*, const std::string& iFilePath, oglDevice* iDevice);

	virtual GLuint GetResource() {return mBuffer;}
	virtual GLuint GetBuffer() {return mBuffer;}

	size_t sizeBytes();
	size_t width() const {return mWidth;}
	size_t height() const {return mHeight;}
private:
	GLuint mBuffer;
	GLenum mFormat;
	size_t mWidth, mHeight;
};

#include <map>
typedef boost::shared_ptr<oglTexture2d> oglTexture2dHandle;
//typedef boost::shared_ptr<oglPlus::Texture> oglTextureHandle;

class oglTextureManager
{
public:
	oglTextureManager(size_t memLimit);

	oglTexture2dHandle loadTexture(std::string id, std::string path);
	oglTexture2dHandle createTexture(std::string id, size_t w, size_t h, GLenum internalFormat);
	oglTexture2dHandle createTexture(size_t w, size_t h, GLenum internalFormat, void* data = NULL, GLenum dataFormat = 0, GLenum dataType = 0);

private:
	typedef std::map<std::string, boost::weak_ptr<oglTexture2d> > InstanceMap;
	InstanceMap mTextureInstances;

	size_t mMemLimit;
	size_t mMem;

	oglTexture2dHandle mOutOfMemoryTexture;
	oglTexture2dHandle mLoadFailedTexture;

	friend class oglTextureFinalizer;
};

class oglTextureFinalizer
{
public:
	oglTextureFinalizer(oglTextureManager* m) : mTextureManager(m) {}
	void operator()(oglTexture2d* tex) 
	{
		mTextureManager->mMem -= tex->sizeBytes();
		delete tex;
	}
private:
	oglTextureManager* mTextureManager;

};
