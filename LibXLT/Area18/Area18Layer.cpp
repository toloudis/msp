/*****************************************************************************
**  Area18Layer.cpp
**
**      Area18Layer contains the initialization functions
**	for the all packages within the Area18 Layer.
**
**	Studio GPU 
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "Area18/Area18Layer.hpp"

#include "Area18/g3d/g3dLightMgrOGL.hpp"
#include "Area18/ogl/oglContext.h"
#include "Area18/ogl/oglDevice.hpp"
#include "Area18/ogl/oglSystem2D.h"
#include "Area18/ogl/oglSystem3D.h"
#include "Area18/ogl/oglWindow.h"
//#include "Area18/ocl/oclDevice.hpp"
#include "Area18/mesh/meshFragmentCreate.hpp"
#include "Area18/mesh/meshMdlFragCreate.hpp"
#include "Area18/mesh/meshRenderer.hpp"
#include "Area18/mesh/meshTriMeshFrag.hpp"
#include "Area18/rndr/rndrSceneRendererCreate.hpp"
#include "Area18/rndr/rndrTiledRenderer.hpp"
//#include "Area18/test/testCamera.h"
//#include "Area18/test/testPrimitive.h"
//#include "Area18/test/testTexture.h"
//#include "Area18/test/testFBO.h"
//#include "Area18/test/testDirLt.h"
//#include "Area18/test/testText.h"
#include "Area18/test/testScene.h"
//#include "AntTweakBar/include/AntTweakBar.h"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"

#include <vector>
#include "Area18/ogl/GL/wglext.h"

namespace
{

int l_RefCount = 0;
std::vector<oglDevice*> l_OpenGLDevices;
oglSystem2D* l_pSystem2D = NULL;
oglSystem3D* l_pSystem3D = NULL;

meshFragmentCreate* l_pFragmentCreator = NULL;
meshMdlFragCreate* l_pModelFragmentCreator = NULL;
meshRenderer* l_pMeshRenderer = NULL;
rndrSceneRendererCreate* l_pRendererCreator = NULL;
g3dLightMgrOGL *l_pLightMgrImpl = NULL;

oglContext* gDefaultContext = NULL;

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

void InitOpenGL()
{
	// must create a context first!

	int retval = gl3wInit();
	if (retval != 0)
		DBG_LOG("Error loading opengl 4.1");

	// default rendering of polygons
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);


//	GLenum err = glewInit();
//	if (GLEW_OK != err)
//	{
//		/* Problem: glewInit failed, something is seriously wrong. */
//		DBG_LOG("Error: " << glewGetErrorString(err));
//	}
//	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
//	if (GLEW_VERSION_3_2)
//	{
//		/* Yay! OpenGL 3.2 is supported! */
//		DBG_LOG("OpenGL 3.2 is supported.");
//	}
}

}

//----------------------------------------------------------------------------
//	Init
//----------------------------------------------------------------------------
void Area18Layer::Init(void* i_hWnd)
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// get a dc compatible with the windows desktop:
		HDC desktopDC = GetDC( HWND_DESKTOP ) ;
		// or
		//HDC desktopDC2 = GetDC( NULL ) ;
		// or
		//HDC desktopDC3 = GetDC( 0 ) ;
		HDC hDC = ::CreateCompatibleDC(desktopDC);
		if (hDC == NULL)
			throw "Could not create compatible desktop dc for opengl init.";
		ReleaseDC( HWND_DESKTOP, desktopDC ) ;

		gDefaultContext = new oglContext(hDC);

		//		loadCoreFunctions();

		//oglDevice* d = CreateDevice();
		//// initialize packages in the layer
		//InitOpenGL();

		oglDevice* d = new oglDevice(gDefaultContext);
		l_OpenGLDevices.push_back(d);
		//if (d)
		//	l_OpenGLDevices.push_back(d);

//		TwInit(TW_OPENGL_CORE, NULL);

		l_pSystem2D = new oglSystem2D;
		l_pSystem3D = new oglSystem3D;

		// initialize packages in the layer
		l_pFragmentCreator = new meshFragmentCreate;
		g3dFragmentCreate::SetImplementation(l_pFragmentCreator);

		l_pRendererCreator = new rndrSceneRendererCreate;
		g3dSceneRendererCreate::SetImplementation(l_pRendererCreator);

		l_pModelFragmentCreator = new meshMdlFragCreate;
		mdlFragCreate::SetImplementation(l_pModelFragmentCreator);

		l_pMeshRenderer = new meshRenderer;
		meshTriMeshFrag::SetRenderer(l_pMeshRenderer);

		l_pLightMgrImpl = new g3dLightMgrOGL;
		g3dLightMgr::SetImplementation(l_pLightMgrImpl);
	}

	//	increment the ref count
	l_RefCount++;
}

oglContext* Area18Layer::defaultContext()
{
	return gDefaultContext;
}

//----------------------------------------------------------------------------
//	CleanUp
//----------------------------------------------------------------------------
void Area18Layer::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//

		g3dLightMgr::SetImplementation(NULL);
		delete l_pLightMgrImpl;

		meshTriMeshFrag::SetRenderer(NULL);
		delete l_pMeshRenderer;

		mdlFragCreate::SetImplementation(NULL);
		delete l_pModelFragmentCreator;

		g3dSceneRendererCreate::SetImplementation(NULL);
		delete l_pRendererCreator;

		g3dFragmentCreate::SetImplementation(NULL);
		delete l_pFragmentCreator;

		delete l_pSystem3D;
		delete l_pSystem2D;

//		TwTerminate();

		envSTLHelpers::DeleteContainer(l_OpenGLDevices);

	}
}


//------------------------------------------------------------------------
//	InitGraphics - should be called after device is created
//------------------------------------------------------------------------
void Area18Layer::InitGraphics()
{
}

//------------------------------------------------------------------------
//	CleanUpGraphics - should be called before device is destroyed
//------------------------------------------------------------------------
void Area18Layer::CleanUpGraphics()
{
}

g2dSystem* Area18Layer::GetSystem2D()
{
	return l_pSystem2D;
}
g3dSystem* Area18Layer::GetSystem3D()
{
	return l_pSystem3D;
}

testScene* Area18Layer::GetTestScene(TestScene t)
{
	testScene* ts = NULL;
	switch(t) {
//	case TS_CAMERA: ts = new testCamera; break;
//	case 	TS_FBO: ts = new testFBO; break;
//	case 	TS_TEXTURE: ts = new testTexture; break;
//	case 	TS_PRIMITIVE: ts = new testPrimitive; break;
//	case TS_TEXT: ts = new testText; break;
//	case TS_DIRLT: ts = new testDirLt; break;
	};
	if (ts != NULL) ts->Setup();
	return ts;
}

oglDevice* Area18Layer::GetDevice(int i)
{
	return l_OpenGLDevices[i];
}

rndrEngine* Area18Layer::createTiledRenderer()
{
	return new rndrTiledRenderer(GetDevice(0));
}
