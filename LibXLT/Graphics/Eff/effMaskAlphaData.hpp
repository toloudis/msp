/*****************************************************************************
**  effMaskAlphaData.hpp
**
**      effMaskAlphaData is the data for a shader effect.
**
**	This shader is used in pick map and depth map rendering in order to
**	mask out the pixels that are below an alpha threshold.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_MASKALPHADATA_HPP
#error effMaskAlphaData.hpp multiply included
#endif
#define EFF_MASKALPHADATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif


//============================================================================
//============================================================================
class effMaskAlphaData : public effShaderData
{
public:
	effMaskAlphaData();
	virtual ~effMaskAlphaData();
	effMaskAlphaData(const effMaskAlphaData& i_CopyFrom);
	effMaskAlphaData& operator = (const effMaskAlphaData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effMaskAlphaData(*this);}

	maFloatRGBA m_Color;
	float m_AlphaThreshold;
	bool m_bDitherTranslucent;
	float m_DitherAlphaBias;
	float m_Transparency;

	matTexture* m_TextureTransparencyMap;
};
