/****************************************************************************\
**  lyerLayerData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/lyer/lyerLayerData.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lyerLayerData::lyerLayerData()
:	m_bVisible(true), 
	m_bPickable(true), 
	m_bWireframe(false), 
	m_bLowRes(false)
{

}

//----------------------------------------------------------------------------
// Add data as new layer, or merge data into 
// existing layer with the same name.
//----------------------------------------------------------------------------
void lyerLayersData::AddLayerData(const lyerLayerData& i_Data)
{
	const int num_layers = m_Layers.size();
	for (int i=0; i<num_layers; ++i)
	{
		// Look for layer with the same name
		if (m_Layers[i].m_Name.GetString() == i_Data.m_Name.GetString())
		{
			// Merge objects from i_Data into
			// this data object, without adding in a new data struct.
			std::copy(i_Data.m_Objects.begin(), i_Data.m_Objects.end(), std::back_inserter(m_Layers[i].m_Objects));
			return;
		}
	}

	// If we got here, there is no existing light layer with the 
	// same name, so add it into the list as a new light layer.
	m_Layers.push_back(i_Data);
}