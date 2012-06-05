/*****************************************************************************
**  effTextured.cpp
**
**      effTextured is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effRamp.hpp"

#include "Graphics/eff/effRampData.hpp"

#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

//====================================================================
//====================================================================
effRamp::effRamp(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name)
:	effShaderBaseDX11(i_Directory, i_pEffect, i_name)
{
	MapParameter("gradientMap", m_hGradientTexture);
	MapParameter("hasGradient", m_hHasGradient);
	MapParameter("noiseMap", m_hNoiseTexture);
	MapParameter("hasNoise", m_hHasNoise);

	MapParameter("gradientShape", m_hShape);
	MapParameter("gradientInterpolation", m_hInterpolation);
	MapParameter("uWave", m_hUWave);
	MapParameter("uWaveFreq", m_hUWaveFreq);
	MapParameter("vWave", m_hVWave);
	MapParameter("vWaveFreq", m_hVWaveFreq);
	MapParameter("noiseOffset", m_hNoise);
	MapParameter("noiseFreq", m_hNoiseFreq);
}

//====================================================================
//====================================================================
effRamp::~effRamp()
{
}


//====================================================================
// Returns false because this effect doesn't need single light passes.
//====================================================================
bool effRamp::DoesLighting() const
{
	return false;
}

//====================================================================
//====================================================================
void effRamp::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effRampData* pData = dynamic_cast<const effRampData*>(i_Material->GetEffectData(i_MaterialLayerIndex));
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effRamp");

	m_hHasGradient->SetBool((pData->m_Texture != NULL) ? TRUE : FALSE);
	m_hGradientTexture->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_Texture));

	m_hHasNoise->SetBool((pData->m_NoiseTexture != NULL) ? TRUE : FALSE);
	m_hNoiseTexture->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_NoiseTexture));

	m_hShape->SetInt(pData->m_RampShape);
	m_hInterpolation->SetInt(pData->m_RampInterpolation);
	m_hUWave->SetFloat(pData->m_UWave);
	m_hUWaveFreq->SetFloat(pData->m_UWaveFreq);
	m_hVWave->SetFloat(pData->m_VWave);
	m_hVWaveFreq->SetFloat(pData->m_VWaveFreq);
	m_hNoise->SetFloat(pData->m_Noise);
	m_hNoiseFreq->SetFloat(pData->m_NoiseFreq);
}

//====================================================================
//====================================================================
effShaderData* effRamp::CreateData(const matMaterial* i_Mat)
{
	effRampData* pData = new effRampData;
	return pData;
}


