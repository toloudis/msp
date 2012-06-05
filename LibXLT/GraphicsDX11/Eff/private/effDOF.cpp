/*****************************************************************************
**  effDOF.cpp
**
**      effDOF is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effDOF.hpp"

#include "Graphics/eff/effDOFData.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"

effDOF::effDOF(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name)
:	effShaderBaseDX11(i_Directory, i_pEffect, i_name)
{
	MapParameter("tSource", m_hSharpTexture );
	MapParameter("tSourceLow", m_hBlurryTexture );
	MapParameter("pixelSizeHigh", m_hSharpResolutionData );
	MapParameter("pixelSizeLow", m_hBlurryResolutionData );
	MapParameter("maxCoC", m_hMaxCoC );
}
void effDOF::SetupParams(const effShaderData* i_Data) const
{
	const effDOFData* pData = dynamic_cast<const effDOFData*>(i_Data);
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effDOF");

	m_hSharpTexture->SetResource( g3dDX11TextureUtil::GetD3DTexture(pData->m_pSharpTexture));
	m_hBlurryTexture->SetResource( g3dDX11TextureUtil::GetD3DTexture(pData->m_pBlurryTexture));
	float vec[2] = {pData->m_FilterWidthX, pData->m_FilterWidthY};
	m_hSharpResolutionData->SetFloatVector(vec);
	float vecBlur[2] = {pData->m_FilterWidthX, pData->m_FilterWidthY};
	m_hBlurryResolutionData->SetFloatVector(vecBlur);
	m_hMaxCoC->SetFloat(pData->m_MaxCoC);
}

effShaderData* effDOF::CreateData(const matMaterial* i_Mat)
{
	return new effDOFData();
}




