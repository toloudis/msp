/****************************************************************************\
**  grpsGroupData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/grps/grpsGroupData.hpp"

#include <iterator>

//----------------------------------------------------------------------------
// Add data as new group, or merge data into 
// existing group with the same name.
//----------------------------------------------------------------------------
void grpsGroupsData::AddGroupData(const grpsGroupData& i_Data)
{
	const int num_groups = m_Groups.size();
	for (int i=0; i<num_groups; ++i)
	{
		// Look for group with the same name
		if (m_Groups[i].m_Name.GetString() == i_Data.m_Name.GetString())
		{
			// Merge objects from i_Data into
			// this data object, without adding in a new data struct.
			std::copy(i_Data.m_ObjectNames.begin(), i_Data.m_ObjectNames.end(), std::back_inserter(m_Groups[i].m_ObjectNames));
			return;
		}
	}

	// If we got here, there is no existing light group with the 
	// same name, so add it into the list as a new light group.
	m_Groups.push_back(i_Data);
}