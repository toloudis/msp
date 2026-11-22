/*****************************************************************************
**  effSolid.cpp
**
**      effSolid is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_EFF_SOLID_HPP
#include "GraphicsVulkan/Eff/effSolid.hpp"
#endif

#ifndef EFF_SOLIDDATA_HPP
#include "Graphics/eff/effSolidData.hpp"
#endif

#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif

namespace gvk
{

//====================================================================
//====================================================================
effSolid::effSolid(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name)
:	effShaderBaseVk(i_Directory, std::move(i_pEffect), i_name)
{
	MapParameter("solidColor", m_hSolidColor );
}

//====================================================================
// Returns false because this effect doesn't need single light passes.
//====================================================================
bool effSolid::DoesLighting() const
{
	return false;
}

//====================================================================
//====================================================================
void effSolid::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	const effSolidData* pData = dynamic_cast<const effSolidData*>(i_Material->GetEffectData(i_MaterialLayerIndex));
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effSolid");

	// set all data fields into shader...
	// could use BeginParameterBlock / ApplyParameterBlock here.

	m_hSolidColor->SetFloatVector( pData->m_Color.Ptr() );
}

//====================================================================
//====================================================================
effShaderData* effSolid::CreateData(const matMaterial* i_Mat)
{
	effSolidData* pData = new effSolidData;
	return pData;
}

}
