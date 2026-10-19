/*****************************************************************************
**  g3dVkUtil.cpp
**
**	See g3dVkUtil.hpp.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif

#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
//#include "GraphicsVulkan/G3d/g3dPixelShaderMgrWin.hpp"
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif

#ifndef APP_TIME_HPP
#include "Core/app/appTime.hpp"
#endif
#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
#endif
#ifndef EFF_MASKALPHADATA_HPP
#include "Graphics/eff/effMaskAlphaData.hpp"
#endif
#ifndef EFF_OCCLUSIONDATA_HPP
#include "Graphics/eff/effOcclusionData.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/eff/effPhongData.hpp"
#endif
#ifndef G3D_DIRECTIONALLIGHT_HPP
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif
#ifndef G3D_PASSBUFFERS_HPP
#include "Graphics/g3d/g3dPassBuffers.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/g3d/g3dProjectedLight.hpp"
#endif
#ifndef G3D_RENDERSTATE_HPP
#include "Graphics/g3d/g3dRenderState.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef G3D_SPOTLIGHT_HPP
#include "Graphics/g3d/g3dSpotLight.hpp"
#endif
#ifndef MAT_MATANIM_HPP
#include "Graphics/mat/matMatAnim.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_UVATEXTURE_HPP
#include "Graphics/mat/matUVATexture.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_EFF_SHADERARRAY_HPP
#include "GraphicsVulkan/Eff/effShaderArray.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_MATSTATICCUBETEXTURE_HPP
#include "GraphicsVulkan/Mat/matStaticCubeTexture.hpp"
#endif
#ifndef GVK_G2DFULLSCREENQUAD_HPP
#include "GraphicsVulkan/G2d/g2dFullscreenQuad.hpp"
#endif
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G2DDEPTHSTENCILBUFFERVK_HPP
#include "GraphicsVulkan/G2d/g2dDepthStencilBufferVk.hpp"
#endif
#ifndef GVK_EFF_PLAINSHADERVK_HPP
#include "GraphicsVulkan/Eff/effPlainShaderVk.hpp"
#endif
#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif

namespace gvk
{

namespace
{
	//	PSSetShaderResources(i_Slot, 1, &i_pImage): NULL binds the transparent
	//	black image, as D3D11 reads a NULL view
	void set_texture(uint32_t i_Slot, vkdImage* i_pImage)
	{
		vkdContext& context = vkdGlobal::Context();
		vkdImage* image = i_pImage ? i_pImage : context.NullTexture();
		context.Bindings().SetTexture(i_Slot, image->SampledIndex(context.Bindings()));
	}

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
matShaderEffect* g3dVkUtil::GetEffect(const matMaterial &i_Material)
{
	// Check for override material in shader effects also.
	if (l_OverrideMaterial && (&i_Material != l_OverrideMaterial))
	{
		return GetEffect(*l_OverrideMaterial);
	}

	return matShaderMgr::GetEffect(i_Material);
}

matShaderEffect* g3dVkUtil::GetEffect(const std::string& i_effectName)
{
	return matShaderMgr::GetSpecialEffect(i_effectName);
}

fxEffectVk* g3dVkUtil::GetPlainEffect(const std::string& i_effectName)
{
	effPlainShaderVk* pEffect = dynamic_cast<effPlainShaderVk*>(GetEffect(i_effectName));
	return pEffect ? pEffect->GetEffect() : NULL;
}

//------------------------------------------------------------------------
// Sometimes the shader doesn't need to be rendered at the given time.
// If we check this early, then we can skip some overhead in 
// the rendering.
//------------------------------------------------------------------------
bool g3dVkUtil::CanSkipRender(const matMaterial &i_Material,
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
void g3dVkUtil::SetAdditiveMode()
{
	if (!l_bAllowAdditive) return;

	if ( !g3dSceneGlobal::g_AdditiveMode )
	{
		DBG_WARNING("BLEND NOT IMPLEMENTED");
//		g2dVkGlobal::g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//		g2dVkGlobal::g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
//		g3dSceneGlobal::g_AdditiveMode = true;
	}
}

//------------------------------------------------------------------------
//	SetMultiplicativeMode
//------------------------------------------------------------------------
void g3dVkUtil::SetMultiplicativeMode()
{
	if (!l_bAllowAdditive) return;

	if ( g3dSceneGlobal::g_AdditiveMode )
	{
		DBG_WARNING("BLEND NOT IMPLEMENTED");
//		g2dVkGlobal::g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//		g2dVkGlobal::g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//		g3dSceneGlobal::g_AdditiveMode = false;
	}
}

//------------------------------------------------------------------------
//	AllowAdditiveChanges - set to false when you want to force
//  all fragments to use the same blending mode (to render lights
//  in multiple passes for instance).  Calls to SetAdditiveMode and
//  SetMultiplicativeMode will be disabled.
//------------------------------------------------------------------------
void g3dVkUtil::AllowAdditiveChanges(bool i_bAllow)
{
	l_bAllowAdditive = i_bAllow;
}

//------------------------------------------------------------------------
//	AllowAdditiveChanges - get current AllowAdditiveChanges setting
//------------------------------------------------------------------------
bool g3dVkUtil::GetAllowAdditiveChanges()
{
	return l_bAllowAdditive;
}

//------------------------------------------------------------------------
// Forces all objects to use the same material. All calls to
//	SetMaterial() will not be effective.  This can be called
//	with NULL to cancel override.
//------------------------------------------------------------------------
void g3dVkUtil::SetOverrideMaterial( const matMaterial *i_Material )
{
	l_OverrideMaterial = i_Material;
}

//------------------------------------------------------------------------
// If we are in the shadow map generation pass, set dithering parameters
// in the effect.
//------------------------------------------------------------------------
void g3dVkUtil::SetupDitheredShadows(bool i_bDither, float i_DitherAlphaBias)
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
void g3dVkUtil::release_textures()
{
	//DBG_TRACE("release_textures does nothing");
}

// get a screen space depth (0..1)
void g3dVkUtil::GetScreenDepth(const g3dSceneNode* i_pNode, const camCamera* i_pCamera, 
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

maAxisBox g3dVkUtil::XForm(const maAxisBox& i_bounds, const maMatrix4x4& i_xform)
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

void g3dVkUtil::CopyBackBufferToRenderTargetTex(matRenderTargetTexture* io_destination)
{

	// current render target:
	vkdContext& context = vkdGlobal::Context();
	vkdImage* colors[8] = {};
	vkdImage* depth = NULL;
	context.GetRenderTargets(colors, &depth);
	vkdImage* pCurrentRenderTarget = colors[0];
	DBG_ASSERT(pCurrentRenderTarget, "Failed to get render target");
	if (!pCurrentRenderTarget)
		return;

#ifdef _DEBUG
	uint32_t height = pCurrentRenderTarget->Height();
	uint32_t width = pCurrentRenderTarget->Width();

	DBG_ASSERT(io_destination->GetHeight() == height,"height mismatch");
	DBG_ASSERT(io_destination->GetWidth() == width,"height mismatch");
#endif

	//	CopySubresourceRegion of subresource 0 to (0, 0): D3D11 copies the
	//	whole source subresource
	std::lock_guard<std::recursive_mutex> lock(vkdGlobal::Device().StreamLock());
	VkImageCopy region = {};
	region.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, pCurrentRenderTarget->Range().baseMipLevel, pCurrentRenderTarget->Range().baseArrayLayer, 1 };
	region.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	region.extent = { pCurrentRenderTarget->Width(), pCurrentRenderTarget->Height(), 1 };
	vkCmdCopyImage(context.Commands(), pCurrentRenderTarget->Image(), VK_IMAGE_LAYOUT_GENERAL,
				   io_destination->GetTextureSurface()->Image(), VK_IMAGE_LAYOUT_GENERAL, 1, &region);

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
////	static const uint32_t FVF;
//};
//const uint32_t ScreenVertex::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
// coordinates for a fullscreen quad in D3DFVF_XYZRHW transformed screen space 
// are shifted by -0.5 for the tex coords [0,0]-[1,1] to hit pixels in the corners 
// exactly.   The quad pts are then: [-0.5,-0.5]-[xpixels-0.5, ypixels-0.5]
// this D3DFVF_XYZRHW essentially bypasses the vertex shader's position transforms.

// after worldviewprojection transform, pts are in "projection space" [-1,-1,0]-[1,1,1]. 
// These bounds define the "clip volume". Assuming the d3d viewport is set up with default
// clipping params - clip space is the same as projection space. 
// so a fullscreen quad would need to span [-1,-1,0]-[1,1,0].  (z=0 being the near plane)
// does this account for the 0.5 pixel shift? probably not.

void g3dVkUtil::DrawFullScreenQuad(int i_Width, int i_Height, float offsetPixelsX /*=0*/, float offsetPixelsY /*=0*/)
{
	g2dFullscreenQuad::DrawFullScreenQuad( i_Width, i_Height, offsetPixelsX, offsetPixelsY);
}

void g3dVkUtil::DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV,
									float offsetPixelsX/*=0*/, float offsetPixelsY/*=0*/)
{
	vkdViewport vp[vkdContext::c_MaxViewports];
	vkdGlobal::Context().GetViewports( vp );

	// Ensure that we're directly mapping texels to pixels by offset by 0.5
    // For more info see the doc page titled "Directly Mapping Texels to Pixels"
	static const float tex2pixX = -0.5f;
	static const float tex2pixY = -0.5f;
	float pixelOffsetX = offsetPixelsX + tex2pixX;
	float pixelOffsetY = offsetPixelsY + tex2pixY;

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
	g2dFullscreenQuad::DrawFullScreenQuad(NULL, 
											(uint32_t)vp[0].m_Width, (uint32_t)vp[0].m_Height, 
											vp[0].m_X, vp[0].m_Y,
											svQuad);
*/

	g2dFullscreenQuad::DrawFullScreenQuad( (uint32_t)vp[0].m_Width, (uint32_t)vp[0].m_Height, 
		vp[0].m_X, vp[0].m_Y, svQuad );
}

void g3dVkUtil::DrawFullScreenQuadZ(float fLeftU, float fTopV, float fRightU, float fBottomV,
									float offsetPixelsX/*=0*/, float offsetPixelsY/*=0*/)
{
	// This function looks the same with DrawFullScreenQuad in Vk
	DrawFullScreenQuad(fLeftU, fTopV, fRightU, fBottomV, offsetPixelsX, offsetPixelsY);
}

void g3dVkUtil::DrawPartialQuad(float fLeftU, float fTopV, float fRightU, float fBottomV,
						  float fLeftX, float fTopY, float fRightX, float fBottomY)
{

	vkdViewport vp[vkdContext::c_MaxViewports];
	vkdGlobal::Context().GetViewports( vp );

    // Ensure that we're directly mapping texels to pixels by offset by 0.5
    // For more info see the doc page titled "Directly Mapping Texels to Pixels"
	static const float pixelOffsetX = -0.5f;
	static const float pixelOffsetY = -0.5f;

	float fWidth = vp[0].m_Width;
	float fHeight = vp[0].m_Height;

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
	g2dFullscreenQuad::DrawFullScreenQuad(NULL, 
											(uint32_t)((fLeftX - fRightX)*fWidth), (uint32_t)((fTopY - fBottomY)*fHeight), 
											fLeftX*fWidth + pixelOffsetX, fTopY*fHeight + pixelOffsetY,
											svQuad);
*/

	g2dFullscreenQuad::DrawFullScreenQuad( (uint32_t)((fLeftX - fRightX)*fWidth), (uint32_t)((fTopY - fBottomY)*fHeight), 
		fLeftX*fWidth + pixelOffsetX, fTopY*fHeight + pixelOffsetY, svQuad );


}

void g3dVkUtil::CopyTexToTarget(matTexture* src, g2dRenderTarget* tgt, float pixOffsetX/*=0*/, float pixOffsetY/*=0*/)
{
    uint32_t uiPass;
    
	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");
	pEffBase->SetTechnique("SimpleCopy");
    
	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions(w,h);

	vkdImage* aRes = g3dVkTextureUtil::GetImage(src);
	
	int nPasses = pEffBase->Begin();
	for (uiPass = 0; uiPass < nPasses; ++uiPass)
	{
		pEffBase->BeginPass(uiPass);
		set_texture(0, aRes);
		g3dVkUtil::DrawFullScreenQuad( w, h, pixOffsetX, pixOffsetY );
		pEffBase->EndPass();
	}
	pEffBase->End();

	set_texture(0, NULL);

}

void g3dVkUtil::CopyTexToTargetTechnique(matTexture* src, g2dRenderTarget* tgt, std::string& i_Technique)
{
    uint32_t uiPass;
    
	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");
	pEffBase->SetTechnique(i_Technique.c_str());
    
	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions(w,h);

	vkdImage* aRes = g3dVkTextureUtil::GetImage(src);
	
	int nPasses = pEffBase->Begin();
	for (uiPass = 0; uiPass < nPasses; ++uiPass)
	{
		pEffBase->BeginPass(uiPass);
		set_texture(0, aRes);
		g3dVkUtil::DrawFullScreenQuad( w, h, 0,0 );
		pEffBase->EndPass();
	}
	pEffBase->End();

	set_texture(0, NULL);

}

void g3dVkUtil::FixBakedTextureSeams(matTexture* src, matTexture* tmp, int i_nPixels,
									  UVSeamFillMode i_FillMode /*= e_Blend*/)
{

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

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );

	for (int i = i_nPixels; i > 0; i--)
	{
		// jitter the result by i pixels in 4 directions... blended.
		g3dVkUtil::CopyTexToTarget(src, tmpTarget,
			(float)-i, (float)-i);
		g3dVkUtil::CopyTexToTarget(src, tmpTarget,
			(float)-i, (float)i);
		g3dVkUtil::CopyTexToTarget(src, tmpTarget,
			(float)i, (float)-i);
		g3dVkUtil::CopyTexToTarget(src, tmpTarget,
			(float)i, (float)i);
	}
	g3dVkUtil::CopyTexToTarget(src, tmpTarget);
	// now tmp holds the result texture.

	if (i_FillMode == e_Blend)
	{
		g3dBlendStateMgr::SetBlendState(stp_NoBlend);
	}
	else // e_AlphaTest
	{
		DBG_ERROR("alpha test unimplemented!");
		//g2dVkGlobal::g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	}

	// blit back to orig tex.
	DBG_ASSERT(src != NULL, "shdwAORendererD3D::FixBakedUVSeams: NULL AO rendertarget");
	g2dRenderTarget* srcTarget = src->GetRenderTargetAPI();
	DBG_ASSERT(srcTarget != NULL, "shdwAORendererD3D::FixBakedUVSeams: TextureDiffuse is not a valid rendertarget");
	
	g3dVkUtil::CopyTexToTarget(tmp, srcTarget);

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

}

//copies one texture component into target texture component
void g3dVkUtil::CopyTextureComponent(matTexture* src, g2dRenderTarget* tgt, int i_nSrcComp, int i_nDstComp )
{

	DBG_ASSERT( i_nSrcComp >= 0 && i_nSrcComp < 4, "CopyTextureComponent src outside range.");
	DBG_ASSERT( i_nDstComp >= 0 && i_nDstComp < 4, "CopyTextureComponent dst outside range.");

	uint32_t uiPass;

	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");
	int technique = pEffect->FindTechnique("CopyComponent");

	pEffect->SetConstant(pEffect->FindConstant("g_nSrcComponent"), (int)(i_nSrcComp));
	pEffect->SetConstant(pEffect->FindConstant("g_nDstComponent"), (int)(i_nDstComp));

	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions(w,h);

	vkdImage* aRes = g3dVkTextureUtil::GetImage(src);

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );

	g3dBlendStateMgr::SetBlendState(stp_ColorComponent[i_nDstComp]);

    uint32_t nTechPasses = (uint32_t)pEffect->GetPassCount(technique);
	for (uiPass = 0; uiPass < nTechPasses; ++uiPass)
	{
		pEffect->Apply(technique, uiPass);
		set_texture(0, aRes);
		g3dVkUtil::DrawFullScreenQuad( w, h );
	}

	set_texture(0, NULL);

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );
}

// copies source texture into dest textures depth
bool g3dVkUtil::CopyTextureToDepth( matTexture* src, g2dRenderTarget* tgt, float pixOffsetX/*=0*/, float pixOffsetY/*=0*/)
{
	uint32_t uiPass;

	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");

	//disable color writes
	g3dBlendStateMgr::BlendState bs_Saved;
	g3dBlendStateMgr::GetCurrentBlendState( bs_Saved );
	g3dBlendStateMgr::SetBlendState( stp_NoColor );

	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_Always_NS );

	int technique = pEffect->FindTechnique("DepthCopy");

	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions(w,h);

	vkdImage* aRes = g3dVkTextureUtil::GetImage(src);

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

    uint32_t nTechPasses = (uint32_t)pEffect->GetPassCount(technique);
	for (uiPass = 0; uiPass < nTechPasses; ++uiPass)
	{
		pEffect->Apply(technique, uiPass);
		set_texture(0, aRes);
		g3dVkUtil::DrawFullScreenQuad( w, h, pixOffsetX, pixOffsetY );
	}


	set_texture(0, NULL);

	//restore used states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );
	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	g3dBlendStateMgr::SetBlendState( &bs_Saved );

	return true;
}

// copies source target into dest textures depth
bool g3dVkUtil::CopyDepth( matRenderTargetTexture* src, g2dRenderTarget* tgt, float pixOffsetX/*=0*/, float pixOffsetY/*=0*/)
{
	uint32_t uiPass;
	shared_ptr<g2dDepthStencilBufferVk> pDS = std::dynamic_pointer_cast<g2dDepthStencilBufferVk>(src->GetDepthStencilBuffer());

	if( !pDS ) return false;	//source has no depth stencil


	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");

	//disable color writes
	g3dBlendStateMgr::BlendState bs_Saved;
	g3dBlendStateMgr::GetCurrentBlendState( bs_Saved );
	g3dBlendStateMgr::SetBlendState( stp_NoColor );

	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_Always_NS );

	int technique = -1;
	uint32_t Slot = 0;
	if( pDS->GetMultisampleCount() > 1 )	//has multisampling
	{
		technique = pEffect->FindTechnique("DepthCopyMS");
		Slot = 8;
	}
	else
	{
		technique = pEffect->FindTechnique("DepthCopy");
	}

	tgt->MakeCurrent();
	int w,h;
	tgt->GetDimensions(w,h);

	vkdImage* aRes = pDS->GetDepthShaderResource();

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	uint32_t nTechPasses = (uint32_t)pEffect->GetPassCount(technique);
	for (uiPass = 0; uiPass < nTechPasses; ++uiPass)
	{
		pEffect->Apply(technique, uiPass);
		set_texture(Slot, aRes);
		g3dVkUtil::DrawFullScreenQuad( w, h, pixOffsetX, pixOffsetY );
	}


	set_texture(Slot, NULL);

	//restore used states
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );
	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	g3dBlendStateMgr::SetBlendState( &bs_Saved );

	return true;
}


const matMaterial* g3dVkUtil::GetMaterial(const g3dSceneNode* i_pNode)
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
void g3dVkUtil::SetupShaderGlobals(matShaderEffect* i_pEffect)
{

	i_pEffect->SetTime(g3dSceneGlobal::g_FrameTime);
	i_pEffect->SetVertexUVBakeMode(g3dSingleLightRendering::GetDoBaking());
	i_pEffect->SetIsolateReflections(g3dSingleLightRendering::GetDoIsolateReflections());
	i_pEffect->SetAlphaTestRef(g3dSceneGlobal::g_AlphaTestRef);
	i_pEffect->SetClipPlane(g3dSceneGlobal::g_ClipPlane);
}

// geometry data
void g3dVkUtil::SetupShaderGeometry(matShaderEffect* i_pEffect, 
									  const g3dSceneNode* i_pNode,
									 int i_MaterialLayerIndex)
{

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

		((effShaderBaseVk*)i_pEffect)->SetTessellateValue( (dData.m_TessellationValue*2)+1.0f );
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
}

void g3dVkUtil::SetCullEnable( bool i_bCull )
{
	l_bCull = i_bCull;
}

bool g3dVkUtil::GetCullEnable( )
{
	return l_bCull;
}

VkCullModeFlags g3dVkUtil::GetCullMode()
{
	return l_bCull ? (g3dSingleLightRendering::GetDoReflectionGen() ? VK_CULL_MODE_FRONT_BIT : VK_CULL_MODE_BACK_BIT) : VK_CULL_MODE_NONE;
}


//------------------------------------------------------------------------
//	Init - initialize global variables
//------------------------------------------------------------------------
void g3dVkUtil::InitStates()
{
	stp_FixBakeBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD, 
		(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT) );
	stp_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, 
		(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT) );
	stp_ColorComponent[0] = new g3dBlendStateMgr::BlendState(false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_R_BIT );
	stp_ColorComponent[1] = new g3dBlendStateMgr::BlendState(false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_G_BIT );
	stp_ColorComponent[2] = new g3dBlendStateMgr::BlendState(false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_B_BIT );
	stp_ColorComponent[3] = new g3dBlendStateMgr::BlendState(false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_A_BIT );
	stp_ColorComponent[4] = new g3dBlendStateMgr::BlendState(false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, 
		(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT) );
	stp_NoColor = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, 
		0 );
	stp_AddBlend = new g3dBlendStateMgr::BlendState( false, true,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_R_BIT | 
		VK_COLOR_COMPONENT_G_BIT | 
		VK_COLOR_COMPONENT_B_BIT );
	stp_MultBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_COLOR,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_R_BIT | 
		VK_COLOR_COMPONENT_G_BIT | 
		VK_COLOR_COMPONENT_B_BIT );
	stp_BlendRestore = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_COLOR,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_OP_ADD, 
		(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT));

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	dsp_Test_Write_Always_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_ALWAYS, false, 0xff, 0xff, OP_KEEP, OP_KEEP );
	dsp_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_KEEP, OP_KEEP );
	dsp_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

//------------------------------------------------------------------------
//	CleanUp
//------------------------------------------------------------------------
void g3dVkUtil::CleanupStates()
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
void g3dVkUtil::BlendBuffers(g2dRenderTarget* i_pRenderTarget, matTexture* i_pSrc, int i_BlendOp, float i_Intensity, 
							   float i_Top, float i_Bottom, float i_Left, float i_Right )
{
	

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

	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");

	int technique = pEffect->FindTechnique("ScaledCopy");

	pEffect->SetConstant(pEffect->FindConstant("g_scaledCopyFactor"), (float)(i_Intensity));

	maVector4d viewport = maVector4d(i_Top,i_Bottom,i_Left,i_Right);
	pEffect->SetFloatVector(pEffect->FindConstant("g_scaledCopyUVs"), viewport.Ptr());

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// set render target and source texture
	i_pRenderTarget->MakeCurrent();
	int w,h;
	i_pRenderTarget->GetDimensions(w,h);
	vkdImage* pResView = g3dVkTextureUtil::GetImage(i_pSrc);

	// turn on stencil - was set up in earlier pass.
	//g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_Ref_Equal, 1 );

	g3dBlendStateMgr::SetBlendState(st_Blend);

	pEffect->Apply(technique, 0);
	set_texture(0, pResView);
	g3dVkUtil::DrawFullScreenQuad( w,h );

	// restore stencil state
	//g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );


	g3dBlendStateMgr::SetBlendState(stp_BlendRestore);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	
	// unbind texture from shader so that it can be set as rendertarget later.
	set_texture(0, NULL);
	set_texture(1, NULL);
	//g2dVkGlobal::g_pDeviceContext->PSSetShader(NULL, NULL, 0);

}

}
