#pragma once

//#include "glew-1.5.7/include/gl/glew.h"
//#include "glew-1.5.7/include/gl/wglew.h"

//#define GLEW_STATIC
//#include "Area18/ogl/GL/glew.h"
//#include "Area18/ogl/GL/wglew.h"
#include "Area18/ogl/GL/gl3w.h"

//#include <CL/cl.hpp>
//#include <CL/opencl.h>

#include "Core/Dbg/dbgMsg.hpp"

class g2dPFD;

std::string GetGLError(GLenum err);
std::string GetGLFramebufferStatus(GLenum err);

#define CHECKGLERROR() \
	{\
	GLenum eGLERROR = glGetError();\
	if (eGLERROR != GL_NO_ERROR){\
		DBG_LOG("GL Error: " << eGLERROR << " " << GetGLError(eGLERROR));\
	}\
	}

#define CHECKGLFRAMEBUFFER(fb) \
	{\
	GLenum eGLERROR = glCheckFramebufferStatus(fb);\
	if (eGLERROR != GL_FRAMEBUFFER_COMPLETE){\
		DBG_LOG("GL Framebuffer: " << eGLERROR << " " << GetGLFramebufferStatus(eGLERROR));\
	}\
	}

namespace ogl 
{
int bitsPerPixel(GLenum gltype);
void PFDFromGLFormat(GLenum i_Format, g2dPFD& o_PFD);
GLenum GLFormatFromPFD(const g2dPFD& i_PFD);
}
