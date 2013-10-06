#include "oglContext.h"

#include "Core/Dbg/dbgMsg.hpp"
#include "Area18/Area18Layer.hpp"
#include "Area18/mat/matShaderBaseGL.hpp"
#include "Area18/ogl/GL/wglext.h"
#include "Area18/rndr/rndrEngine.h"
#include "Area18/shdr/shdrPipeline.hpp"
#include "Graphics/g3d/g3dType.hpp"
#include "Graphics/Mat/matShaderMgr.hpp"

static __declspec(thread) oglContext* gCurrentContext = NULL;

void getGLVersion(int& major, int& minor)
{
	// for all versions
	char* ver = (char*)glGetString(GL_VERSION); // ver = "3.2.0"
	major = ver[0] - '0';
	if( major >= 3)
	{
		// for GL 3.x
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);
	}
	else
	{
		minor = ver[2] - '0';
	}
	// GLSL version
	ver = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
}

oglContext::oglContext(HDC hDC, oglContext* shareContext)
: mPixelFormat (0)
, mRenderEngine(NULL)
, mVAO(0)
, mDummyBuffer(0)
{
	// context is current after this call
	createContext(hDC, (shareContext != NULL) ? shareContext->mHGLRC : NULL);
	setupVAOs();
	setupShaderPipelines();
	setupCL(hDC);
}


oglContext::~oglContext(void)
{
	if (isCurrent()) {
		doneCurrent();
	}

	glDeleteBuffers(1, &mDummyBuffer);
	glDeleteVertexArrays(1, &mVAO); 

}

void oglContext::setupVAOs()
{
	// create a dummy buffer
	glGenBuffers(1, &mDummyBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mDummyBuffer);
	g3dType::BumpTex1Vertex vtx;
	glBufferData(GL_ARRAY_BUFFER, sizeof(g3dType::BumpTex1Vertex), &vtx, GL_STATIC_DRAW);

	// create a VAO
	glGenVertexArrays(1, &mVAO); // Create our Vertex Array Object  
	glBindVertexArray(mVAO); // Bind our Vertex Array Object so we can use it

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), 0); 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), (GLvoid*)(sizeof(float)*3)); 
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), (GLvoid*)(sizeof(float)*6)); 
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), (GLvoid*)(sizeof(float)*8)); 
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(g3dType::BumpTex1Vertex), (GLvoid*)(sizeof(float)*11)); 
}

GLuint oglContext::getVAO()
{
	return mVAO;
}

// for all shaders in matShaderMgr, 
// store a shdrPipeline here for use by renderers.
void oglContext::setupShaderPipelines()
{
	class makePipelines : public matShaderMgr::matShaderMapVisitor
	{
	public:
		oglContext* mContext;
		virtual void visit(const std::string& key, matShaderEffect* effect) {
			matShaderBaseGL* gleffect = (matShaderBaseGL*)effect;
			shdrPipeline* pipeline = gleffect->GetEffect();
			// make a pipeline object with the same shaders
			// and store it in this context.
			mContext->mShaderMap[key] = new shdrPipeline(*pipeline);
		}
	};
	makePipelines visitor;
	visitor.mContext = this;
	matShaderMgr::visitShaders(&visitor);
}

shdrPipeline* oglContext::getShader(const std::string& s)
{
	// TODO: error check
	return mShaderMap[s];
}

void APIENTRY openglCallbackFunction(GLenum source,
										   GLenum type,
										   GLuint id,
										   GLenum severity,
										   GLsizei length,
										   const GLchar* message,
										   void* userParam)
{ 
	DBG_LOG( "*** GL DEBUG MSG: "<< message );
	//DBG_LOG( "type: ";
	//switch (type) {
	//case GL_DEBUG_TYPE_ERROR:
	//	DBG_LOG( "ERROR";
	//	break;
	//case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
	//	DBG_LOG( "DEPRECATED_BEHAVIOR";
	//	break;
	//case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
	//	DBG_LOG( "UNDEFINED_BEHAVIOR";
	//	break;
	//case GL_DEBUG_TYPE_PORTABILITY:
	//	DBG_LOG( "PORTABILITY";
	//	break;
	//case GL_DEBUG_TYPE_PERFORMANCE:
	//	DBG_LOG( "PERFORMANCE";
	//	break;
	//case GL_DEBUG_TYPE_OTHER:
	//	DBG_LOG( "OTHER";
	//	break;
	//}
	//DBG_LOG( endl;
 //
	//DBG_LOG( "id: "<    cout << "severity: ";
	//switch (severity){
	//case GL_DEBUG_SEVERITY_LOW:
	//	DBG_LOG( "LOW";
	//	break;
	//case GL_DEBUG_SEVERITY_MEDIUM:
	//	DBG_LOG( "MEDIUM";
	//	break;
	//case GL_DEBUG_SEVERITY_HIGH:
	//	DBG_LOG( "HIGH";
	//	break;
	//}
	//DBG_LOG( endl;
	//DBG_LOG( "---------------------opengl-callback-end--------------" << endl;
}

void oglContext::createContext(HDC hDC, HGLRC hShareContext)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int nPixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (nPixelFormat == 0) 
		throw "bad choosepixelformat";
	DescribePixelFormat(hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR),
								&pfd);
	BOOL bResult = SetPixelFormat (hDC, nPixelFormat, &pfd);
	if (!bResult) 
		throw "bad setpixelformat";

	HGLRC hDummyContext = wglCreateContext(hDC);
	bResult = wglMakeCurrent(hDC, hDummyContext);

	int major = 4, minor = 0;

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor, 
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, 
		0
	};

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
	if(wglCreateContextAttribsARB != NULL)
	{
		//create ogl 3.2 context
		mHGLRC = wglCreateContextAttribsARB(hDC, hShareContext, attribs);
		mPixelFormat = nPixelFormat;
	}

	//delete temporary context
	bResult = wglMakeCurrent(NULL,NULL); 
	wglDeleteContext(hDummyContext);

	bResult = wglMakeCurrent(hDC,mHGLRC);
		
	//after ogl 3.2 context is created load core opengl functions
	int retval = gl3wInit();
	if (retval != 0)
		DBG_LOG("Error loading opengl 4.1");
	getGLVersion(major,minor);

	// TODO: rid of arb
	glDebugMessageCallbackARB(openglCallbackFunction, this);

	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

bool oglContext::isCurrent()
{
	return (mHGLRC == wglGetCurrentContext());
}

void oglContext::makeCurrent(HDC hDC)
{
	if (mHGLRC == wglGetCurrentContext())
	{
		gCurrentContext = this;
		return;
	}

	BOOL b = wglMakeCurrent(hDC,mHGLRC);
	if (!b) {
		DWORD dw = ::GetLastError();
		LPVOID lpMsgBuf;

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );
		// Display the error message and exit the process
		DBG_LOG("makecurrent failed: " << lpMsgBuf); 
		LocalFree(lpMsgBuf);
	}
	else {
		gCurrentContext = this;
	}
}

void oglContext::doneCurrent()
{
	wglMakeCurrent(NULL,NULL);
	gCurrentContext = NULL;
}

oglContext* oglContext::currentContext() 
{
	return gCurrentContext;
}

void oglContext::update()
{
	if (mRenderEngine)
		mRenderEngine->draw();
}

void oglContext::resize(int w, int h)
{
//    TwWindowSize(w, h);
	if (mRenderEngine)
		mRenderEngine->resize(w,h);
}

int oglContext::setupCL(HDC hDC)
{
	cl_int err = CL_SUCCESS;
	cl_device_type dType = CL_DEVICE_TYPE_GPU;

	/*
	 * Have a look at the available platforms and pick either
	 * the AMD one if available or a reasonable default.
	 */
	err = cl::Platform::get(&platforms);
	if(err != CL_SUCCESS)
	{
		DBG_ERROR("OpenCL init: Platform::get() failed.");
		return SDK_FAILURE;
	}

	std::vector<cl::Platform>::iterator i;
	if(platforms.size() > 0)
	{
		for(i = platforms.begin(); i != platforms.end(); ++i)
		{
			if(!strcmp((*i).getInfo<CL_PLATFORM_VENDOR>().c_str(), 
				"Advanced Micro Devices, Inc."))
			{
				break;
			}
		}
	}

	cl_context_properties cps[7] = 
	{ 
		CL_CONTEXT_PLATFORM, 
		(cl_context_properties)(*i)(),
		CL_GL_CONTEXT_KHR, 
		(cl_context_properties)mHGLRC, 
		CL_WGL_HDC_KHR,
		(cl_context_properties)hDC,
		0 
	};

	context = cl::Context(dType, cps, NULL, NULL, &err);
	if(err != CL_SUCCESS)
	{
		DBG_ERROR("OpenCL init: Context::Context() failed.");
		return SDK_FAILURE;
	}


	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	if(err != CL_SUCCESS)
	{
		DBG_ERROR("OpenCL init: Context::getInfo() failed.");
		return SDK_FAILURE;
	}

	DBG_LOG("Platform :" << (*i).getInfo<CL_PLATFORM_VENDOR>().c_str());
	int deviceCount = (int)devices.size();
	int j = 0;
	for (std::vector<cl::Device>::iterator i = devices.begin(); i != devices.end(); ++i, ++j)
	{
		DBG_LOG("Device " << j << " : " << (*i).getInfo<CL_DEVICE_NAME>());
	}

	if (deviceCount == 0) 
	{
		DBG_ERROR("No device available");
		return SDK_FAILURE;
	}

	unsigned int deviceId = 0;
	if(deviceId >= deviceCount)
	{
		DBG_ERROR("sampleCommon::validateDeviceId() failed");
		return SDK_FAILURE;
	}

	commandQueue = cl::CommandQueue(context, devices[deviceId], 0, &err);
	if(err != CL_SUCCESS)
	{
		DBG_ERROR("CommandQueue::CommandQueue() failed.");
		return SDK_FAILURE;
	}

#if 0
	/*
	* Create and initialize memory objects
	*/

	// Set Presistent memory only for AMD platform
	cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
	if(isAmdPlatform())
		inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

	/* Create memory object for input Image */
	inputImageBuffer = cl::Buffer(context, 
								  inMemFlags, 
								  width * height * pixelSize,
								  0,
								  &err);
	if(!sampleCommon->checkVal(
		err,
		CL_SUCCESS,
		"Buffer::Buffer() failed. (inputImageBuffer)"))
	{
		return SDK_FAILURE;
	}


	/* Create memory object for output Image */
	outputImageBuffer = cl::Buffer(context, 
								   CL_MEM_WRITE_ONLY, 
								   width * height * pixelSize,
								   NULL,
								   &err);

	if(!sampleCommon->checkVal(err,
		CL_SUCCESS,
		"Buffer::Buffer() failed. (outputImageBuffer)"))
	{
		return SDK_FAILURE;
	}
#endif

	device.push_back(devices[deviceId]);

#if 0
	/* create a CL program using the kernel source */
	streamsdk::SDKFile kernelFile;
	std::string kernelPath = sampleCommon->getPath();

	if(isLoadBinaryEnabled())
	{
		kernelPath.append(loadBinary.c_str());
		if(!kernelFile.readBinaryFromFile(kernelPath.c_str()))
		{
			std::cout << "Failed to load kernel file : " << kernelPath << std::endl;
			return SDK_FAILURE;
		}
		cl::Program::Binaries programBinary(1,std::make_pair(
											  (const void*)kernelFile.source().data(), 
											  kernelFile.source().size()));
		
		program = cl::Program(context, device, programBinary, NULL, &err);
		if(!sampleCommon->checkVal(
			err,
			CL_SUCCESS,
			"Program::Program(Binary) failed."))
			return SDK_FAILURE;

	}
	else
	{
		kernelPath.append("GaussianNoise_Kernels.cl");
		if(!kernelFile.open(kernelPath.c_str()))
		{
			std::cout << "Failed to load kernel file : " << kernelPath << std::endl;
			return SDK_FAILURE;
		}

		cl::Program::Sources programSource(1, 
			std::make_pair(kernelFile.source().data(), 
			kernelFile.source().size()));
		
		program = cl::Program(context, programSource, &err);
		if(!sampleCommon->checkVal(
			err,
			CL_SUCCESS,
			"Program::Program(Source) failed."))
			return SDK_FAILURE;

	}

	std::string flagsStr = std::string("");

	// Get additional options
	if(isComplierFlagsSpecified())
	{
		streamsdk::SDKFile flagsFile;
		std::string flagsPath = sampleCommon->getPath();
		flagsPath.append(flags.c_str());
		if(!flagsFile.open(flagsPath.c_str()))
		{
			std::cout << "Failed to load flags file: " << flagsPath << std::endl;
			return SDK_FAILURE;
		}
		flagsFile.replaceNewlineWithSpaces();
		const char * flags = flagsFile.source().c_str();
		flagsStr.append(flags);
	}

	if(flagsStr.size() != 0)
		std::cout << "Build Options are : " << flagsStr.c_str() << std::endl;

	err = program.build(device, flagsStr.c_str());
	if(err != CL_SUCCESS)
	{
		if(err == CL_BUILD_PROGRAM_FAILURE)
		{
			std::string str = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[deviceId]);

			std::cout << " \n\t\t\tBUILD LOG\n";
			std::cout << " ************************************************\n";
			std::cout << str << std::endl;
			std::cout << " ************************************************\n";
		}
	}

	if(!sampleCommon->checkVal(
		err,
		CL_SUCCESS,
		"Program::build() failed."))
		return SDK_FAILURE;
	
	/* Create kernel */
	kernel = cl::Kernel(program, "gaussian_transform",  &err);
	if(!sampleCommon->checkVal(
		err,
		CL_SUCCESS,
		"Kernel::Kernel() failed."))
	{
		return SDK_FAILURE;
	}

	/* Check group size against group size returned by kernel */
	kernelWorkGroupSize = kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(devices[deviceId], &err);
	if(!sampleCommon->checkVal(
		err,
		CL_SUCCESS, 
		"Kernel::getWorkGroupInfo()  failed."))
	{
		return SDK_FAILURE;
	}

	if((blockSizeX * blockSizeY) > kernelWorkGroupSize)
	{
		if(!quiet)
		{
			std::cout << "Out of Resources!" << std::endl;
			std::cout << "Group Size specified : "
					  << blockSizeX * blockSizeY << std::endl;
			std::cout << "Max Group Size supported on the kernel : "
					  << kernelWorkGroupSize << std::endl;
			std::cout << "Falling back to " << kernelWorkGroupSize << std::endl;
		}

		if(blockSizeX > kernelWorkGroupSize)
		{
			blockSizeX = kernelWorkGroupSize;
			blockSizeY = 1;
		}
	}
#endif
	return SDK_SUCCESS;
}
