/*****************************************************************************
**  effLightGlow.cpp
**
**      effLightGlow is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_EFF_LIGHTGLOW_HPP
#include "GraphicsVulkan/Eff/effLightGlow.hpp"
#endif

#ifndef EFF_LIGHTGLOWDATA_HPP
#include "Graphics/eff/effLightGlowData.hpp"
#endif

#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/g3d/g3dProjectedLight.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif

namespace gvk
{

effLightGlow::effLightGlow(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name)
:	effShaderBaseVk(i_Directory, std::move(i_pEffect), i_name)
{
	MapParameter("hasDiffuseTexture", m_hHasDiffuseMap );
	MapParameter("diffuseTexture", m_hDiffuseMap );
	MapParameter("edgeFuzzCutoff", m_hEdgeFuzzCutoff );
	MapParameter("distFalloffStart", m_hDistFalloffStart );
	MapParameter("distFalloffEnd", m_hDistFalloffEnd );
	MapParameter("glowAlpha", m_hGlowAlpha );
	MapParameter("useShadow", m_hUseShadow );
}

void effLightGlow::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effLightGlowData* pData = dynamic_cast<const effLightGlowData*>(i_Material->GetEffectData(i_MaterialLayerIndex));
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effLightGlow");

	// set all data fields into shader...
	// could use BeginParameterBlock / ApplyParameterBlock here.

	m_hHasDiffuseMap->SetBool((pData->m_TextureDiffuse != NULL) ? true : false);
	m_hDiffuseMap->SetResource(g3dVkTextureUtil::GetImage(pData->m_TextureDiffuse));

	m_hEdgeFuzzCutoff->SetFloat(pData->m_EdgeFuzzCutoff);
	m_hDistFalloffStart->SetFloat(pData->m_DistFalloffStart);
	m_hDistFalloffEnd->SetFloat(pData->m_DistFalloffEnd);
	m_hGlowAlpha->SetFloat(pData->m_GlowAlpha);

	m_hUseShadow->SetBool(g3dSingleLightRendering::GetDoSingleLightRendering()?true:false);

	matShaderEffect::Technique tec = g3dSceneRenderUtil::SelectShadowTechnique(pData->m_pLight);
	SetTechnique(tec);
	SetupSingleLight(pData->m_pLight, pData->m_pLight, true);
}

effShaderData* effLightGlow::CreateData(const matMaterial* i_Mat)
{
	return new effLightGlowData();
}

//====================================================================
// Returns false because this effect doesn't need single light passes.
//====================================================================
bool effLightGlow::DoesLighting() const
{
	return false;
}

}
