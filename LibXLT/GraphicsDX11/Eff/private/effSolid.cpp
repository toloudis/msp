/*****************************************************************************
**  effSolid.cpp
**
**      effSolid is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effSolid.hpp"

#include "Graphics/eff/effSolidData.hpp"

#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "Graphics/mat/matMaterial.hpp"

//====================================================================
//====================================================================
effSolid::effSolid(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name)
:	effShaderBaseDX11(i_Directory, std::move(i_pEffect), i_name)
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


