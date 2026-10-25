/*****************************************************************************
**  effMaskAlpha.cpp
**
**      effMaskAlpha is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_EFF_MASKALPHA_HPP
#include "GraphicsVulkan/Eff/effMaskAlpha.hpp"
#endif

#ifndef EFF_MASKALPHADATA_HPP
#include "Graphics/eff/effMaskAlphaData.hpp"
#endif

#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif

#include <algorithm>

namespace gvk
{

//====================================================================
//====================================================================
effMaskAlpha::effMaskAlpha(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name)
:	effShaderBaseVk(i_Directory, std::move(i_pEffect), i_name)
{
	MapParameter("solidColor", m_hSolidColor );
	MapParameter("hasTransparencyMap", m_hHasTransparency );
	MapParameter("transparencyMap", m_hTransparencyMap );

	MapParameter("g_bUseDither", m_hUseDither );
	MapParameter("g_DitherAlphaBias", m_hDitherAlphaBias );
	MapParameter("g_transparency", m_hTransparency );

	MapParameter("alphaThreshold", m_hAlphaThreshold );
	m_DefaultAlphaThreshold = 0.0f;
	if (m_hAlphaThreshold->IsValid())
		m_hAlphaThreshold->AsScalar()->GetFloat(&m_DefaultAlphaThreshold);
}

//====================================================================
// Returns false because this effect doesn't need single light passes.
//====================================================================
bool effMaskAlpha::DoesLighting() const
{
	return false;
}

//====================================================================
//====================================================================
void effMaskAlpha::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effMaskAlphaData* pData = dynamic_cast<const effMaskAlphaData*>(i_Material->GetEffectData(i_MaterialLayerIndex));
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effMaskAlpha");

	// set all data fields into shader...
	// could use BeginParameterBlock / ApplyParameterBlock here.

	m_hSolidColor->SetFloatVector( pData->m_Color.Ptr() );

	//Note that this overrides the global variable!
	SetAlphaTestRef( pData->m_AlphaThreshold );

	m_hHasTransparency->SetBool((pData->m_TextureTransparencyMap != NULL) ? true : false);
	vkdImage* transparency_map = g3dVkTextureUtil::GetImage(pData->m_TextureTransparencyMap);
	if (transparency_map && pData->m_bTransparencyMapAlpha)
		transparency_map = transparency_map->AlphaView();
	m_hTransparencyMap->SetResource(transparency_map);

	// a MASK material's cutoff (m_AlphaThreshold) reaches the RSM's own
	// threshold too, which stays at least its default
	if (m_hAlphaThreshold->IsValid())
		m_hAlphaThreshold->AsScalar()->SetFloat(std::max(m_DefaultAlphaThreshold, pData->m_AlphaThreshold));


	m_hUseDither->SetBool((pData->m_bDitherTranslucent)?true:false);
	m_hDitherAlphaBias->SetFloat(pData->m_DitherAlphaBias);
	m_hTransparency->SetFloat(pData->m_Transparency );

}

//====================================================================
//====================================================================
effShaderData* effMaskAlpha::CreateData(const matMaterial* i_Mat)
{
	effMaskAlphaData* pData = new effMaskAlphaData;
	return pData;
}

}
