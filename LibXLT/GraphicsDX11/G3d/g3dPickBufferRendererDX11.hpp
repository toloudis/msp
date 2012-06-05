/****************************************************************************\
**	g3dPickBufferRendererDX11.hpp
**
**	The g3dPickBufferRendererDX11 renders the scene with solid colors so
**	that you can tell which object is visible at each point.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_PICKBUFFERRENDERERDX11_HPP
#error g3dPickBufferRendererDX11.hpp multiply included
#endif
#define G3D_PICKBUFFERRENDERERDX11_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class effMaskAlphaData;
class g3dSceneNode;
class g3dLayer;
class maFloatRGBA;
class matMaterial;

class g3dPickBufferRendererDX11 : public g3dPickRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dPickBufferRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~g3dPickBufferRendererDX11();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
				 const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
				 float i_fSimTime );

	void ReleaseResources(){}

	//--------------------------------------------------------------------
	//	set a 1x1 viewport before clearing.
	//--------------------------------------------------------------------
	virtual void SetPickViewport(g2dRenderTarget* i_pTarget);

	//--------------------------------------------------------------------
	//	restore viewport
	//--------------------------------------------------------------------
	virtual void RestoreViewport();

	//--------------------------------------------------------------------
	//	return info about the picked object.
	//--------------------------------------------------------------------
	virtual void GetPickInfo(g2dRenderTarget* i_pWindow, g3dPickInfo& o_PickInfo);

	static void InitStates();
	static void CleanupStates();

private:
	void world_space_render( g3dSceneNode* i_pNode, 
							 const maPoint3d &i_CameraPos,
							 bool i_bRenderLowRes );
	void setup_layer(g3dLayer& i_Layer);
	void render_layer(g3dLayer& i_Layer, const camCamera& i_Camera);

	matMaterial* m_pColorCodeMat;
	effMaskAlphaData* m_pMaskAlphaData;
	envType::UInt32	m_FragmentCount;

	g2dD3D11TexturePtr m_pReadbackSurface;

	int DrawNode(const g3dSceneNode* i_pNode);

	// save current viewport to be restored after pick render.
	D3D11_VIEWPORT m_OldViewport;

	//--------------------------------------------------------------------
	// Static conversion functions for pixel color to object index
	//--------------------------------------------------------------------
	static maFloatRGBA ConvertIndexToColor(envType::UInt32 i_FragmentIndex);
	static envType::UInt32 ConvertColorToIndex(envType::UInt8 i_Red, 
												envType::UInt8 i_Green,
												envType::UInt8 i_Blue);

};

