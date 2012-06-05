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
	effTextured(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name);

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
	ID3DX11EffectScalarVariable* m_hHasTexture;
	ID3DX11EffectShaderResourceVariable* m_hTexture;
	ID3DX11EffectVectorVariable* m_hColor;
};
