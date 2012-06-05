/*****************************************************************************
**  effBlurData.hpp
**
**      effBlurData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_BLURDATA_HPP
#error effBlurData.hpp multiply included
#endif
#define EFF_BLURDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif


//============================================================================
//============================================================================
class effBlurData : public effShaderData
{
public:
	effBlurData();
	virtual ~effBlurData();
	effBlurData(const effBlurData& i_CopyFrom);
	effBlurData& operator = (const effBlurData& i_CopyFrom);
	bool operator == (const effBlurData& i_EffBlurData) const;

	virtual effShaderData* Clone() const {return new effBlurData(*this);}

	matTexture* m_pSceneTexture;
	matTexture* m_pDownsampledTexture;
	matTexture* m_pHorizontalBlurTexture;

	// spacing between samples
	// texture coordinate space is in fraction of image  
	// (1/imageres for single pixel sampling)
	float m_FilterWidthX;
	float m_FilterWidthY;
};
