/*****************************************************************************
**  effBillboard.hpp
**
**      effBillboard is the Vk implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_EFF_BILLBOARD_HPP
#error effBillboard.hpp multiply included
#endif
#define GVK_EFF_BILLBOARD_HPP

#ifndef GVK_EFF_TEXTURED_HPP
#include "GraphicsVulkan/Eff/effTextured.hpp"
#endif

namespace gvk
{

class effBillboard : public effTextured
{
public:
	//====================================================================
	//====================================================================
	effBillboard(const fsLocator& i_Directory, std::unique_ptr<fxEffect> i_pEffect, std::string i_name);

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
}
