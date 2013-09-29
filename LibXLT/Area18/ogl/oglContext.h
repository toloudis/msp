#pragma once

#include "Area18/ogl/oglTypes.hpp"
#include <map>

class rndrEngine;
class shdrPipeline;

// GL objects that can NOT be shared:
// Framebuffer Objects
// Program Pipeline Objects
// Transform Feedback Objects
// Vertex Array Objects
//
// other types CAN be shared:
// Buffer Objects
// Query Objects
// Renderbuffer Objects
// Sampler Objects
// Texture Objects
// (*)Sync Objects
// (*)GLSL Shader and Program Objects
class oglContext
{
public:
	oglContext(HDC hDC, oglContext* shareContext = NULL);
	virtual ~oglContext(void);

	void makeCurrent(HDC hDC);
	bool isCurrent();
	void update();
	void doneCurrent();
	void resize(int w, int h);

	int pixelFormat() {return mPixelFormat;}

	void setRenderEngine(rndrEngine* engine) {mRenderEngine = engine;}
	rndrEngine* renderEngine() {return mRenderEngine;}

	cl::Context& clContext() {return context;}
	std::vector<cl::Device>& clDevices() {return devices;}
	cl::CommandQueue& clCommandQueue() {return commandQueue;}

	void setupShaderPipelines();
	shdrPipeline* getShader(const std::string& s);

	void setupVAOs();
	GLuint getVAO();

	// get the current (per-thread) context!
	static oglContext* currentContext();
private:
	HGLRC mHGLRC;
	int mPixelFormat;

	void createContext(HDC hDC, HGLRC hShareContext);

	rndrEngine* mRenderEngine;


    cl::Context context;                    /**< Context */
    std::vector<cl::Device> devices;        /**< vector of devices */
    std::vector<cl::Device> device;         /**< device to be used */
    std::vector<cl::Platform> platforms;    /**< vector of platforms */
    cl::CommandQueue commandQueue;          /**< command queue */
#define SDK_SUCCESS 0
#define SDK_FAILURE 1
#define SDK_EXPECTED_FAILURE 2
	int setupCL(HDC hDC);

	std::map<std::string, shdrPipeline*> mShaderMap;

	GLuint mVAO;
	GLuint mDummyBuffer;
};

