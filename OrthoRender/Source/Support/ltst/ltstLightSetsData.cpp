/****************************************************************************\
**  ltstLightSetsData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/ltst/ltstLightSetsData.hpp"


//----------------------------------------------------------------------------
// Add data as new light set, or merge data into 
// existing light set.
//----------------------------------------------------------------------------
void ltstLightSetsData::AddLightSetData(const ltstLightSetData& i_Data)
{
	const int num_sets = m_LightSets.size();
	for (int i=0; i<num_sets; ++i)
	{
		// Look for light set with the same name
		if (m_LightSets[i].m_Name.GetString() == i_Data.m_Name.GetString())
		{
			// Merge lights and objects from i_Data into
			// this data object, without adding in a new data struct.
			std::copy(i_Data.m_Lights.begin(), i_Data.m_Lights.end(), std::back_inserter(m_LightSets[i].m_Lights));
			std::copy(i_Data.m_Objects.begin(), i_Data.m_Objects.end(), std::back_inserter(m_LightSets[i].m_Objects));
			return;
		}
	}

	// If we got here, there is no existing light set with the 
	// same name, so add it into the list as a new light set.
	m_LightSets.push_back(i_Data);
}