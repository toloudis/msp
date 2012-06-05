/*****************************************************************************
**  effBillboardData.hpp
**
**      effBillboardData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_BILLBOARDDATA_HPP
#error effBillboardData.hpp multiply included
#endif
#define EFF_BILLBOARDDATA_HPP

#ifndef EFF_TEXTUREDDATA_HPP
#include "Graphics/eff/effTexturedData.hpp"
#endif


//============================================================================
//============================================================================
class matTexture;

//============================================================================
//============================================================================
class effBillboardData : public effTexturedData
{
public:
	effBillboardData();
	virtual ~effBillboardData();
	effBillboardData(const effBillboardData& i_CopyFrom);
	effBillboardData& operator = (const effBillboardData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effBillboardData(*this);}

	bool m_bCKActive;
	maFloatRGBA m_CKColor;
	float m_CKTolerance;
	bool m_bCKRemoveSpill;
	int m_CKSpillType;
	float m_CKSpillBias;
	int m_CKEdgeBlur;

	float m_Brightness;
};
