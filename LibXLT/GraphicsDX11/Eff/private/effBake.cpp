/*****************************************************************************
**  effBake.cpp
**
**      effBake is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effBake.hpp"

#include "Graphics/eff/effBakeData.hpp"

#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"

effBake::effBake(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name)
:	effShaderBaseDX11(i_Directory, i_pEffect, i_name)
{
	MapParameter("hasEmissiveMap", m_hHasEmissive);
	MapParameter("emissiveMap", m_hEmissiveMap);
	MapParameter("hasNormalMap", m_hHasNormal );
	MapParameter("normalMap", m_hNormalMap );
	MapParameter("g_emissive", m_MaterialEmissiveHandle);
}

void effBake::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effBakeData* pData = dynamic_cast<const effBakeData*>(i_Material->GetEffectData(i_MaterialLayerIndex));
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effBake");

	// set all data fields into shader...
	// could use BeginParameterBlock / ApplyParameterBlock here.

	if (g3dSingleLightRendering::GetDoBaking())
	{
		float comb_emissive[4] = {0,0,0,0};
		m_MaterialEmissiveHandle->SetFloatVector(comb_emissive);
		m_hHasEmissive->AsScalar()->SetBool( (pData->m_TextureEmissive != NULL) ? TRUE : FALSE);
		m_hEmissiveMap->AsShaderResource()->SetResource( g3dDX11TextureUtil::GetD3DTexture(pData->m_TextureEmissive));
	}
	else
	{
		m_MaterialEmissiveHandle->SetFloatVector( pData->m_ColorEmissive.Ptr() );
		m_hHasEmissive->AsScalar()->SetBool( (pData->m_TextureEmissive != NULL) ? TRUE : FALSE);
		m_hEmissiveMap->AsShaderResource()->SetResource( g3dDX11TextureUtil::GetD3DTexture(pData->m_TextureEmissive));
	}

	m_hHasNormal->AsScalar()->SetBool((pData->m_TextureNormalMap != NULL) ? TRUE : FALSE);
	m_hNormalMap->AsShaderResource()->SetResource( g3dDX11TextureUtil::GetD3DTexture(pData->m_TextureNormalMap));
	
	effShaderBaseDX11::SetupMaterial( i_Material, i_MaterialLayerIndex );
}

effShaderData* effBake::CreateData(const matMaterial* i_Mat)
{
	effBakeData* pData = new effBakeData;
	return pData;
}


