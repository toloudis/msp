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

effGlow::effGlow(std::unique_ptr<fxEffectDX11> i_pEffect, std::string i_name)
:	effPlainShaderDX11(std::move(i_pEffect), i_name)
{
	m_hTexture = m_pEffect->FindResource("glowTexture");
	m_hGlowAmount = m_pEffect->FindConstant("glowAmount");
	m_hGlowScale = m_pEffect->FindConstant("glowScale");
	m_hGlowSize = m_pEffect->FindConstant("glowSize");
	m_hConstantGlow = m_pEffect->FindConstant("bConstantGlow");
	m_hSrcSizeInfo = m_pEffect->FindConstant("srcSizeInfo");
}

void effGlow::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effGlowData* pData = &(i_Material->GetGlowData());
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effGlow");

	m_pEffect->SetResource(m_hTexture, g3dDX11TextureUtil::GetD3DTexture(pData->m_pTexture));
	m_pEffect->SetConstant(m_hGlowAmount, (float)pData->m_GlowAmount);
	m_pEffect->SetFloatVector(m_hGlowScale, pData->m_GlowScale.Ptr());
	m_pEffect->SetConstant(m_hGlowSize, (float)pData->m_GlowSize);
	m_pEffect->SetConstant(m_hConstantGlow, (int)(pData->m_bConstantGlow ? 1 : 0));	// HLSL bool is 4 bytes
	m_pEffect->SetFloatVector(m_hSrcSizeInfo, pData->m_SrcSizeInfo.Ptr());
}

effShaderData* effGlow::CreateData(const matMaterial* i_Mat)
{
	return new effGlowData();
}




