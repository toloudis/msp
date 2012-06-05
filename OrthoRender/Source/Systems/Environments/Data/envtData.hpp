/********************************************************************************************\
**  envtData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#ifdef ENVT_DATA_HPP
#error envtData.hpp multiply included
#endif
#define ENVT_DATA_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif


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
	prtyName		m_Name;

	prtyFileName	m_DiffuseMapName;
	prtyFloat		m_DiffuseFactor;
	prtyFloat		m_DiffuseAngle;
	prtyFileName	m_SpecularMapName;
	prtyFloat		m_SpecularFactor;
	prtyFloat		m_SpecularAngle;

	std::vector<nameString> m_Objects;
};
