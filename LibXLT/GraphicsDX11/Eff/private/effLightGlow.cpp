/*****************************************************************************
**  effLightGlow.cpp
**
**      effLightGlow is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effLightGlow.hpp"

#include "Graphics/eff/effLightGlowData.hpp"

#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"

effLightGlow::effLightGlow(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name)
:	effShaderBaseDX11(i_Directory, std::move(i_pEffect), i_name)
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

	m_hHasDiffuseMap->SetBool((pData->m_TextureDiffuse != NULL) ? TRUE : FALSE);
	m_hDiffuseMap->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_TextureDiffuse));

	m_hEdgeFuzzCutoff->SetFloat(pData->m_EdgeFuzzCutoff);
	m_hDistFalloffStart->SetFloat(pData->m_DistFalloffStart);
	m_hDistFalloffEnd->SetFloat(pData->m_DistFalloffEnd);
	m_hGlowAlpha->SetFloat(pData->m_GlowAlpha);

	m_hUseShadow->SetBool(g3dSingleLightRendering::GetDoSingleLightRendering()?TRUE:FALSE);

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

