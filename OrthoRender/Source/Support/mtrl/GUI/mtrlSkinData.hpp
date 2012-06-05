/*****************************************************************************
**  mtrlSkinData.hpp
**
**      mtrlSkinData is the data for a shader effect.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_SKINDATA_HPP
#error mtrlSkinData.hpp multiply included
#endif
#define MTRL_SKINDATA_HPP

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
class mtrlSkinData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlSkinData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlSkinData(const mtrlSkinData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlSkinData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlSkinData& operator=(const mtrlSkinData& i_Data);

	prtyColor m_SpecColor;
	prtyFloat m_SpecPower;
	prtyFloat m_SpecGloss;
	prtyFloat m_SpecFresnel;
	prtyFloat m_FresnelPower;
	prtyFloat m_FresnelGloss;
	prtyColor m_TransColIn;
	prtyColor m_TransColOut;
	prtyColor m_TransColBack;
	prtyFloat m_TransMultiplier;
	prtyFloat m_TransRampOff;
	prtyFloat m_MicroScale;

	prtyFileName m_DiffTex;
	prtyFileName m_NormalTex;
	prtyFileName m_MicroTex;
	prtyFileName m_SpecTex;
	prtyFileName m_SpecPowerTex;
	prtyFileName m_TransTex;
	prtyFileName m_CubeMapTex;
	prtyFileName m_ReflectFactorTex;
	prtyFileName m_TransparencyTex;

	prtyFloat m_BumpMapScale;
	prtyFloat m_Transparency;
	prtyFloat m_UScale, m_VScale;
	prtyFloat m_UTrans, m_VTrans;
	prtyFloat m_UVAngle;

};

