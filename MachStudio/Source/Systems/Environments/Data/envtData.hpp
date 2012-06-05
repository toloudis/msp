/********************************************************************************************\
**  envtData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#ifdef ENVT_DATA_HPP
#error envtData.hpp multiply included
#endif
#define ENVT_DATA_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif
#ifndef RMP_DATA_HPP
#include "Support/rmp/rmpData.hpp"
#endif
#ifndef SWL_DATA_HPP
#include "Support/swl/swlData.hpp"
#endif

// enable this flag for swl ui
//#define USE_SWL_UI

//============================================================================
//============================================================================
class envtData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	envtData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	envtData(const envtData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~envtData();

	//-------------------------------------------------
	bool operator == (const envtData& i_Item);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	envtData& operator=(const envtData& i_Data);

public:
	prtyName			m_Name;

	prtyColor			m_DiffuseColor;
	prtyTextureFileName	m_DiffuseMapName;
	prtyFloat			m_DiffuseFactor;
	prtyFloat			m_DiffuseAngle;
	prtyColor			m_SpecularColor;
	prtyTextureFileName	m_SpecularMapName;
	prtyFloat			m_SpecularFactor;
	prtyFloat			m_SpecularAngle;

	rmpData				m_RampData;					// Ramp data wrapper
	swlData				m_SwlData;
	
	std::vector<nameString> m_Objects;
};
