/*****************************************************************************
**  effTextured.hpp
**
**      effTextured is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_TEXTURED_HPP
#error effTextured.hpp multiply included
#endif
#define EFF_TEXTURED_HPP

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class matMaterial;

class effTextured : public effShaderBaseDX11
{
public:
	//====================================================================
	//====================================================================
	effTextured(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name);

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
	fxEffectVariable* m_hHasTexture;
	fxEffectVariable* m_hTexture;
	fxEffectVariable* m_hColor;
};
