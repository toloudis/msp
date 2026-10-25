/*****************************************************************************
**  effOpacity.cpp
**
**		see .hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "Graphics/Eff/effOpacity.hpp"

#ifndef EFF_PBRDATA_HPP
#include "Graphics/Eff/effPbrData.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif

#include <algorithm>
#include <cmath>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float effOpacity::AlphaTestRef(float i_PassRef) const
{
	if (!m_bCutOut)
		return i_PassRef;
	return std::max(i_PassRef, std::nextafter(m_Cutoff, 0.0f));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effOpacity effOpacity::Of(const matMaterial& i_Material)
{
	effOpacity opacity;
	shared_ptr<effShaderParams> params = i_Material.GetShaderParams();

	if (params && params->m_pAlphaMode)
	{
		const int mode = params->m_pAlphaMode->GetProperty().GetValue();
		if (mode != effPbrData::e_Mask && mode != effPbrData::e_Blend)
			return opacity;
		if (params->m_pDiffuseMap)
			opacity.m_pMap = params->m_pDiffuseMap->GetTexture();
		opacity.m_bMapAlpha = true;
		if (params->m_pDiffuseColor)
			opacity.m_Value = params->m_pDiffuseColor->GetProperty().GetValue().GetAlpha();
		if (mode == effPbrData::e_Mask)
		{
			opacity.m_bCutOut = true;
			if (params->m_pAlphaCutoff)
				opacity.m_Cutoff = params->m_pAlphaCutoff->GetProperty().GetValue();
		}
		return opacity;
	}

	if (!i_Material.GetHasTransparency())
		return opacity;

	if (params)
	{
		if (params->m_pTransparencyMap)
			opacity.m_pMap = params->m_pTransparencyMap->GetTexture();
		if (params->m_pTransparency)
			opacity.m_Value = params->m_pTransparency->GetProperty().GetValue();
	}
	else if (effShaderData* data = i_Material.GetEffectData())
	{
		opacity.m_pMap = data->GetTransparencyTexture();
		opacity.m_Value = data->GetTransparencyValue();
	}
	return opacity;
}
