/*****************************************************************************
**  mtrlToonData.hpp
**
**      mtrlToonData is the data for a shader effect.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_TOONDATA_HPP
#error mtrlToonData.hpp multiply included
#endif
#define MTRL_TOONDATA_HPP

#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif


//============================================================================
//============================================================================
class mtrlToonData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlToonData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlToonData(const mtrlToonData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlToonData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlToonData& operator=(const mtrlToonData& i_Data);

	prtyColor m_ColorAmbient;
	prtyColor m_ColorMidtone;
	prtyColor m_ColorDiffuse;
	prtyColor m_ColorSpecular;

	prtyFloat m_Transparency;
	prtyFloat m_UScale, m_VScale;
	prtyFloat m_UTrans, m_VTrans;
	prtyFloat m_UVAngle;

	prtyFloat m_Transition1;
	prtyFloat m_Transition2;
	prtyFloat m_Transition3;

	prtyFileName m_TextureDiffuse;
	prtyFileName m_TextureGradientMap;
	prtyFileName m_TextureTransparencyMap;

	prtyBoolean m_SpecularEnable;
	prtyFloat m_Smoothness;

};

