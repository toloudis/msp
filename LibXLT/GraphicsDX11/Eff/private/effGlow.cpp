/*****************************************************************************
**  effGlow.cpp
**
**      effGlow is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effGlow.hpp"

#include "Graphics/eff/effGlowData.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"

effGlow::effGlow(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name)
:	effShaderBaseDX11(i_Directory, i_pEffect, i_name)
{
	MapParameter("glowTexture", m_hTexture );
	MapParameter("glowAmount", m_hGlowAmount );
	MapParameter("glowScale", m_hGlowScale );
	MapParameter("glowSize", m_hGlowSize );
	MapParameter("bConstantGlow", m_hConstantGlow );
	MapParameter("srcSizeInfo", m_hSrcSizeInfo );
}

void effGlow::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effGlowData* pData = &(i_Material->GetGlowData());
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effGlow");

	m_hTexture->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_pTexture));
	m_hGlowAmount->SetFloat(pData->m_GlowAmount);
	m_hGlowScale->SetFloatVector( pData->m_GlowScale.Ptr() );
	m_hGlowSize->SetFloat(pData->m_GlowSize);
	m_hConstantGlow->SetBool(pData->m_bConstantGlow);
	m_hSrcSizeInfo->SetFloatVector( pData->m_SrcSizeInfo.Ptr() );
}

effShaderData* effGlow::CreateData(const matMaterial* i_Mat)
{
	return new effGlowData();
}




