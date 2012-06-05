/*****************************************************************************
**  effTextured.cpp
**
**      effTextured is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effTextured.hpp"

#include "Graphics/eff/effTexturedData.hpp"

#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "Graphics/mat/matMaterial.hpp"

//====================================================================
//====================================================================
effTextured::effTextured(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name)
:	effShaderBaseDX11(i_Directory, i_pEffect, i_name)
{
	MapParameter("hasDiffuseMap", m_hHasTexture );
	MapParameter("diffuseMap", m_hTexture );
	MapParameter("g_diffuse", m_hColor );
}

//====================================================================
// Returns false because this effect doesn't need single light passes.
//====================================================================
bool effTextured::DoesLighting() const
{
	return false;
}

//====================================================================
//====================================================================
void effTextured::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effTexturedData* pData = dynamic_cast<const effTexturedData*>(i_Material->GetEffectData(i_MaterialLayerIndex));
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effTextured");

	// set all data fields into shader...
	// could use BeginParameterBlock / ApplyParameterBlock here.

	if (m_hHasTexture)
	{
		m_hHasTexture->SetBool(pData->m_Texture?TRUE:FALSE);
	}
	m_hTexture->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_Texture));
	if (m_hColor)
	{
		m_hColor->SetFloatVector( pData->m_Color.Ptr() );
	}
		
}

//====================================================================
//====================================================================
effShaderData* effTextured::CreateData(const matMaterial* i_Mat)
{
	effTexturedData* pData = new effTexturedData;
	return pData;
}


