/*****************************************************************************
**  effPbrFallback.hpp
**
**      effPbrFallback is DX11's answer to Pbr.fx: there is no PBR shader on
**      DX11, so a Pbr material is drawn by the Blinn effect. Each
**      material's Pbr parameter bag is converted (effPbrData::
**      MakeBlinnFallback) into a Blinn bag when the material is set up; the
**      Blinn bag is derived, never saved. Pbr materials can't be edited on
**      DX11, and DX11's material picker doesn't list Pbr.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef EFF_PBRFALLBACK_HPP
#error effPbrFallback.hpp multiply included
#endif
#define EFF_PBRFALLBACK_HPP

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif
#ifndef EFF_PBRDATA_HPP
#include "Graphics/Eff/effPbrData.hpp"
#endif

#include <map>
#include <memory>


class effPbrFallback : public effShaderBaseDX11
{
public:
	// i_pBlinnEffect: the embedded Materials/Blinn effect; i_Name "Pbr.fx"
	effPbrFallback(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pBlinnEffect, std::string i_Name);

	// The material's Pbr bag converted to a Blinn bag (cached per bag,
	// rebuilt when the Pbr values change), its textures the Pbr bag's
	// loaded ones, then set up as Blinn
	virtual void SetupMaterial(const matMaterial* i_Material, int i_MaterialLayerIndex = 0) const;

	// A Pbr bag gets no bindings or UI (it isn't editable here; logged
	// once), only its special parameters (diffuse colour and map, alpha
	// mode) so the material's colour and transparency are known; any other
	// bag (the Blinn defaults and Blinn bags) is built as Blinn's
	virtual int BuildPrtyObject(effShaderParams* o_pParams) const;

private:
	struct Entry
	{
		effPbrData m_Data;
		std::shared_ptr<effShaderParams> m_pBlinn;
	};

	// The Blinn bag for i_pPbr, made or remade as needed
	effShaderParams* blinn_params(effShaderParams* i_pPbr) const;

	mutable std::map<const effShaderParams*, Entry> m_Blinn;
	mutable bool m_bLoggedNotEditable;
};
