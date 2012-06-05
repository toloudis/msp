/****************************************************************************\
**	g3dCubeMapRendererDX11.hpp
**
**	The g3dCubeMapRendererDX11 renders the scene from a light's point of 
**	view in order to get the depth map info for shadow mapping.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_CUBEMAPRENDERERDX11_HPP
#error g3dCubeMapRendererDX11.hpp multiply included
#endif
#define G3D_CUBEMAPRENDERERDX11_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class scObject;

class g3dCubeMapRendererDX11 : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dCubeMapRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~g3dCubeMapRendererDX11();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
				 const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
				 float i_fSimTime );

	void ReleaseResources();

	void SetSceneObject(scObject* i_Obj) {m_pObject = i_Obj;}
protected:
	scObject* m_pObject;
};

