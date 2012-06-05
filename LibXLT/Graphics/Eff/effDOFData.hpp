/*****************************************************************************
**  effDOFData.hpp
**
**      effDOFData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_DOFDATA_HPP
#error effDOFData.hpp multiply included
#endif
#define EFF_DOFDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif


//============================================================================
//============================================================================
class effDOFData : public effShaderData
{
public:
	effDOFData();
	virtual ~effDOFData();
	effDOFData(const effDOFData& i_CopyFrom);
	effDOFData& operator = (const effDOFData& i_CopyFrom);
	bool operator == (const effDOFData& i_EffDOFData) const;

	virtual effShaderData* Clone() const {return new effDOFData(*this);}

	matTexture* m_pSharpTexture;
	matTexture* m_pBlurryTexture;

	// spacing between samples
	// texture coordinate space is in fraction of image  
	// (1/imageres for single pixel sampling)
	float m_FilterWidthX;
	float m_FilterWidthY;

	float m_MaxCoC;
};
