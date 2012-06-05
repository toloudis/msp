/****************************************************************************\
**	setsExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Sets/setsExportInterest.hpp"

#include "Systems/Sets/Data/setsDataMgr.hpp"
#include "Systems/Sets/Data/setsScriptData.hpp"

#include "Core/ma/maRotation.hpp"
#include "Support/mexp/mexpExporter.hpp"

#include <algorithm>

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* setsExportInterest::GetChunkDesc() const
{
	return "Sets";
}

//--------------------------------------------------------------------
// Gather names of items that could potentially be exported.
//--------------------------------------------------------------------
void setsExportInterest::GatherItemNames(std::vector<std::string> &o_Items) const
{
	// put the names of the objects in the list
	const int num_objects = setsDataMgr::GetNumSetItems();
	o_Items.resize( num_objects );
	for (int i=0; i<num_objects; ++i)
	{
		const setsScriptData& data = setsDataMgr::GetItemData(i);
		o_Items[i] = itStringUtil::GetStdString(data.m_BaseData.m_Filename.GetValue());
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void setsExportInterest::Export( mexpExporter& i_Exporter,
						const std::vector<std::string> &i_Items)
{
	const int num_objects = setsDataMgr::GetNumSetItems();
	for (int i=0; i<num_objects; ++i)
	{
		const setsScriptData& data = setsDataMgr::GetItemData(i);
		std::string name = itStringUtil::GetStdString(data.m_BaseData.m_Filename.GetValue());
		// If the name of this object is in the Items list, then export it
		if (std::find(i_Items.begin(), i_Items.end(), name) != i_Items.end())
		{
			i_Exporter.ExportGeometryLocation(data.m_BaseData.m_Filename.GetValue(),
				maPoint3d(0,0,0),
				maRotation());
		}
	}
}
