/*****************************************************************************
**  GraphicsOptiXLayer.cpp
**
**      See GraphicsOptiXLayer.hpp. Hooks are installed where GraphicsVulkan
**	installs its own: the 2D and 3D systems install theirs while they
**	exist; Init installs the scene renderer creator and the fragment
**	creators; InitGraphics installs baking.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_GRAPHICSOPTIXLAYER_HPP
#include "GraphicsOptiX/GraphicsOptiXLayer.hpp"
#endif
#ifndef GOX_OPTXLOADER_HPP
#include "GraphicsOptiX/optx/optxLoader.hpp"
#endif
#ifndef GOX_G2DSYSTEMOPTIX_HPP
#include "GraphicsOptiX/G2d/g2dSystemOptiX.hpp"
#endif
#ifndef GOX_G3DSYSTEMOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dSystemOptiX.hpp"
#endif
#ifndef GOX_G3DSCENERENDERERCREATEOPTIX_HPP
#include "GraphicsOptiX/G3d/private/g3dSceneRendererCreateOptiX.hpp"
#endif
#ifndef GOX_G3DBAKEOPTIX_HPP
#include "GraphicsOptiX/G3d/private/g3dBakeOptiX.hpp"
#endif
#ifndef GOX_G3DFRAGMENTCREATEOPTIX_HPP
#include "GraphicsOptiX/G3d/private/g3dFragmentCreateOptiX.hpp"
#endif
#ifndef GOX_MAYFRAGCREATEOPTIX_HPP
#include "GraphicsOptiX/may/private/mayFragCreateOptiX.hpp"
#endif
#ifndef G3D_GFXOPTIONS_HPP
#include "Graphics/G3d/g3dGfxOptions.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <string>

using namespace gox;

namespace
{
	std::string						l_InitError;
	bool							l_bInitialized = false;
	g2dSystemOptiX*					l_pSystem2D = NULL;
	g3dSystemOptiX*					l_pSystem3D = NULL;
	g3dSceneRendererCreateOptiX*	l_pRendererCreator = NULL;
	g3dFragmentCreateOptiX*			l_pFragmentCreator = NULL;
	mayFragCreateOptiX*				l_pModelFragmentCreator = NULL;
	g3dBakeOptiX*					l_pBake = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool GraphicsOptiXLayer::Init(void* /*i_hWnd*/, const g3dGfxOptions& i_Options)
{
	DBG_ASSERT(!l_bInitialized, "GraphicsOptiXLayer::Init called twice");
	l_InitError.clear();

#ifdef GOX_NO_CUDA_TOOLKIT
	//	No device programs (cuda.props): nothing could be drawn
	l_InitError = "this build was made without the CUDA toolkit";
	DBG_LOG("GraphicsOptiX: " << l_InitError);
	return false;
#endif
	if (!optxLoader::Load(l_InitError))
	{
		DBG_LOG("GraphicsOptiX: " << l_InitError);
		return false;
	}
	l_bInitialized = true;

	l_pSystem2D = new g2dSystemOptiX(i_Options.m_bHeadless);
	l_pSystem3D = new g3dSystemOptiX;

	l_pRendererCreator = new g3dSceneRendererCreateOptiX;
	g3dSceneRendererCreate::SetImplementation(l_pRendererCreator);
	l_pFragmentCreator = new g3dFragmentCreateOptiX;
	g3dFragmentCreate::SetImplementation(l_pFragmentCreator);
	l_pModelFragmentCreator = new mayFragCreateOptiX;
	mdlFragCreate::SetImplementation(l_pModelFragmentCreator);
	return true;
}

void GraphicsOptiXLayer::CleanUp() throw()
{
	if (!l_bInitialized)
	{
		return;
	}
	mdlFragCreate::SetImplementation(NULL);
	delete l_pModelFragmentCreator;
	l_pModelFragmentCreator = NULL;
	g3dFragmentCreate::SetImplementation(NULL);
	delete l_pFragmentCreator;
	l_pFragmentCreator = NULL;
	g3dSceneRendererCreate::SetImplementation(NULL);
	delete l_pRendererCreator;
	l_pRendererCreator = NULL;

	delete l_pSystem3D;
	delete l_pSystem2D;
	l_pSystem3D = NULL;
	l_pSystem2D = NULL;

	optxLoader::Unload();
	l_bInitialized = false;
}

void GraphicsOptiXLayer::InitGraphics()
{
	l_pBake = new g3dBakeOptiX;
	g3dBake::SetImplementation(l_pBake);
}

void GraphicsOptiXLayer::CleanUpGraphics()
{
	g3dBake::SetImplementation(NULL);
	delete l_pBake;
	l_pBake = NULL;
}

g2dSystem* GraphicsOptiXLayer::GetSystem2D()
{
	return l_pSystem2D;
}

g3dSystem* GraphicsOptiXLayer::GetSystem3D()
{
	return l_pSystem3D;
}

const char* GraphicsOptiXLayer::GetName()
{
	return "OptiX";
}

const char* GraphicsOptiXLayer::GetCodeName()
{
	return "optix";
}

const std::string& GraphicsOptiXLayer::GetInitError()
{
	return l_InitError;
}
