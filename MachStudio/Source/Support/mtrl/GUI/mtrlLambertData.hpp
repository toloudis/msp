/*****************************************************************************
**  mtrlLambertData.hpp
**
**      mtrlLambertData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_LAMBERTDATA_HPP
#error mtrlLambertData.hpp multiply included
#endif
#define MTRL_LAMBERTDATA_HPP

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
class mtrlLambertData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlLambertData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlLambertData(const mtrlLambertData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlLambertData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlLambertData& operator=(const mtrlLambertData& i_Data);

	prtyColor m_ColorAmbient;
	prtyColor m_ColorDiffuse;
	prtyColor m_ColorEmissive;

	prtyFloat m_BumpMapScale;
	prtyFloat m_Transparency;
	prtyFloat m_UScale, m_VScale;
	prtyFloat m_UTrans, m_VTrans;
	prtyFloat m_UVAngle;

	prtyFileName m_TextureDiffuse;
	prtyFileName m_TextureNormalMap;
	prtyFileName m_TextureTransparencyMap;
};

