/*****************************************************************************
**  effTextured.cpp
**
**      effTextured is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_EFF_BILLBOARD_HPP
#include "GraphicsVulkan/Eff/effBillboard.hpp"
#endif

#ifndef EFF_BILLBOARDDATA_HPP
#include "Graphics/eff/effBillboardData.hpp"
#endif

#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/mat/matTexture.hpp"
#endif

namespace gvk
{

//====================================================================
//====================================================================
effBillboard::effBillboard(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name)
:	effTextured(i_Directory, std::move(i_pEffect), i_name)
{
	MapParameter("bCKActive", m_hCKActive );
	MapParameter("CKColor", m_hCKColor );
	MapParameter("CKTolerance", m_hCKTolerance );
	MapParameter("bCKRemoveSpill", m_hCKRemoveSpill );
	MapParameter("CKSpillType", m_hCKSpillType);
	MapParameter("CKSpillBias", m_hCKSpillBias);
	MapParameter("CKEdgeBlur", m_hCKEdgeBlur);
	MapParameter("g_brightness", m_hBrightness);

	MapParameter("texWidth", m_hTexWidth);
	MapParameter("texHeight", m_hTexHeight);
}


//====================================================================
//====================================================================
effShaderData* effBillboard::CreateData(const matMaterial* i_Mat)
{
	effBillboardData* pData = new effBillboardData;
	return pData;
}

//====================================================================
//====================================================================
void effBillboard::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	effTextured::SetupMaterial(i_Material, i_MaterialLayerIndex);

	const effBillboardData* pData = dynamic_cast<const effBillboardData*>(i_Material->GetEffectData(i_MaterialLayerIndex));
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effBillboard");

	// set all data fields into shader...
	// could use BeginParameterBlock / ApplyParameterBlock here.

	m_hCKActive->AsScalar()->SetBool( pData->m_bCKActive );
	m_hCKColor->SetFloatVector( pData->m_CKColor.Ptr() );
	m_hCKTolerance->AsScalar()->SetFloat( pData->m_CKTolerance );
	m_hCKRemoveSpill->AsScalar()->SetBool( pData->m_bCKRemoveSpill );
	m_hCKSpillType->AsScalar()->SetInt( pData->m_CKSpillType );
	m_hCKSpillBias->AsScalar()->SetFloat( pData->m_CKSpillBias );
	m_hCKEdgeBlur->AsScalar()->SetInt( pData->m_CKEdgeBlur );

	m_hBrightness->AsScalar()->SetFloat( pData->m_Brightness );

	if (pData->m_Texture)
	{
		m_hTexWidth->AsScalar()->SetInt(pData->m_Texture->GetWidth());
		m_hTexHeight->AsScalar()->SetInt(pData->m_Texture->GetHeight());
	}
}

}
