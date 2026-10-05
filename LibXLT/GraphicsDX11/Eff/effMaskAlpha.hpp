/*****************************************************************************
**  effMaskAlpha.hpp
**
**      effMaskAlpha is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_MASKALPHA_HPP
#error effMaskAlpha.hpp multiply included
#endif
#define EFF_MASKALPHA_HPP

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class matMaterial;

class effMaskAlpha : public effShaderBaseDX11
{
public:
	//====================================================================
	//====================================================================
	effMaskAlpha(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name);

	//====================================================================
	//====================================================================
	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	//====================================================================
	// Returns false because this effect doesn't need single light passes.
	//====================================================================
	virtual bool DoesLighting() const;

	//====================================================================
	//====================================================================
	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

protected:
	fxEffectVariable* m_hSolidColor;
	fxEffectVariable* m_hHasTransparency;
	fxEffectVariable* m_hTransparencyMap;

	fxEffectVariable* m_hUseDither;
	fxEffectVariable* m_hDitherAlphaBias;
	fxEffectVariable* m_hTransparency;
};
