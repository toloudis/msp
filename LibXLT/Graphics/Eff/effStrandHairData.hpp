/*****************************************************************************
**  effStrandHairData.hpp
**
**      effStrandHairData is the data for a stranded hair shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_STRANDHAIRDATA_HPP
#error effStrandHairData.hpp multiply included
#endif
#define EFF_STRANDHAIRDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef MA_VECTOR2D_HPP
#include "Core/ma/maVector2d.hpp"
#endif

#ifndef MA_VECTOR4D_HPP
#include "Core/ma/maVector4d.hpp"
#endif

#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#include <string>

class effStrandHairData : public effShaderData
{
public:
	effStrandHairData();
	virtual ~effStrandHairData();
	effStrandHairData(const effStrandHairData& i_CopyFrom);
	effStrandHairData& operator = (const effStrandHairData& i_CopyFrom);
	bool operator == (const effStrandHairData& i_EffStrandHairData) const;

	void Default();	//set parameters to default state
	virtual effShaderData* Clone() const {return new effStrandHairData(*this);}

	//all hair techniques
	maVector2d m_InvScreenSize;
	float m_SubPixelPower;

	float m_ZNear;					
	float m_ZFar;

	maVector4d m_LightViewPlane;

	matTexture* m_pOSM[8];
	matTexture* m_pDepthTexture;
	matTexture* m_pDataTexture;		//stores strand control points for hardware tessellation
};
