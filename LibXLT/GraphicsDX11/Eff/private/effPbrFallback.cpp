/*****************************************************************************
**	effPbrFallback.cpp
**
**		see .hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "GraphicsDX11/eff/effPbrFallback.hpp"

#include "Graphics/Eff/effBlinnData.hpp"
#include "Graphics/Eff/effShaderParams.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Core/dbg/dbgMsg.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effPbrFallback::effPbrFallback(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pBlinnEffect, std::string i_Name)
:	effShaderBaseDX11(i_Directory, std::move(i_pBlinnEffect), i_Name),
	m_bLoggedNotEditable(false)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effShaderParams* effPbrFallback::blinn_params(effShaderParams* i_pPbr) const
{
	const effPbrData data = effPbrData::FromParams(*i_pPbr);
	Entry& entry = m_Blinn[i_pPbr];
	if (!entry.m_pBlinn || entry.m_Data != data)
	{
		// A new bag, or the Pbr values changed (an animated channel, or
		// another material at the same address): convert again
		entry.m_Data = data;
		entry.m_pBlinn = effPbrData::MakeBlinnFallbackParams(*i_pPbr);
	}
	else
	{
		// The textures the Pbr bag has loaded since (Blinn's are found by
		// name only when its own bag is loaded, which this one never is)
		effPbrData::CopyTexturesToBlinnFallback(*i_pPbr, *entry.m_pBlinn);
	}
	return entry.m_pBlinn.get();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effPbrFallback::SetupMaterial(const matMaterial* i_Material, int i_MaterialLayerIndex) const
{
	shared_ptr<effShaderParams> p = i_Material->GetShaderParams(i_MaterialLayerIndex);
	SetupMaterialParams(i_Material, p ? blinn_params(p.get()) : NULL);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int effPbrFallback::BuildPrtyObject(effShaderParams* o_pParams) const
{
	for (const auto& entry : m_Blinn)
	{
		if (entry.second.m_pBlinn.get() == o_pParams)
			return effShaderBaseDX11::BuildPrtyObject(o_pParams);
	}

	// A Pbr bag: what the material asks of it (its colour, its
	// transparency) still works
	o_pParams->m_pDiffuseColor = o_pParams->FindColorParam(effPbrData::c_BaseColor);
	o_pParams->m_pDiffuseMap = o_pParams->FindTextureParam(effPbrData::c_BaseColorMap);
	o_pParams->m_pAlphaMode = o_pParams->FindIntParam(effPbrData::c_AlphaMode);

	if (!m_bLoggedNotEditable)
	{
		m_bLoggedNotEditable = true;
		DBG_WARNING("PBR materials are drawn as Blinn on DX11 and can't be edited there");
	}
	return 0;
}
