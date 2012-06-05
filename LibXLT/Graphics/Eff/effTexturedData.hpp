/*****************************************************************************
**  effTexturedData.hpp
**
**      effTexturedData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_TEXTUREDDATA_HPP
#error effTexturedData.hpp multiply included
#endif
#define EFF_TEXTUREDDATA_HPP

#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif


//============================================================================
//============================================================================
class matTexture;


//============================================================================
//============================================================================
class effTexturedData : public effShaderData
{
public:
	effTexturedData();
	virtual ~effTexturedData();
	effTexturedData(const effTexturedData& i_CopyFrom);
	effTexturedData& operator = (const effTexturedData& i_CopyFrom);

	virtual bool HasTransparency();

	virtual effShaderData* Clone() const {return new effTexturedData(*this);}

	matTexture* m_Texture;
	maFloatRGBA m_Color;
};
