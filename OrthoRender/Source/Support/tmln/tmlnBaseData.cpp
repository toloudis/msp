/****************************************************************************\
**  tmlnBaseData.cpp
**
**		see .hpp
**
**  TODO: phase out this class/file
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnBaseData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tmlnBaseData::tmlnBaseData() 
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool tmlnBaseData::operator == (const tmlnBaseData& i_Item)
{
	// FIX: [rjk] loop through the drivers and verify 1 to 1 match
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tmlnBaseData& tmlnBaseData::operator=(const tmlnBaseData& i_Data)
{
	if (this == &i_Data) return *this;

	envSTLHelpers::DeleteContainer(this->m_Drivers);

	int num_drivers = i_Data.m_Drivers.size();
	for (int i=0; i<num_drivers; i++)
	{
		this->m_Drivers.push_back(i_Data.m_Drivers[i]->Clone());
	}

	return *this;
}

//------------------------------------------------------------------------
// FIX: [rjk] temporary until all scene files are converted.
//------------------------------------------------------------------------
tmlnBaseData& tmlnBaseData::operator = (const mnmBaseData& i_Data)
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);

	int num_drivers = i_Data.m_Drivers.size();
	for (int i=0; i<num_drivers; i++)
	{
		this->m_Drivers.push_back(i_Data.m_Drivers[i]->Clone());
	}

	return *this;
}
