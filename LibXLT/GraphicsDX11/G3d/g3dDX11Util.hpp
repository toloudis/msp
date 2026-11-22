/*****************************************************************************
**  g3dDX11Util.hpp
**
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_DX11UTIL_HPP
#error g3dDX11Util.hpp multiply included
#endif
#define G3D_DX11UTIL_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

#include "windows.h"
#include <string>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class camCamera;
class effOcclusionData;
class g2dRenderTarget;
struct g3dAmbientEnvState;
class g3dDirectionalLight;
class g3dPointLight;
class g3dSceneNode;
class g3dSpotLight;
class maAxisBox;
class maFloatRGBA;
class maMatrix4x4;
class matMaterial;
class matRenderTargetTexture;
class matShaderEffect;
class fxEffectDX11;
class matTexture;

namespace g3dDX11Util
{
	//------------------------------------------------------------------------
	//	SetMaterial - sets material's properties into state.
	//------------------------------------------------------------------------
	void SetMaterial( const matMaterial &i_Material,
					  bool i_bUseVertexColors,//DWORD i_VertexShader,
					  const maAxisBox& i_BBox );

	//------------------------------------------------------------------------
	//	SetUVAMaterial - sets UVAmaterial's properties into state.  This is
	//	for UVAs where the frame is being set outside of the SetMaterial call.
	//------------------------------------------------------------------------
	void SetUVAMaterial(const matMaterial &i_Material,
						bool i_bUseVertexColors,//DWORD i_VertexShader,
						const maAxisBox& i_BBox);

	//------------------------------------------------------------------------
	// GetEffect - return effect for given material. This will return NULL
	//	if no effect is needed.
	//------------------------------------------------------------------------
	matShaderEffect* GetEffect(const matMaterial &i_Material);

	//------------------------------------------------------------------------
	// GetEffect - return effect by name. This will do a lookup in the
	// cached list of preloaded effects. Null if not found.
	//------------------------------------------------------------------------
	matShaderEffect* GetEffect(const std::string& i_effectName);

	//------------------------------------------------------------------------
	// GetPlainEffect - the plain-HLSL runtime effect behind a built-in effect
	// that has been converted from .fx. Null if not found or not converted.
	//------------------------------------------------------------------------
	fxEffectDX11* GetPlainEffect(const std::string& i_effectName);

	//------------------------------------------------------------------------
	// Sometimes the shader doesn't need to be rendered at the given time.
	// If we check this early, then we can skip some overhead in 
	// the rendering.
	//------------------------------------------------------------------------
	bool CanSkipRender(const matMaterial &i_Material,
							   matShaderEffect* i_pEffect);

	//------------------------------------------------------------------------
	//	SetAdditiveMode - enables additive rendering
	//------------------------------------------------------------------------
	void SetAdditiveMode();

	//------------------------------------------------------------------------
	//	SetMultiplicativeMode - enables multiplicative rendering
	//------------------------------------------------------------------------
	void SetMultiplicativeMode();

	//------------------------------------------------------------------------
	//	AllowAdditiveChanges - set to false when you want to force
	//  all fragments to use the same blending mode (to render lights
	//  in multiple passes for instance).  Calls to SetAdditiveMode and
	//  SetMultiplicativeMode will be disabled.
	//------------------------------------------------------------------------
	void AllowAdditiveChanges(bool i_bAllow);

	//------------------------------------------------------------------------
	//	AllowAdditiveChanges - get current AllowAdditiveChanges setting
	//------------------------------------------------------------------------
	bool GetAllowAdditiveChanges();

	//------------------------------------------------------------------------
	// Forces all objects to use the same material. All calls to
	//	SetMaterial() will not be effective.  This can be called
	//	with NULL to cancel override.
	//------------------------------------------------------------------------
	void SetOverrideMaterial( const matMaterial *i_Material );

	//------------------------------------------------------------------------
	// If we are in the shadow map generation pass, set dithering parameters
	// in the effect.
	//------------------------------------------------------------------------
	void SetupDitheredShadows(bool i_bDither, float i_DitherAlphaBias);

	//------------------------------------------------------------------------
	//	release_textures - resets all of the texture stages, so that
	//	we are not using any textures.
	//------------------------------------------------------------------------
	void release_textures();

	//------------------------------------------------------------------------
	// Get the near and far depths of the given node in screen space.
	//------------------------------------------------------------------------
	void GetScreenDepth(const g3dSceneNode* i_pNode, const camCamera* i_pCamera, 
		float& o_near, float& o_far);

	//------------------------------------------------------------------------
	// Transform a bounding box.
	//------------------------------------------------------------------------
	maAxisBox XForm(const maAxisBox& i_bounds, const maMatrix4x4& i_xform);

	//------------------------------------------------------------------------
	// Copy the contents of the back buffer (the current render target)
	// into the given render target.
	//------------------------------------------------------------------------
	void CopyBackBufferToRenderTargetTex(matRenderTargetTexture* io_destination);

	//------------------------------------------------------------------------
	// Draw a simple full screen quad covering the current render target.
	//------------------------------------------------------------------------
	void DrawFullScreenQuad(int i_Width, int i_Height, float offsetPixelsX=0, float offsetPixelsY=0);
	void DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV,
		float offsetPixelsX=0, float offsetPixelsY=0);
	void DrawFullScreenQuadZ(float fLeftU, float fTopV, float fRightU, float fBottomV,
		float offsetPixelsX=0, float offsetPixelsY=0);
	void DrawPartialQuad(float fLeftU, float fTopV, float fRightU, float fBottomV,
						  float fLeftX, float fTopY, float fRightX, float fBottomY);

	void SetCullEnable( bool i_bCull );	//sets to false for cull none, otherwise gets active cull mode
	bool GetCullEnable();
	D3D11_CULL_MODE GetCullMode();

	void CopyTexToTarget( matTexture* src, g2dRenderTarget* tgt, float pixOffsetX=0, float pixOffsetY=0);
	void CopyTexToTargetTechnique( matTexture* src, g2dRenderTarget* tgt, std::string& i_Technique);

	//--------------------------------------------------------------------
	//copies one texture component into target texture component
	//--------------------------------------------------------------------
	void CopyTextureComponent( matTexture* src, g2dRenderTarget* tgt, int i_nSrcComp, int i_nDstComp );

	// copies source texture into dest textures depth
	bool CopyTextureToDepth( matTexture* src, g2dRenderTarget* tgt, float pixOffsetX=0, float pixOffsetY=0);

	// copies source targets depth into dest textures depth
	// returns false if the targets don't have depth buffers or they don't match
	// only render targets can have depth
	bool CopyDepth( matRenderTargetTexture* src, g2dRenderTarget* tgt, float pixOffsetX=0, float pixOffsetY=0);

	const matMaterial* GetMaterial(const g3dSceneNode* i_pNode);

	// set shader globals
	void SetupShaderGlobals(matShaderEffect* i_pEffect);

	// geometry data
	void SetupShaderGeometry(matShaderEffect* i_pEffect, 
		const g3dSceneNode* i_pNode,
		int i_MaterialLayerIndex = 0);

	enum UVSeamFillMode {e_Blend, e_AlphaTest};
	void FixBakedTextureSeams(matTexture* src, matTexture* tmp, int i_nPixels, UVSeamFillMode i_FillMode = e_Blend);

	void BlendBuffers(g2dRenderTarget* i_pRenderTarget, matTexture* i_pSrc, int i_BlendOp, float i_Intensity, 
					  float i_Top, float i_Bottom, float i_Left, float i_Right );

	//------------------------------------------------------------------------
	//	Init - initialize global variables
	//------------------------------------------------------------------------
	void InitStates();

	//------------------------------------------------------------------------
	//	CleanUp
	//------------------------------------------------------------------------
	void CleanupStates();
}
