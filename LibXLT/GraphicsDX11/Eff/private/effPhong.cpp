/*****************************************************************************
**  effPhong.cpp
**
**      effPhong is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effPhong.hpp"

#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/eff/effPhongData.hpp"

#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"

effPhong::effPhong(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name)
:	effShaderBaseDX11(i_Directory, i_pEffect, i_name)
{
	MapParameter("hasDiffuseMap", m_hHasDiffuse);
	MapParameter("diffuseMap", m_hDiffuseMap);
	MapParameter("hasSpecularMap", m_hHasSpecular); 
	MapParameter("specularMap", m_hSpecularMap);
	MapParameter("hasCubeMap", m_hHasCube);
	MapParameter("hasCubeMap", m_hHasCube );
	MapParameter("cubeMap", m_hCubeMap );
	MapParameter("hasNormalMap", m_hHasNormal );
	MapParameter("normalMap", m_hNormalMap );
	MapParameter("hasGlossMap", m_hHasGloss );
	MapParameter("glossMap", m_hGlossMap );
	MapParameter("hasReflectFactorMap", m_hHasReflectFactor );
	MapParameter("reflectFactorMap", m_hReflectFactorMap );
	MapParameter("hasTransparencyMap", m_hHasTransparencyMap );
	MapParameter("transparencyMap", m_hTransparencyMap );
	MapParameter("g_emissive", m_MaterialEmissiveHandle);
	MapParameter("g_ambient", m_MaterialAmbientHandle);
	MapParameter("g_diffuse", m_MaterialDiffuseHandle);
	MapParameter("g_specular", m_MaterialSpecularHandle);
	MapParameter("g_shininess", m_MaterialSpecularPowerHandle);
	MapParameter("g_bumpMapScale", m_MaterialBumpMapScaleHandle);
	MapParameter("g_reflectivity", m_MaterialReflectivityHandle);
	MapParameter("g_transparency", m_MaterialTransparencyHandle);
}

void effPhong::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effPhongData* pData = dynamic_cast<const effPhongData*>(i_Material->GetEffectData(i_MaterialLayerIndex));
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effPhong");

	// set all data fields into shader...
	// could use BeginParameterBlock / ApplyParameterBlock here.

	if (g3dSingleLightRendering::GetDoBaking())
	{
		m_hHasDiffuse->AsScalar()->SetBool(FALSE);
		m_hDiffuseMap->AsShaderResource()->SetResource(NULL);
		m_hHasSpecular->AsScalar()->SetBool(FALSE);
		m_hSpecularMap->AsShaderResource()->SetResource(NULL);
		float comb_emissive[4] = {0,0,0,pData->GetDiffuse().GetAlpha()};
		m_MaterialEmissiveHandle->SetFloatVector(comb_emissive);
		float comb_ambient[4] = {0,0,0,pData->GetDiffuse().GetAlpha() };
		m_MaterialAmbientHandle->SetFloatVector(comb_ambient);
		float comb_diffuse[4] = {1,1,1,pData->GetDiffuse().GetAlpha()};
		m_MaterialDiffuseHandle->SetFloatVector( comb_diffuse );
		float comb_specular[4] = {0,0,0,0};
		m_MaterialSpecularHandle->SetFloatVector( comb_specular );
	}
	else
	{
		m_hHasDiffuse->AsScalar()->SetBool( (pData->m_TextureDiffuse != NULL) ? TRUE : FALSE);
		m_hDiffuseMap->AsShaderResource()->SetResource( g3dDX11TextureUtil::GetD3DTexture(pData->m_TextureDiffuse));
		m_hHasSpecular->AsScalar()->SetBool( (pData->m_TextureSpecular != NULL) ? TRUE : FALSE);
		m_hSpecularMap->AsShaderResource()->SetResource( g3dDX11TextureUtil::GetD3DTexture(pData->m_TextureSpecular));
		m_MaterialEmissiveHandle->SetFloatVector( pData->m_ColorEmissive.Ptr() );
		m_MaterialAmbientHandle->SetFloatVector( pData->m_ColorAmbient.Ptr() );
		m_MaterialDiffuseHandle->SetFloatVector( pData->m_ColorDiffuse.Ptr() );
		m_MaterialSpecularHandle->SetFloatVector( pData->m_ColorSpecular.Ptr() );
	}

	m_hHasNormal->AsScalar()->SetBool((pData->m_TextureNormalMap != NULL) ? TRUE : FALSE);
	m_hNormalMap->AsShaderResource()->SetResource( g3dDX11TextureUtil::GetD3DTexture(pData->m_TextureNormalMap));
	m_hHasGloss->AsScalar()->SetBool((pData->m_TextureGloss != NULL) ? TRUE : FALSE);
	m_hGlossMap->AsShaderResource()->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_TextureGloss));
	m_hHasCube->AsScalar()->SetBool((pData->m_TextureEnvironment != NULL) ? TRUE : FALSE);
	m_hCubeMap->AsShaderResource()->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_TextureEnvironment));
	m_hHasReflectFactor->AsScalar()->SetBool((pData->m_TextureReflectFactorMap != NULL) ? TRUE : FALSE);
	m_hReflectFactorMap->AsShaderResource()->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_TextureReflectFactorMap));
	m_hHasTransparencyMap->AsScalar()->SetBool((pData->m_TextureTransparencyMap != NULL) ? TRUE : FALSE);
	m_hTransparencyMap->AsShaderResource()->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_TextureTransparencyMap));
	
	m_MaterialSpecularPowerHandle->AsScalar()->SetFloat(pData->m_SpecularPower );
	m_MaterialBumpMapScaleHandle->AsScalar()->SetFloat(pData->m_BumpMapScale );
	m_MaterialReflectivityHandle->AsScalar()->SetFloat(pData->m_Reflectivity );
	m_MaterialTransparencyHandle->AsScalar()->SetFloat(pData->m_Transparency );

	maMatrix4x4 muv = pData->m_UV.MakeUVTransform();
	m_UVTransformHandle->AsMatrix()->SetMatrix( muv.Ptr());

	effShaderBaseDX11::SetupMaterial( i_Material, i_MaterialLayerIndex );
}

effShaderData* effPhong::CreateData(const matMaterial* i_Mat)
{
	effPhongData* pData = new effPhongData;
	return pData;
}


