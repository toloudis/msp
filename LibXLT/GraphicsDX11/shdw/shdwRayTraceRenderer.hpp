/****************************************************************************\
**	shdwRayTraceRenderer.hpp
**
**	The shdwRayTraceRenderer is a rendering algorithm that uses
**	only projected lights to generate shadows. It also includes many optional
**	post process render passes.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_RAYTRACERENDERER_HPP
#error shdwRayTraceRenderer.hpp multiply included
#endif
#define SHDW_RAYTRACERENDERER_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif

//#ifndef G3D_RENDERPASS_HPP
//#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
//#endif

//#ifndef G3D_RENDERSCREENSPACE_HPP
//#include "GraphicsDX11/g3d/g3dRenderScreenSpace.hpp"
//#endif

//#ifndef G3D_RENDERCAMERASPACE_HPP
//#include "GraphicsDX11/g3d/g3dRenderCameraSpace.hpp"
//#endif

#include <vector>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class g3dLayer;
class camCamera;
class shdwPassTraversal;
class matPlainTexture;
class matRenderTargetTexture;

class shdwRayTraceRenderer : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwRayTraceRenderer();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~shdwRayTraceRenderer();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy plus additional 
	//	viewer specific layers.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	virtual int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
				 const g3dScene &i_Scene,
				 const std::vector<g3dLayer*>& i_ViewerLayers,
				 float i_fSimTime );

	virtual void ReleaseResources();

protected:

};

