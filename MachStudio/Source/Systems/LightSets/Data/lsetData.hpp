/********************************************************************************************\
**  lsetData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#ifdef LSET_DATA_HPP
#error lsetData.hpp multiply included
#endif
#define LSET_DATA_HPP

#ifndef LTST_LIGHTSETSDATA_HPP
#include "Support/ltst/ltstLightSetsData.hpp"
#endif 

#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif 
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif


//============================================================================
//============================================================================
class lsetData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	lsetData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	lsetData(const lsetData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~lsetData();

	//-------------------------------------------------
	bool operator == (const lsetData& i_Item);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	lsetData& operator=(const lsetData& i_Data);

public:
	prtyName		m_Name;

	std::vector<nameString> m_Lights;
	std::vector<ltstLightSetObjectData> m_Objects;
};
