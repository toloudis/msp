/*****************************************************************************
**  effOutline.hpp
**
**      effOutline is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_OUTLINE_HPP
#error effOutline.hpp multiply included
#endif
#define EFF_OUTLINE_HPP

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class matMaterial;

class effOutline : public effShaderBaseDX11 
{
public:
	effOutline(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

protected:
	ID3DX11EffectShaderResourceVariable* m_hTexture;
	ID3DX11EffectScalarVariable* m_hDepthScale;
	ID3DX11EffectScalarVariable* m_hMinAngle;
	ID3DX11EffectScalarVariable* m_hMaxAngle;
	ID3DX11EffectScalarVariable* m_hThickness;
	ID3DX11EffectVectorVariable* m_hColor;
	ID3DX11EffectVectorVariable* m_hViewSize;
	ID3DX11EffectScalarVariable* m_hMinWidth;
	ID3DX11EffectScalarVariable* m_hMaxWidth;
};
