/*****************************************************************************
**  mtrlPhongData.hpp
**
**      mtrlPhongData is the data for a shader effect.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_PHONGDATA_HPP
#error mtrlPhongData.hpp multiply included
#endif
#define MTRL_PHONGDATA_HPP

#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif

//============================================================================
//============================================================================
class mtrlPhongData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlPhongData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlPhongData(const mtrlPhongData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlPhongData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlPhongData& operator=(const mtrlPhongData& i_Data);

	prtyColor m_ColorAmbient;
	prtyColor m_ColorDiffuse;
	prtyColor m_ColorSpecular;
	prtyColor m_ColorEmissive;

	prtyFloat m_SpecularPower;
	prtyFloat m_BumpMapScale;
	prtyFloat m_Reflectivity;
	prtyFloat m_Transparency;
	prtyFloat m_UScale, m_VScale;
	prtyFloat m_UTrans, m_VTrans;
	prtyFloat m_UVAngle;
	prtyFloat m_DisplacementScale;
	prtyFloat m_DisplacementBias;

	prtyFileName m_TextureDiffuse;
	prtyFileName m_TextureSpecular;
	prtyFileName m_TextureGloss;
	prtyFileName m_TextureEnvironment;
	prtyFileName m_TextureNormalMap;
	prtyFileName m_TextureReflectFactorMap;
	prtyFileName m_TextureTransparencyMap;
	prtyFileName m_TextureDisplacementMap;
};

