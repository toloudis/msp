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
	effStrandHair(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name);

	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	virtual void SetupMaterial(const matMaterial* i_Material,  int i_MaterialLayerIndex = 0) const;

	void SetHairData( const effStrandHairData& i_data );

	static void SetVolumeTexture( ID3D11ShaderResourceView* i_pVolume );

protected:

	fxEffectVariable* m_hHasHairSupport;
	fxEffectVariable* m_hHasOSM;
	fxEffectVariable* m_hHasOSMMulti;
	fxEffectVariable* m_hHasOSMMulti32;
	fxEffectVariable* m_hHasDepthMap;
	fxEffectVariable* m_hHasOpacityVolume;

	fxEffectVariable* m_hOSM[8];//Opacity Shadow Maps
	fxEffectVariable* m_hDepthMap;//the depth map (R32f actual depths)
	fxEffectVariable* m_hOpacityVolume;

	//these will be around the object, except if the light planes cross the bounds
	//then these will be reduced by the lights bounds
	fxEffectVariable* m_hZNear;
	fxEffectVariable* m_hZFar;
	fxEffectVariable* m_hInvScreenSize;

	fxEffectVariable* m_hLightViewPlane;	//plane that intersects the light perpendicularly
//	D3DXHANDLE m_hAspect;			//Projection X,Y scales
	fxEffectVariable* m_hSubPixelPower;	//0 = none, 1 = linear, 2 = squared, etc..

	///for tessellation
	fxEffectVariable* m_hTessellation;
	fxEffectVariable* m_hDataTexture;	//Vertex texture of strand control points

	//for interpolation
	fxEffectVariable* m_hClumpRadius;
};
