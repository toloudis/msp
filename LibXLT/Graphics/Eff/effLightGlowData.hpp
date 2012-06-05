/*****************************************************************************
**  effLightGlowData.hpp
**
**      effLightGlowData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_LIGHTGLOWDATA_HPP
#error effLightGlowData.hpp multiply included
#endif
#define EFF_LIGHTGLOWDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef MA_POINT4D_HPP
#include "Core/ma/maPoint4d.hpp"
#endif

#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif


//============================================================================
//============================================================================
class effLightGlowData : public effShaderData
{
public:
	effLightGlowData();
	virtual ~effLightGlowData();
	effLightGlowData(const effLightGlowData& i_CopyFrom);
	effLightGlowData& operator = (const effLightGlowData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effLightGlowData(*this);}
	// always report as transparent with 50% opacity, 
	// to indicate to the depth peeling code that this is translucent
	virtual bool HasTransparency() {return true;}
	virtual float GetTransparencyValue() const { return 0.5f; }

	float m_EdgeFuzzCutoff;
	float m_DistFalloffStart;
	float m_DistFalloffEnd;
	float m_GlowAlpha;

	// managed by a higher level object (the light)
	matTexture* m_TextureDiffuse;

	// not owned! watch out for object lifetime.
	// assumes light outlives light shaft data.
	g3dProjectedLight* m_pLight;
};
