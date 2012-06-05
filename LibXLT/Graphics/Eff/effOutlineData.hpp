/*****************************************************************************
**  effOutlineData.hpp
**
**      effOutlineData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_OUTLINEDATA_HPP
#error effOutlineData.hpp multiply included
#endif
#define EFF_OUTLINEDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif


//============================================================================
//============================================================================
class effOutlineData : public effShaderData
{
public:
	effOutlineData();
	virtual ~effOutlineData();
	effOutlineData(const effOutlineData& i_CopyFrom);
	effOutlineData& operator = (const effOutlineData& i_CopyFrom);
	bool operator == (const effOutlineData& i_EffOutlineData) const;

	virtual effShaderData* Clone() const {return new effOutlineData(*this);}

	virtual void RemoveTextures();
	virtual chDefs::Name GetChunkName() const;

	matTexture* m_pTexture;
	float m_OutlineDepthScale;
	float m_OutlineMinAngle;
	float m_OutlineMaxAngle;
	float m_OutlineThickness;
	maFloatRGBA	m_OutlineColor;
	maVector2d m_OutlineViewSize;
	bool m_bUseDepths;
	bool m_bUseNormals;
	float m_OutlineMinWidth;
	float m_OutlineMaxWidth;
};
