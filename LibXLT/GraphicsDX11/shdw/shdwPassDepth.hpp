/****************************************************************************\
**	shdwPassDepth.cpp
**
**		Render pass to draw depth values to target
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_PASSDEPTH_HPP
#error shdwPassDepth.hpp multiply included
#endif
#define SHDW_PASSDEPTH_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/Cam/camCamera.hpp"
#endif 

class shdwPassTraversal;
struct sNodePlusState;
class matTexture;

class shdwPassDepth : public g3dRenderPass
{
public:

	enum eTechnique
	{
		eDefault = 0,
		eScreenPeelLess,	//Uses depth peeling with a less equal compare (requires depth texture)
		eScreenPeelGreater,	//Uses depth peeling with a greater equal compare (requires depth texture)
		eScreenBiased,		//Default technique with a biased depth write
		eScreenAlpha,		//default technique with alpha set to allow all but 0 through
		eView,				//Renders view space depths
		eViewPeelLess,		//View space depth peeling with a less equal compare
		eViewPeelGreater,	//View space depth peeling with a greater equal compare
		eNView,				//Renders normalized view space depths
		eNViewPeelLess,		//Normalized view space depth peeling with a less equal compare
		eNViewPeelGreater,	//Normalized view space depth peeling with a greater equal compare
	};
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassDepth();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassDepth(g2dRenderTarget* i_pDepthTarget, const camCamera* i_pCamera, bool i_bRenderInvisible = false, 
		matTexture* i_pSrcDepth = NULL, eTechnique i_eTech = eDefault, bool i_bDepthOnly = false );

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	~shdwPassDepth();

	//----------------------------------------------------------------------------------------
	// This function renders non-transparent objects depth values into the target
	// Note that the target will always be cleared.
	//----------------------------------------------------------------------------------------
	virtual int Render(float i_time);

	//----------------------------------------------------------------------------------------
	// This function renders only transparent objects depth values into the target
	// Note that the target will only be cleared only in reversed mode (near to far)
	//----------------------------------------------------------------------------------------
	int RenderTransparent();

	//----------------------------------------------------------------------------------------
	// This function renders only hair objects depth values into the target
	// Note that the target will only be cleared only in reversed mode (near to far)
	//----------------------------------------------------------------------------------------
	int RenderHair();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	void SetOverscanSize( int i_pixels ){ m_OverscanPixels = i_pixels; AdjustCameraOverscan(); }

	//----------------------------------------------------------------------------------------
	// This function renders a single node
	// if no source is specified then all depths will be rendered
	//   (an optional transparency value will be tested to remove non solid pixels)
	// otherwise one of two depth peeling modes will be used
	//  if reversed then only depths that are farther away from the source will be stored (for back to front sorting or farthest z)
	//  else only depths that are closer than the source will be stored (for front to back sorting or nearest z)
	//----------------------------------------------------------------------------------------
	int RenderNode(const sNodePlusState& i_Node);

	//----------------------------------------------------------------------------------------
	// This function renders a single node with the special hair shader
	// if no source is specified then all depths will be rendered
	//   (an optional transparency value will be tested to remove non solid pixels)
	// otherwise one of two depth peeling modes will be used
	//  if reversed then only depths that are farther away from the source will be stored (for back to front sorting or farthest z)
	//  else only depths that are closer than the source will be stored (for front to back sorting or nearest z)
	//----------------------------------------------------------------------------------------
	int RenderHairNode(const sNodePlusState& i_Node);

	//----------------------------------------------------------------------------------------
	//renders a solid color to the frame buffer (used to clear float buffers to values outside 0-1)
	//----------------------------------------------------------------------------------------
	void ColorTexture(g2dRenderTarget* i_pRenderTarget, maFloatRGBA& i_Color );

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	static void InitStates();
	static void CleanupStates();

protected:
	shdwPassTraversal* m_sceneInfo;
	const camCamera* m_pOrigCamera;
	camCamera m_Camera;

	void SetupMinimalNearFar();
	float m_Near, m_Far;
	int m_OverscanPixels;
	
	//depth peeling is reversed (back to front)
	bool m_bCompareLess;		

	bool m_bRenderInvisible;
	bool m_bOnlyDepths;

	eTechnique m_eTechnique;

	//source depth for depth peel (NULL if not used)
	matTexture* m_pSrcDepthBuffer;	

private:
	//----------------------------------------------------------------------------------------
	//recalculates camera projection based on a number of border pixels
	//----------------------------------------------------------------------------------------
	void AdjustCameraOverscan();
};
