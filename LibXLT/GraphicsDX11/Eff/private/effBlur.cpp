/*****************************************************************************
**  effBlur.cpp
**
**      effBlur is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effBlur.hpp"

#include "Graphics/eff/effBlurData.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexture.hpp"

effBlur::effBlur(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name)
:	effShaderBaseDX11(i_Directory, i_pEffect, i_name)
{
	MapParameter("srcSizeInfo", m_hSrcSize );
	MapParameter("downsampledSizeInfo", m_hDownsampledSize );
	MapParameter("sceneTexture", m_hSceneTexture );
	MapParameter("downsampledTexture", m_hDownsampledTexture );
	MapParameter("horizontalBlurTexture", m_hHorizontalBlurTexture );
}
void effBlur::SetupParams(const effShaderData* i_Data) const
{
	const effBlurData* pData = dynamic_cast<const effBlurData*>(i_Data);
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effBlur");

	m_hSceneTexture->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_pSceneTexture));
	m_hDownsampledTexture->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_pDownsampledTexture));
	m_hHorizontalBlurTexture->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_pHorizontalBlurTexture));

	float vec[4] = {(float)pData->m_pSceneTexture->GetWidth(), (float)pData->m_pSceneTexture->GetHeight(),
			pData->m_FilterWidthX,
			pData->m_FilterWidthY};
	m_hSrcSize->SetFloatVector(vec);
	float vecLow[4] = {(float)pData->m_pDownsampledTexture->GetWidth(), (float)pData->m_pDownsampledTexture->GetHeight(),
			pData->m_FilterWidthX,
			pData->m_FilterWidthY};
	m_hDownsampledSize->SetFloatVector(vecLow);
}

effShaderData* effBlur::CreateData(const matMaterial* i_Mat)
{
	return new effBlurData();
}




