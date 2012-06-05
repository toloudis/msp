/*****************************************************************************
**  effStrandHair.hpp
**
**      effStrandHair is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_STRANDHAIR_HPP
#error effStrandHair.hpp multiply included
#endif
#define EFF_STRANDHAIR_HPP

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

class matMaterial;
class effStrandHairData;

class effStrandHair : public effShaderBaseDX11
{
public:
	effStrandHair(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,  int i_MaterialLayerIndex = 0) const;

	void SetHairData( const effStrandHairData& i_data );

	static void SetVolumeTexture( ID3D11ShaderResourceView* i_pVolume );

protected:

	ID3DX11EffectScalarVariable* m_hHasHairSupport;
	ID3DX11EffectScalarVariable* m_hHasOSM;
	ID3DX11EffectScalarVariable* m_hHasOSMMulti;
	ID3DX11EffectScalarVariable* m_hHasOSMMulti32;
	ID3DX11EffectScalarVariable* m_hHasDepthMap;
	ID3DX11EffectScalarVariable* m_hHasOpacityVolume;

	ID3DX11EffectShaderResourceVariable* m_hOSM[8];//Opacity Shadow Maps
	ID3DX11EffectShaderResourceVariable* m_hDepthMap;//the depth map (R32f actual depths)
	ID3DX11EffectShaderResourceVariable* m_hOpacityVolume;

	//these will be around the object, except if the light planes cross the bounds
	//then these will be reduced by the lights bounds
	ID3DX11EffectScalarVariable* m_hZNear;
	ID3DX11EffectScalarVariable* m_hZFar;
	ID3DX11EffectVectorVariable* m_hInvScreenSize;

	ID3DX11EffectVectorVariable* m_hLightViewPlane;	//plane that intersects the light perpendicularly
//	D3DXHANDLE m_hAspect;			//Projection X,Y scales
	ID3DX11EffectScalarVariable* m_hSubPixelPower;	//0 = none, 1 = linear, 2 = squared, etc..

	///for tessellation
	ID3DX11EffectVectorVariable* m_hTessellation;
	ID3DX11EffectShaderResourceVariable* m_hDataTexture;	//Vertex texture of strand control points

	//for interpolation
	ID3DX11EffectScalarVariable* m_hClumpRadius;
};
