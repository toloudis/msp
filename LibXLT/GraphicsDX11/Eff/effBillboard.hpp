/*****************************************************************************
**  effBillboard.hpp
**
**      effBillboard is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_BILLBOARD_HPP
#error effBillboard.hpp multiply included
#endif
#define EFF_BILLBOARD_HPP

#ifndef EFF_TEXTURED_HPP
#include "GraphicsDX11/eff/effTextured.hpp"
#endif

class effBillboard : public effTextured
{
public:
	//====================================================================
	//====================================================================
	effBillboard(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name);

	//====================================================================
	//====================================================================
	virtual effShaderData* CreateData(const matMaterial* i_Mat);

	//====================================================================
	//====================================================================
	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;

protected:
	fxEffectVariable* m_hCKActive;
	fxEffectVariable* m_hCKColor;
	fxEffectVariable* m_hCKTolerance;
	fxEffectVariable* m_hCKRemoveSpill;
	fxEffectVariable* m_hCKSpillType;
	fxEffectVariable* m_hCKSpillBias;
	fxEffectVariable* m_hCKEdgeBlur;

	fxEffectVariable* m_hTexWidth;
	fxEffectVariable* m_hTexHeight;

	fxEffectVariable* m_hBrightness;
};
