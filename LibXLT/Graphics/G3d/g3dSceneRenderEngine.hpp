/****************************************************************************\
**	g3dSceneRenderEngine.hpp
**
**	The g3dSceneRenderEngine renders the g3dSceneNodes which describe the
**	graphical scene.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_SCENERENDERER_HPP
#error g3dSceneRenderEngine.hpp multiply included
#endif
#define G3D_SCENERENDERER_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif

#include <vector>


//============================================================================
//	Forward References
//============================================================================
class g2dRenderTarget;
class g2dWindow;
class g3dPickInfo;
class g3dLayer;
class g3dScene;
class camCamera;


//============================================================================
//============================================================================
class g3dSceneRenderEngine
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dSceneRenderEngine() {};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~g3dSceneRenderEngine() {};

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy plus additional 
	//	viewer specific layers.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	virtual int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
						 const g3dScene &i_Scene,
						 const std::vector<g3dLayer*>& i_ViewerLayers,
						 float i_fSimTime ) = 0;

	//--------------------------------------------------------------------
	//	Let a renderer free up any memory it is holding on to 
	//--------------------------------------------------------------------
	virtual void ReleaseResources() = 0;

	//--------------------------------------------------------------------
	//	A renderer can be enabled or disabled from having Render() called.
	//--------------------------------------------------------------------
	virtual bool IsEnabled() {return true;}

	//--------------------------------------------------------------------
	//	Sometimes we need to access the pre-buffer pixels that don't get 
	//	drawn to a window.
	//--------------------------------------------------------------------
	virtual g2dRenderTarget* GetRawBuffer() {return NULL;}

	//--------------------------------------------------------------------
	//	Sometimes we need to access the buffer's pfd type
	//--------------------------------------------------------------------
	virtual g2dPFD::PixelFormat GetRawBufferPFD() {return g2dPFD::e_Unknown;}
};

