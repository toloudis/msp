/*****************************************************************************
**  g3dDX11Util.cpp
**
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dDX11Util.hpp"

#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dHelpersWin.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
//#include "GraphicsDX11/g3d/g3dPixelShaderMgrWin.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"

#include "Core/app/appTime.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/eff/effMaskAlphaData.hpp"
#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPassBuffers.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/g3d/g3dSpotLight.hpp"
#include "Graphics/mat/matMatAnim.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matUVATexture.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/eff/effShaderArray.hpp"
#include "GraphicsDX11/mat/matDX11GlobalWin.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/mat/matStaticCubeTexture.hpp"
#include "GraphicsDX11/g2d/g2dFullscreenQuad.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
//#include "GraphicsDX11/g3d/g3dHelpersWin.hpp"
#include "GraphicsDX11/G3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/G2d/g2dDepthStencilBufferDX11.hpp"

//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D_ERROR(op_result, error_string) \
			if ( op_result != D3D_OK )	\
			{	\
				g2dDX11Global::PrintDXError(op_result);	\
				DBG_ASSERT0(false, error_string);	\
			}	\

namespace
{
	bool l_bCull = true;			//culling enabled
	bool l_bAllowAdditive = true;	// whether to allow additive/multi. changes
	const matMaterial *l_OverrideMaterial = NULL;

	g3dBlendStateMgr::BlendState* stp_FixBakeBlend = NULL;
	g3dBlendStateMgr::BlendState* stp_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* stp_AddBlend = NULL;
	g3dBlendStateMgr::BlendState* stp_MultBlend = NULL;
	g3dBlendStateMgr::BlendState* stp_BlendRestore = NULL;
	g3dBlendStateMgr::BlendState* stp_ColorComponent[5] = {NULL};
	g3dBlendStateMgr::BlendState* stp_NoColor = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Test_Write_Always_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Test_Write_LessE_NS = NULL;
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const matMaterial* get_animated_material(const matMaterial& i_Material)
	{
		const matMaterial* ret_val;
		static matMaterial adjusted_mat;

		if ( i_Material.GetMatAnimList().size() == 0 )
			ret_val = &i_Material;
		else
		{
			const effPhongData* pData = dynamic_cast<const effPhongData*>(i_Material.GetEffectData());
			DBG_ASSERT(pData != NULL, "get_animated_material expects Phong materials");

			// Make simple copy, no material animations
			//adjusted_mat = i_Material;
			adjusted_mat.SimpleCopy(i_Material);
			ret_val = &adjusted_mat;

			std::list<matMatAnim*>::const_iterator it = i_Material.GetMatAnimList().begin();
			std::list<matMatAnim*>::const_iterator end = i_Material.GetMatAnimList().end();
			
			while( it != end )
			{
				const matMatAnim& anim = **it;
				float cur_time = (anim.GetUseRealTime()) ? appTime::GetTime() : g3dSceneGlobal::g_FrameTime;

				if (anim.IsActive())
				{
					switch( anim.GetParamIndex() )
					{
						case matMatParamIndex::e_Ambient:
							adjusted_mat.TypedData<effPhongData>()->m_ColorAmbient = anim.GetColorAnimation()->GetValue(cur_time-anim.GetStartTime());
						break;

						case matMatParamIndex::e_Diffuse:
							adjusted_mat.TypedData<effPhongData>()->m_ColorDiffuse = anim.GetColorAnimation()->GetValue(cur_time-anim.GetStartTime());
							adjusted_mat.TypedData<effPhongData>()->m_Transparency = adjusted_mat.TypedData<effPhongData>()->m_ColorDiffuse.GetAlpha();
						break;

						case matMatParamIndex::e_Specular:
							adjusted_mat.TypedData<effPhongData>()->m_ColorSpecular = anim.GetColorAnimation()->GetValue(cur_time-anim.GetStartTime());
						break;

						case matMatParamIndex::e_Emissive:
							adjusted_mat.TypedData<effPhongData>()->m_ColorEmissive = anim.GetColorAnimation()->GetValue(cur_time-anim.GetStartTime());
						break;

						case matMatParamIndex::e_SpecularPower:
							adjusted_mat.TypedData<effPhongData>()->m_SpecularPower = anim.GetFloatAnimation()->GetValue(cur_time-anim.GetStartTime());
						break;
#if 0
						case matMatParamIndex::e_TextureTranslation0:
							DBG_ASSERT(adjusted_mat.GetNumTextures() > 0, "No texture available for animation");
							adjusted_mat.SetTranslation( anim.GetVectorAnimation()->GetValue(cur_time-anim.GetStartTime()), 0 );
						break;

						case matMatParamIndex::e_TextureTranslation1:
							DBG_ASSERT(adjusted_mat.GetNumTextures() > 1, "No texture available for animation");
							adjusted_mat.SetTranslation( anim.GetVectorAnimation()->GetValue(cur_time-anim.GetStartTime()), 1 );
						break;

						case matMatParamIndex::e_TextureTranslation2:
							DBG_ASSERT(adjusted_mat.GetNumTextures() > 2, "No texture available for animation");
							adjusted_mat.SetTranslation( anim.GetVectorAnimation()->GetValue(cur_time-anim.GetStartTime()), 2 );
						break;

						case matMatParamIndex::e_TextureTranslation3:
							DBG_ASSERT(adjusted_mat.GetNumTextures() > 3, "No texture available for animation");
							adjusted_mat.SetTranslation( anim.GetVectorAnimation()->GetValue(cur_time-anim.GetStartTime()), 3 );
						break;

						case matMatParamIndex::e_TextureScale0:
							DBG_ASSERT(adjusted_mat.GetNumTextures() > 0, "No texture available for animation");
							adjusted_mat.SetScale( anim.GetVectorAnimation()->GetValue(cur_time-anim.GetStartTime()), 0 );
						break;

						case matMatParamIndex::e_TextureScale1:
							DBG_ASSERT(adjusted_mat.GetNumTextures() > 1, "No texture available for animation");
							adjusted_mat.SetScale( anim.GetVectorAnimation()->GetValue(cur_time-anim.GetStartTime()), 1 );
						break;

						case matMatParamIndex::e_TextureScale2:
							DBG_ASSERT(adjusted_mat.GetNumTextures() > 2, "No texture available for animation");
							adjusted_mat.SetScale( anim.GetVectorAnimation()->GetValue(cur_time-anim.GetStartTime()), 2 );
						break;

						case matMatParamIndex::e_TextureScale3:
							DBG_ASSERT(adjusted_mat.GetNumTextures() > 3, "No texture available for animation");
							adjusted_mat.SetScale( anim.GetVectorAnimation()->GetValue(cur_time-anim.GetStartTime()), 3 );
						break;

						case matMatParamIndex::e_TextureRotation0:
							DBG_ASSERT(adjusted_mat.GetNumTextures() > 0, "No texture available for animation");
							adjusted_mat.SetRotation( anim.GetFloatAnimation()->GetValue(cur_time-anim.GetStartTime()), 0 );
						break;

						case matMatParamIndex::e_TextureRotation1:
							DBG_ASSERT(adjusted_mat.GetNumTextures() > 1, "No texture available for animation");
							adjusted_mat.SetRotation( anim.GetFloatAnimation()->GetValue(cur_time-anim.GetStartTime()), 1 );
						break;

						case matMatParamIndex::e_TextureRotation2:
							DBG_ASSERT(adjusted_mat.GetNumTextures() > 2, "No texture available for animation");
							adjusted_mat.SetRotation( anim.GetFloatAnimation()->GetValue(cur_time-anim.GetStartTime()), 2 );
						break;

						case matMatParamIndex::e_TextureRotation3:
							DBG_ASSERT(adjusted_mat.GetNumTextures() > 3, "No texture available for animation");
							adjusted_mat.SetRotation( anim.GetFloatAnimation()->GetValue(g3dSceneGlobal::g_FrameTime-anim.GetStartTime()), 3 );
						break;
#endif
					}
				}

				++it;
			}
		}

		return ret_val;
	}

}

//------------------------------------------------------------------------
// GetEffect - return effect for given material. This will return NULL
//	if no effect is needed.
//------------------------------------------------------------------------
matShaderEffect* g3dDX11Util::GetEffect(const matMaterial &i_Material)
{
	// Check for override material in shader effects also.
	if (l_OverrideMaterial && (&i_Material != l_OverrideMaterial))
	{
		return GetEffect(*l_OverrideMaterial);
	}

	return matShaderMgr::GetEffect(i_Material);
}

matShaderEffect* g3dDX11Util::GetEffect(const std::string& i_effectName)
{
	return matShaderMgr::GetSpecialEffect(i_effectName);
}

//------------------------------------------------------------------------
// Sometimes the shader doesn't need to be rendered at the given time.
// If we check this early, then we can skip some overhead in 
// the rendering.
//------------------------------------------------------------------------
bool g3dDX11Util::CanSkipRender(const matMaterial &i_Material,
							   matShaderEffect* i_pEffect)
{
	if (i_pEffect)
	{	
		if (g3dSingleLightRendering::GetDoSingleLightRendering()
			&& (g3dSingleLightRendering::GetActiveLight() != NULL))
		{
			// If we don't need lighting in this shader, then we can
			// skip the single light passes. 
			if (!i_pEffect->DoesLighting())
			{
				return true;
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------
//	SetAdditiveMode
//------------------------------------------------------------------------
void g3dDX11Util::SetAdditiveMode()
{
	if (!l_bAllowAdditive) return;

	if ( !g3dSceneGlobal::g_AdditiveMode )
	{
		DBG_WARNING("BLEND NOT IMPLEMENTED");
//		g2dDX11Global::g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//		g2dDX11Global::g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
//		g3dSceneGlobal::g_AdditiveMode = true;
	}
}

//------------------------------------------------------------------------
//	SetMultiplicativeMode
//------------------------------------------------------------------------
void g3dDX11Util::SetMultiplicativeMode()
{
	if (!l_bAllowAdditive) return;

	if ( g3dSceneGlobal::g_AdditiveMode )
	{
		DBG_WARNING("BLEND NOT IMPLEMENTED");
//		g2dDX11Global::g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//		g2dDX11Global::g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//		g3dSceneGlobal::g_AdditiveMode = false;
	}
}

//------------------------------------------------------------------------
//	AllowAdditiveChanges - set to false when you want to force
//  all fragments to use the same blending mode (to render lights
//  in multiple passes for instance).  Calls to SetAdditiveMode and
//  SetMultiplicativeMode will be disabled.
//------------------------------------------------------------------------
void g3dDX11Util::AllowAdditiveChanges(bool i_bAllow)
{
	l_bAllowAdditive = i_bAllow;
}

//------------------------------------------------------------------------
//	AllowAdditiveChanges - get current AllowAdditiveChanges setting
//------------------------------------------------------------------------
bool g3dDX11Util::GetAllowAdditiveChanges()
{
	return l_bAllowAdditive;
}

//------------------------------------------------------------------------
// Forces all objects to use the same material. All calls to
//	SetMaterial() will not be effective.  This can be called
//	with NULL to cancel override.
//------------------------------------------------------------------------
void g3dDX11Util::SetOverrideMaterial( const matMaterial *i_Material )
{
	l_OverrideMaterial = i_Material;
}

//------------------------------------------------------------------------
// If we are in the shadow map generation pass, set dithering parameters
// in the effect.
//------------------------------------------------------------------------
void g3dDX11Util::SetupDitheredShadows(bool i_bDither, float i_DitherAlphaBias)
{
	if (l_OverrideMaterial != NULL)
	{
		effMaskAlphaData* shadowMappingData = dynamic_cast<effMaskAlphaData*>(l_OverrideMaterial->GetEffectData());
		if (shadowMappingData != NULL)
		{
			shadowMappingData->m_bDitherTranslucent = i_bDither;
			shadowMappingData->m_DitherAlphaBias = i_DitherAlphaBias;
		}
	}
}

//------------------------------------------------------------------------
//	release_textures - resets all of the texture stages, so that
//	we are not using any textures.
//------------------------------------------------------------------------
void g3dDX11Util::release_textures()
{
	//DBG_TRACE("release_textures does nothing");
}

// get a screen space depth (0..1)
void g3dDX11Util::GetScreenDepth(const g3dSceneNode* i_pNode, const camCamera* i_pCamera, 
					float& o_near, float& o_far)
{
	// temp storage to be reused on subsequent calls
	static maPoint4d	l_box_points[8];

	maAxisBox world_box;
	const g3dFragment* pFrag = i_pNode->GetFragment();
	if (pFrag == NULL)
		return;

	// Convert box to world space
	if ( pFrag->IsModelSpaceBox() )
	{
		const maMatrix4x4& total_transform = i_pNode->GetTotalTransform();

		pFrag->GetBoundingBox().GetBoxPoints( l_box_points );
		// Trasform the points to world space
		for ( int i = 0; i < 8; ++i )
		{
			total_transform.Transform( l_box_points[i] );
			world_box.Union( maPoint3d(l_box_points[i].GetX(), l_box_points[i].GetY(), l_box_points[i].GetZ()) );
		}
	}
	// Already in world space
	else
	{
		world_box.Union( pFrag->GetBoundingBox() );
	}

	maMatrix4x4 worldToScreen = g3dSceneGlobal::GetCameraProjectionTransform();

	world_box.GetBoxPoints( l_box_points );
	maAxisBox screenSpaceBox;
	for ( int i = 0; i < 8; ++i )
	{
		worldToScreen.Transform( l_box_points[i] );
		l_box_points[i].Wdiv();
		screenSpaceBox.Union( maPoint3d(l_box_points[i].GetX(), l_box_points[i].GetY(), l_box_points[i].GetZ()) );
	}
	o_near = screenSpaceBox.GetMinZ();
	o_far = screenSpaceBox.GetMaxZ();
}

maAxisBox g3dDX11Util::XForm(const maAxisBox& i_bounds, const maMatrix4x4& i_xform)
{
	// temp storage to be reused on subsequent calls
	static maPoint3d l_box_pts[8];

	i_bounds.GetBoxPoints( l_box_pts );
	// transform bounds into camera space
	maAxisBox newBounds;
	for ( int i = 0; i < 8; ++i )
	{
		i_xform.Transform( l_box_pts[i] );
		newBounds.Union( l_box_pts[i] );
	}
	return newBounds;
}

void g3dDX11Util::CopyBackBufferToRenderTargetTex(matRenderTargetTexture* io_destination)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dDX11Util::CopyBackBufferToRenderTargetTex" );

	// current render target:
	ID3D11RenderTargetView * pCurrentRenderTarget = NULL;
	ID3D11DepthStencilView * pCurrentDepthStencil = NULL;
	g2dDX11Global::g_pDeviceContext->OMGetRenderTargets(1,
		&pCurrentRenderTarget,
		&pCurrentDepthStencil
		);	
	DBG_ASSERT(pCurrentRenderTarget, "Failed to get render target");

	ID3D11Resource* srcResource = NULL;
	pCurrentRenderTarget->GetResource(&srcResource);

#ifdef _DEBUG
	ID3D11Texture2D* texture = NULL;
	D3D11_TEXTURE2D_DESC dsc;
	srcResource->QueryInterface( __uuidof(ID3D11Texture2D), (LPVOID*)&texture);
	DBG_ASSERT(texture,"bad render target; not a texture2D");
	texture->GetDesc(&dsc);
	UINT height = dsc.Height;
	UINT width = dsc.Width; 
	SAFE_RELEASE( texture )

	DBG_ASSERT(io_destination->GetHeight() == height,"height mismatch");
	DBG_ASSERT(io_destination->GetWidth() == width,"height mismatch");
#endif

	//g2dDX11Global::g_pDeviceContext->CopyResource(io_destination->GetTextureSurface(), srcResource);
	g2dDX11Global::g_pDeviceContext->CopySubresourceRegion(io_destination->GetTextureSurface(), 0,
															0, 0, 0, srcResource, 0, NULL);
	
	SAFE_RELEASE( srcResource );
	SAFE_RELEASE( pCurrentRenderTarget );
	SAFE_RELEASE( pCurrentDepthStencil );

	D3DPERF_EndEvent();
	// now io_destination has the backbuffer in it!
}

//-----------------------------------------------------------------------------
// Name: DrawFullScreenQuad
// Desc: Draw a properly aligned quad covering the entire render target
//	TODO: merge this with g3dRenderFullScreenQuad
//-----------------------------------------------------------------------------
// Screen quad vertex format
//struct ScreenVertex
//{
//	float p[4]; // position
//	float t[2]; // texture coordinate
//
////	static const DWORD FVF;
//};
//const DWORD ScreenVertex::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
// coordinates for a fullscreen quad in D3DFVF_XYZRHW transformed screen space 
// are shifted by -0.5 for the tex coords [0,0]-[1,1] to hit pixels in the corners 
// exactly.   The quad pts are then: [-0.5,-0.5]-[xpixels-0.5, ypixels-0.5]
// this D3DFVF_XYZRHW essentially bypasses the vertex shader's position transforms.

// after worldviewprojection transform, pts are in "projection space" [-1,-1,0]-[1,1,1]. 
// These bounds define the "clip volume". Assuming the d3d viewport is set up with default
// clipping params - clip space is the same as projection space. 
// so a fullscreen quad would need to span [-1,-1,0]-[1,1,0].  (z=0 being the near plane)
// does this account for the 0.5 pixel shift? probably not.

void g3dDX11Util::DrawFullScreenQuad(int i_Width, int i_Height, float offsetPixelsX /*=0*/, float offsetPixelsY /*=0*/)
{
	g2dFullscreenQuad::DrawFullScreenQuad11( i_Width, i_Height, offsetPixelsX, offsetPixelsY);
}

void g3dDX11Util::DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV,
									float offsetPixelsX/*=0*/, float offsetPixelsY/*=0*/)
{
	D3D11_VIEWPORT vp[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	UINT nViewPorts = 1;
	g2dDX11Global::g_pDeviceContext->RSGetViewports( &nViewPorts, vp );

	// Ensure that we're directly mapping texels to pixels by offset by 0.5
    // For more info see the doc page titled "Directly Mapping Texels to Pixels"
	static const float tex2pixX = -0.5f;
	static const float tex2pixY = -0.5f;
	FLOAT pixelOffsetX = offsetPixelsX + tex2pixX;
	FLOAT pixelOffsetY = offsetPixelsY + tex2pixY;

	g2dFullscreenQuad::SCREEN_VERTEX svQuad[4];

    // Draw the quad
	svQuad[0].pos = maVector4d(-1, 1, 0.5f, 1.0f);
	svQuad[0].tex = maVector2d(fLeftU, fTopV);

	svQuad[1].pos = maVector4d(-1, -1, 0.5f, 1.0f);
	svQuad[1].tex = maVector2d(fLeftU, fBottomV);

	svQuad[2].pos = maVector4d(1, 1, 0.5f, 1.0f);
	svQuad[2].tex = maVector2d(fRightU, fTopV);

	svQuad[3].pos = maVector4d(1, -1, 0.5f, 1.0f);
	svQuad[3].tex = maVector2d(fRightU, fBottomV);
/*
	g2dFullscreenQuad::DrawFullScreenQuad11(NULL, 
											(UINT)vp[0].Width, (UINT)vp[0].Height, 
											vp[0].TopLeftX, vp[0].TopLeftY,
											svQuad);
*/

	g2dFullscreenQuad::DrawFullScreenQuad11( (UINT)vp[0].Width, (UINT)vp[0].Height, 
		vp[0].TopLeftX, vp[0].TopLeftY, svQuad );
}

void g3dDX11Util::DrawFullScreenQuadZ(float fLeftU, float fTopV, float fRightU, float fBottomV,
									float offsetPixelsX/*=0*/, float offsetPixelsY/*=0*/)
{
	// This function looks the same with DrawFullScreenQuad in DX11
	DrawFullScreenQuad(fLeftU, fTopV, fRightU, fBottomV, offsetPixelsX, offsetPixelsY);
}

void g3dDX11Util::DrawPartialQuad(float fLeftU, float fTopV, float fRightU, float fBottomV,
						  float fLeftX, float fTopY, float fRightX, float fBottomY)
{

	D3D11_VIEWPORT vp[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	UINT nViewPorts = 1;
	g2dDX11Global::g_pDeviceContext->RSGetViewports( &nViewPorts, vp );

    // Ensure that we're directly mapping texels to pixels by offset by 0.5
    // For more info see the doc page titled "Directly Mapping Texels to Pixels"
	static const FLOAT pixelOffsetX = -0.5f;
	static const FLOAT pixelOffsetY = -0.5f;

	FLOAT fWidth = vp[0].Width;
	FLOAT fHeight = vp[0].Height;

	g2dFullscreenQuad::SCREEN_VERTEX svQuad[4];

	// Draw the quad
	svQuad[0].pos = maVector4d(-1, 1, 0.5f, 1.0f);
	svQuad[0].tex = maVector2d(fLeftU, fTopV);

	svQuad[1].pos = maVector4d(-1, -1, 0.5f, 1.0f);
	svQuad[1].tex = maVector2d(fLeftU, fBottomV);

	svQuad[2].pos = maVector4d(1, 1, 0.5f, 1.0f);
	svQuad[2].tex = maVector2d(fRightU, fTopV);

	svQuad[3].pos = maVector4d(1, -1, 0.5f, 1.0f);
	svQuad[3].tex = maVector2d(fRightU, fBottomV);
/*
	g2dFullscreenQuad::DrawFullScreenQuad11(NULL, 
											(UINT)((fLeftX - fRightX)*fWidth), (UINT)((fTopY - fBottomY)*fHeight), 
											fLeftX*fWidth + pixelOffsetX, fTopY*fHeight + pixelOffsetY,
											svQuad);
*/

	g2dFullscreenQuad::DrawFullScreenQuad11( (UINT)((fLeftX - fRightX)*fWidth), (UINT)((fTopY - fBottomY)*fHeight), 
		fLeftX*fWidth + pixelOffsetX, fTopY*fHeight + pixelOffsetY, svQuad );

	D3DPERF_EndEvent();

}

void g3dDX11Util::CopyTexToTarget(matTexture* src, g2dRenderTarget* tgt, float pixOffsetX/*=0*/, float pixOffsetY/*=0*/)
{
    UINT uiPass;
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dDX11Util::CopyTexToTarget" );
    
	effShaderBaseDX11* pEffBase = (effShaderBaseDX11*)g3dDX11Util::GetEffect("HDRLighting.fx");
	ID3DX11Effect* pEffect = pEffBase->GetD3DXEffect();
	pEffBase->SetTechnique("SimpleCopy");
    
	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions(w,h);

	ID3D11ShaderResourceView* aRes = g3dDX11TextureUtil::GetD3DTexture(src);
	
	int nPasses = pEffBase->Begin();
	for (uiPass = 0; uiPass < nPasses; ++uiPass)
	{
		pEffBase->BeginPass(uiPass);
		g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, &aRes);
		g3dDX11Util::DrawFullScreenQuad( w, h, pixOffsetX, pixOffsetY );
		pEffBase->EndPass();
	}
	pEffBase->End();

	ID3D11ShaderResourceView* nullTex[1] = {NULL};
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, nullTex);

	D3DPERF_EndEvent();
}

void g3dDX11Util::CopyTexToTargetTechnique(matTexture* src, g2dRenderTarget* tgt, std::string& i_Technique)
{
    UINT uiPass;
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dDX11Util::CopyTexToTarget" );
    
	effShaderBaseDX11* pEffBase = (effShaderBaseDX11*)g3dDX11Util::GetEffect("HDRLighting.fx");
	ID3DX11Effect* pEffect = pEffBase->GetD3DXEffect();
	pEffBase->SetTechnique(i_Technique.c_str());
    
	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions(w,h);

	ID3D11ShaderResourceView* aRes = g3dDX11TextureUtil::GetD3DTexture(src);
	
	int nPasses = pEffBase->Begin();
	for (uiPass = 0; uiPass < nPasses; ++uiPass)
	{
		pEffBase->BeginPass(uiPass);
		g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, &aRes);
		g3dDX11Util::DrawFullScreenQuad( w, h, 0,0 );
		pEffBase->EndPass();
	}
	pEffBase->End();

	ID3D11ShaderResourceView* nullTex[1] = {NULL};
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, nullTex);

	D3DPERF_EndEvent();
}

void g3dDX11Util::FixBakedTextureSeams(matTexture* src, matTexture* tmp, int i_nPixels,
									  UVSeamFillMode i_FillMode /*= e_Blend*/)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dDX11Util::FixBakedTextureSeams" );

	// src contains the initial baked info
	// tgt is a holding area
	// src will end up containing the result.

	DBG_ASSERT(tmp != NULL, "shdwAORendererD3D::FixBakedUVSeams: NULL AO rendertarget");
	g2dRenderTarget* tmpTarget = tmp->GetRenderTargetAPI();
	DBG_ASSERT(tmpTarget != NULL, "shdwAORendererD3D::FixBakedUVSeams: TextureDiffuse is not a valid rendertarget");


	tmpTarget->MakeCurrent();
	tmpTarget->Clear(maFloatRGBA(0,0,0,0));

	g3dBlendStateMgr::SetBlendState(stp_NoBlend);
	if (i_FillMode == e_Blend)
	{
		g3dBlendStateMgr::SetBlendState(stp_FixBakeBlend);
	}
	else // e_AlphaTest
	{
		DBG_ERROR("alpha test unimplemented!");
	}

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );

	for (int i = i_nPixels; i > 0; i--)
	{
		// jitter the result by i pixels in 4 directions... blended.
		g3dDX11Util::CopyTexToTarget(src, tmpTarget,
			(float)-i, (float)-i);
		g3dDX11Util::CopyTexToTarget(src, tmpTarget,
			(float)-i, (float)i);
		g3dDX11Util::CopyTexToTarget(src, tmpTarget,
			(float)i, (float)-i);
		g3dDX11Util::CopyTexToTarget(src, tmpTarget,
			(float)i, (float)i);
	}
	g3dDX11Util::CopyTexToTarget(src, tmpTarget);
	// now tmp holds the result texture.

	if (i_FillMode == e_Blend)
	{
		g3dBlendStateMgr::SetBlendState(stp_NoBlend);
	}
	else // e_AlphaTest
	{
		DBG_ERROR("alpha test unimplemented!");
		//g2dDX11Global::g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}

	// blit back to orig tex.
	DBG_ASSERT(src != NULL, "shdwAORendererD3D::FixBakedUVSeams: NULL AO rendertarget");
	g2dRenderTarget* srcTarget = src->GetRenderTargetAPI();
	DBG_ASSERT(srcTarget != NULL, "shdwAORendererD3D::FixBakedUVSeams: TextureDiffuse is not a valid rendertarget");
	
	g3dDX11Util::CopyTexToTarget(tmp, srcTarget);

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	D3DPERF_EndEvent();
}

//copies one texture component into target texture component
void g3dDX11Util::CopyTextureComponent(matTexture* src, g2dRenderTarget* tgt, int i_nSrcComp, int i_nDstComp )
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dDX11Util::CopyTextureComponent" );

	DBG_ASSERT( i_nSrcComp >= 0 && i_nSrcComp < 4, "CopyTextureComponent src outside range.");
	DBG_ASSERT( i_nDstComp >= 0 && i_nDstComp < 4, "CopyTextureComponent dst outside range.");

	UINT uiPass;

	effShaderBaseDX11* pEffBase = (effShaderBaseDX11*)g3dDX11Util::GetEffect("HDRLighting.fx");
	ID3DX11Effect* pEffect = pEffBase->GetD3DXEffect();
	ID3DX11EffectTechnique* pEffectTechnique = pEffect->GetTechniqueByName("CopyComponent");

	pEffect->GetVariableByName("g_nSrcComponent")->AsScalar()->SetInt(i_nSrcComp);
	pEffect->GetVariableByName("g_nDstComponent")->AsScalar()->SetInt(i_nDstComp);

	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions(w,h);

	ID3D11ShaderResourceView* aRes = g3dDX11TextureUtil::GetD3DTexture(src);

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );

	g3dBlendStateMgr::SetBlendState(stp_ColorComponent[i_nDstComp]);

	D3DX11_TECHNIQUE_DESC techDesc;
    pEffectTechnique->GetDesc( &techDesc );
	for (uiPass = 0; uiPass < techDesc.Passes; ++uiPass)
	{
		pEffectTechnique->GetPassByIndex(uiPass)->Apply(0, g2dDX11Global::g_pDeviceContext);
		g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, &aRes);
		g3dDX11Util::DrawFullScreenQuad( w, h );
	}

	ID3D11ShaderResourceView* nullTex[1] = {NULL};
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, nullTex);

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );
	D3DPERF_EndEvent();
}

// copies source texture into dest textures depth
bool g3dDX11Util::CopyTextureToDepth( matTexture* src, g2dRenderTarget* tgt, float pixOffsetX/*=0*/, float pixOffsetY/*=0*/)
{
	UINT uiPass;
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dDX11Util::CopyTextureToDepth" );

	effShaderBaseDX11* pEffBase = (effShaderBaseDX11*)g3dDX11Util::GetEffect("HDRLighting.fx");
	ID3DX11Effect* pEffect = pEffBase->GetD3DXEffect();

	//disable color writes
	g3dBlendStateMgr::BlendState bs_Saved;
	g3dBlendStateMgr::GetCurrentBlendState( bs_Saved );
	g3dBlendStateMgr::SetBlendState( stp_NoColor );

	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_Always_NS );

	ID3DX11EffectTechnique* pEffectTechnique = pEffect->GetTechniqueByName("DepthCopy");

	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions(w,h);

	ID3D11ShaderResourceView* aRes = g3dDX11TextureUtil::GetD3DTexture(src);

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	D3DX11_TECHNIQUE_DESC techDesc;
    pEffectTechnique->GetDesc( &techDesc );
	for (uiPass = 0; uiPass < techDesc.Passes; ++uiPass)
	{
		pEffectTechnique->GetPassByIndex(uiPass)->Apply(0, g2dDX11Global::g_pDeviceContext);
		g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, &aRes);
		g3dDX11Util::DrawFullScreenQuad( w, h, pixOffsetX, pixOffsetY );
	}


	ID3D11ShaderResourceView* nullTex[1] = {NULL};
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, nullTex);

	//restore used states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );
	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	g3dBlendStateMgr::SetBlendState( &bs_Saved );

	D3DPERF_EndEvent();
	return true;
}

// copies source target into dest textures depth
bool g3dDX11Util::CopyDepth( matRenderTargetTexture* src, g2dRenderTarget* tgt, float pixOffsetX/*=0*/, float pixOffsetY/*=0*/)
{
	UINT uiPass;
	shared_ptr<g2dDepthStencilBufferDX11> pDS = boost::dynamic_pointer_cast<g2dDepthStencilBufferDX11>(src->GetDepthStencilBuffer());

	if( !pDS ) return false;	//source has no depth stencil

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dDX11Util::CopyDepth" );

	effShaderBaseDX11* pEffBase = (effShaderBaseDX11*)g3dDX11Util::GetEffect("HDRLighting.fx");
	ID3DX11Effect* pEffect = pEffBase->GetD3DXEffect();

	//disable color writes
	g3dBlendStateMgr::BlendState bs_Saved;
	g3dBlendStateMgr::GetCurrentBlendState( bs_Saved );
	g3dBlendStateMgr::SetBlendState( stp_NoColor );

	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_Always_NS );

	ID3DX11EffectTechnique* pEffectTechnique;
	UINT Slot = 0;
	if( pDS->GetMultisampleCount() > 1 )	//has multisampling
	{
		pEffectTechnique = pEffect->GetTechniqueByName("DepthCopyMS");
		Slot = 8;
	}
	else
	{
		pEffectTechnique = pEffect->GetTechniqueByName("DepthCopy");
	}

	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions(w,h);

	ID3D11ShaderResourceView* aRes = pDS->GetDepthShaderResource();

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	D3DX11_TECHNIQUE_DESC techDesc;
	pEffectTechnique->GetDesc( &techDesc );
	for (uiPass = 0; uiPass < techDesc.Passes; ++uiPass)
	{
		pEffectTechnique->GetPassByIndex(uiPass)->Apply(0, g2dDX11Global::g_pDeviceContext);
		g2dDX11Global::g_pDeviceContext->PSSetShaderResources( Slot, 1, &aRes);
		g3dDX11Util::DrawFullScreenQuad( w, h, pixOffsetX, pixOffsetY );
	}


	ID3D11ShaderResourceView* nullTex[1] = {NULL};
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources( Slot, 1, nullTex);

	//restore used states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );
	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	g3dBlendStateMgr::SetBlendState( &bs_Saved );

	D3DPERF_EndEvent();
	return true;
}


const matMaterial* g3dDX11Util::GetMaterial(const g3dSceneNode* i_pNode)
{
	// Check if the scene node has a material that overides the default fragment material
	const matMaterial* pMaterialOveride = i_pNode->GetMaterial();
	const g3dFragment* pFrag = i_pNode->GetFragment();
	const matMaterial* pMaterial = ( pMaterialOveride ) ? pMaterialOveride : pFrag->GetMaterial();

	if (l_OverrideMaterial && (pMaterial != l_OverrideMaterial))
	{
		pMaterial = l_OverrideMaterial;
	}
	const matMaterial* material = get_animated_material(*pMaterial);
	return material;
}

// set shader globals
void g3dDX11Util::SetupShaderGlobals(matShaderEffect* i_pEffect)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dDX11Util::SetupShaderGlobals" );

	i_pEffect->SetTime(g3dSceneGlobal::g_FrameTime);
	i_pEffect->SetVertexUVBakeMode(g3dSingleLightRendering::GetDoBaking());
	i_pEffect->SetIsolateReflections(g3dSingleLightRendering::GetDoIsolateReflections());
	i_pEffect->SetAlphaTestRef(g3dSceneGlobal::g_AlphaTestRef);
	i_pEffect->SetClipPlane(g3dSceneGlobal::g_ClipPlane);
	D3DPERF_EndEvent();
}

// geometry data
void g3dDX11Util::SetupShaderGeometry(matShaderEffect* i_pEffect, 
									  const g3dSceneNode* i_pNode,
									 int i_MaterialLayerIndex)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dDX11Util::SetupShaderGeometry" );

	const g3dFragment* pFrag = i_pNode->GetFragment();
	DBG_ASSERT(pFrag != NULL, "Node to render for shadows has no fragment");
	i_pEffect->SetIsDoubleSided(pFrag->GetDoubleSided());

	maPoint2d scale, trans;
	pFrag->GetUVBakeFactors(scale, trans);
	i_pEffect->SetBakingFactors(scale, trans);

	i_pEffect->SetupMatrices(pFrag->IsModelSpaceVertices() ? i_pNode->GetTotalTransform() : maMatrix4x4(),
																	 g3dSceneGlobal::GetCameraTransform(), 
																 g3dSceneGlobal::GetProjectionTransform(), 
																		   g3dSceneGlobal::GetCameraPos() );

	if (pFrag->GetHasSkinning() && i_pEffect->GetHasSkinning())
	{
		i_pEffect->SetupSkinningMatrices(pFrag->GetSkinningPalette());
	}

	i_pEffect->SetupTessellatorMeshTexture( pFrag->GetHardwareTesselateMeshTexture() );

	//derive displacement from root material (cannot override material displacement)
	const matMaterial* pMtl = pFrag->GetMaterial();
	matTexture* pTex = NULL;
	float Dscale = 0;
	float Dbias = 0;
	float Dblur = 0;
	maVector2d DObjUVScale;
	if( pMtl )
	{
		// set up uv transform
		maMatrix4x4 muv = pMtl->GetUVTransform(i_MaterialLayerIndex).MakeUVTransform();
		i_pEffect->SetupUVTransform( muv );

		const effDisplacementData& dData = pMtl->GetDisplacementData();

		Dscale = dData.m_Scale;
		Dbias = dData.m_Bias;
		Dblur = dData.m_Blur;
		pTex = dData.m_pDisplacementMap;
		DObjUVScale = dData.m_ObjUVScale;

		((effShaderBaseDX11*)i_pEffect)->SetTessellateValue( (dData.m_TessellationValue*2)+1.0f );
	}

	i_pEffect->SetupDisplacementMap( pTex, Dscale, Dbias, Dblur, DObjUVScale );

	const effNormalsData& nData = pMtl->GetNormalsData();
	i_pEffect->SetupNormalMap( nData.m_pNormalMap, nData.m_BumpScale );

/*
	maVector2d Tess;
	Tess.SetX( g3dPrefs::CurrentPrefs().m_HairTessellation );
	Tess.SetY( g3dPrefs::CurrentPrefs().m_nHairDepthPeelLayers );
	i_pEffect->SetHairTessellationValue( Tess );
*/
	D3DPERF_EndEvent();
}

void g3dDX11Util::SetCullEnable( bool i_bCull )
{
	l_bCull = i_bCull;
}

bool g3dDX11Util::GetCullEnable( )
{
	return l_bCull;
}

D3D11_CULL_MODE g3dDX11Util::GetCullMode()
{
	return l_bCull ? (g3dSingleLightRendering::GetDoReflectionGen() ? D3D11_CULL_FRONT : D3D11_CULL_BACK) : D3D11_CULL_NONE;
}


//------------------------------------------------------------------------
//	Init - initialize global variables
//------------------------------------------------------------------------
void g3dDX11Util::InitStates()
{
	stp_FixBakeBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );
	stp_NoBlend = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );
	stp_ColorComponent[0] = new g3dBlendStateMgr::BlendState(false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED );
	stp_ColorComponent[1] = new g3dBlendStateMgr::BlendState(false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_GREEN );
	stp_ColorComponent[2] = new g3dBlendStateMgr::BlendState(false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_BLUE );
	stp_ColorComponent[3] = new g3dBlendStateMgr::BlendState(false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALPHA );
	stp_ColorComponent[4] = new g3dBlendStateMgr::BlendState(false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );
	stp_NoColor = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ZERO,D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD, 
		0 );
	stp_AddBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
	    D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | 
		D3D11_COLOR_WRITE_ENABLE_GREEN | 
		D3D11_COLOR_WRITE_ENABLE_BLUE );
	stp_MultBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_COLOR,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | 
		D3D11_COLOR_WRITE_ENABLE_GREEN | 
		D3D11_COLOR_WRITE_ENABLE_BLUE );
	stp_BlendRestore = new g3dBlendStateMgr::BlendState( false, FALSE,
		D3D11_BLEND_ZERO,D3D11_BLEND_SRC_COLOR,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ZERO,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL);

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	dsp_Test_Write_Always_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_ALWAYS, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
	dsp_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
	dsp_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

//------------------------------------------------------------------------
//	CleanUp
//------------------------------------------------------------------------
void g3dDX11Util::CleanupStates()
{
	delete stp_FixBakeBlend;
	delete stp_NoBlend;
	delete stp_ColorComponent[0];
	delete stp_ColorComponent[1];
	delete stp_ColorComponent[2];
	delete stp_ColorComponent[3];
	delete stp_ColorComponent[4];
	delete stp_NoColor;
	delete dsp_Test_Write_Always_NS;
	delete dsp_Disable_NS;
	delete dsp_Test_Write_LessE_NS;
	delete stp_BlendRestore;
	delete stp_MultBlend;
	delete stp_AddBlend;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void g3dDX11Util::BlendBuffers(g2dRenderTarget* i_pRenderTarget, matTexture* i_pSrc, int i_BlendOp, float i_Intensity, 
							   float i_Top, float i_Bottom, float i_Left, float i_Right )
{
	
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dDX11Util::BlendBuffers" );

	g3dBlendStateMgr::BlendState* st_Blend;
	if ( i_BlendOp == g3dPassBuffers::e_ADD )
	{
		st_Blend = stp_AddBlend;
	}
	else if ( i_BlendOp == g3dPassBuffers::e_MUL )
	{
		st_Blend = stp_MultBlend;
	}

	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)matShaderMgr::GetSpecialEffect(("HDRLighting.fx"));
	ID3DX11Effect* pEffect = i_pEffect->GetD3DXEffect();

	ID3DX11EffectTechnique* pTechnique = pEffect->GetTechniqueByName("ScaledCopy");

	pEffect->GetVariableByName("g_scaledCopyFactor")->AsScalar()->SetFloat(i_Intensity);

	maVector4d viewport = maVector4d(i_Top,i_Bottom,i_Left,i_Right);
	pEffect->GetVariableByName("g_scaledCopyUVs")->AsVector()->SetFloatVector( viewport.Ptr() );

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	// set render target and source texture
	i_pRenderTarget->MakeCurrent();
	int w,h;
	i_pRenderTarget->GetDimensions(w,h);
	ID3D11ShaderResourceView* pResView = g3dDX11TextureUtil::GetD3DTexture(i_pSrc);

	// turn on stencil - was set up in earlier pass.
	//g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_Ref_Equal, 1 );

	g3dBlendStateMgr::SetBlendState(st_Blend);

	ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(0);
	pPass->Apply(0, g2dDX11Global::g_pDeviceContext);
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources( 0, 1, &pResView );
	g3dDX11Util::DrawFullScreenQuad( w,h );

	// restore stencil state
	//g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );


	g3dBlendStateMgr::SetBlendState(stp_BlendRestore);

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	
	// unbind texture from shader so that it can be set as rendertarget later.
	ID3D11ShaderResourceView* nullPSR[2] = {NULL, NULL};
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0,2,nullPSR);
	//g2dDX11Global::g_pDeviceContext->PSSetShader(NULL, NULL, 0);

	D3DPERF_EndEvent();
}
