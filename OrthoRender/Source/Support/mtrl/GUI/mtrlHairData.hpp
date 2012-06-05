/*****************************************************************************
**  mtrlHairData.hpp
**
**      mtrlHairData is the data for a shader effect.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_HAIRDATA_HPP
#error mtrlHairData.hpp multiply included
#endif
#define MTRL_HAIRDATA_HPP

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
class mtrlHairData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlHairData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlHairData(const mtrlHairData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlHairData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlHairData& operator=(const mtrlHairData& i_Data);

	prtyColor m_HairBaseColor;

	prtyFloat m_BumpMapScale;
	prtyFloat m_UScale, m_VScale;
	prtyFloat m_UTrans, m_VTrans;
	prtyFloat m_UVAngle;
	prtyFloat m_Reflectivity;

	prtyFileName m_TextureBase;
	prtyFileName m_TextureAlpha;
	prtyFileName m_TextureSpecularShift;
	prtyFileName m_TextureSpecularMask;
	prtyFileName m_TextureNormalMap;

	prtyColor m_SpecularColor0;
	prtyColor m_SpecularColor1;
	prtyFloat m_SpecularExponent0;
	prtyFloat m_SpecularExponent1;
	prtyFloat m_SpecularShift0;
	prtyFloat m_SpecularShift1;

};

