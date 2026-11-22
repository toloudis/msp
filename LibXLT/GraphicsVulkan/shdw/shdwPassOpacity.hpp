/****************************************************************************\
**	shdwPassOpacity.hpp
**
**		Render pass to draw opacity values into a light's opacity
**		shadow maps (up to 8 targets of 4 depth layers each), or into its
**		opacity volume
**
**	GraphicsVulkan port of GraphicsDX11's shdwPassOpacity.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_SHDW_PASSOPACITY_HPP
#error shdwPassOpacity.hpp multiply included
#endif
#define GVK_SHDW_PASSOPACITY_HPP

#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/Cam/camCamera.hpp"
#endif

class matTexture;

namespace gvk
{

class shdwPassTraversal;
struct sNodePlusState;
class matRenderTargetTexture;

class shdwPassOpacity : public g3dRenderPass
{
private:
	//recalculates camera projection based on a number of border pixels
	void AdjustCameraOverscan();

public:
	shdwPassOpacity(matRenderTargetTexture* (&i_Targets)[8], const camCamera* i_pCamera, matRenderTargetTexture* i_pSrcDepth = NULL );
	~shdwPassOpacity();

	virtual int Render(float i_time);

	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}
	void SetOverscanSize( int i_pixels ){ m_OverscanPixels = i_pixels; AdjustCameraOverscan(); }

	//---------------------------------------------------------------------------------------
	//  Sets the limits that the Opacity Shadow Map gets scaled to.
	//  Will be clamped to be within the light range
	//---------------------------------------------------------------------------------------
	void SetBounds( float Min, float Max );

	void SetupNodeBounds( g3dSceneNode* i_pSceneNode );

	int RenderNode(const sNodePlusState& i_Node);
	int RenderHairNode(const sNodePlusState& i_Node);
	void SetCamera( const camCamera* i_pCamera );	//sets a new camera
	void SetSrcDepth( matRenderTargetTexture* i_src );
	void SetOpacityVolume( matTexture* i_pOpacityView );

	float m_Near, m_Far;

	static void InitStates();
	static void CleanupStates();

protected:
	shdwPassTraversal* m_sceneInfo;
	const camCamera* m_pOrigCamera;
	camCamera m_Camera;

	int m_OverscanPixels;

	matRenderTargetTexture* m_pTargets[8];
	matRenderTargetTexture* m_pSrcDepthBuffer;	//source depth for depth peel (NULL if not used)

	void CalculateNodeBounds();
	int DrawNodes();
};

}
