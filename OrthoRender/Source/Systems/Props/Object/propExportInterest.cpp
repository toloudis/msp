/****************************************************************************\
**	propExportInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/Object/propExportInterest.hpp"

#include "Systems/Props/Object/propObjectMgr.hpp"

#include "Support/mexp/mexpExporter.hpp"
#include "Support/mexp/mexpExporterUtil.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"

#include <algorithm>

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* propExportInterest::GetChunkDesc() const
{
	return "Props";
}

//--------------------------------------------------------------------
// Gather names of items that could potentially be exported.
//--------------------------------------------------------------------
void propExportInterest::GatherItemNames(std::vector<std::string> &o_Items) const
{
	// put the names of the objects in the list
	const int num_objects = propObjectMgr::GetNumObjects();
	o_Items.resize( num_objects );
	for (int i=0; i<num_objects; ++i)
	{
		o_Items[i] = propObjectMgr::GetObject(i)->GetName().GetString();
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void propExportInterest::Export( mexpExporter& i_Exporter,
								 const std::vector<std::string> &i_Items)
{
	const int num_objects = propObjectMgr::GetNumObjects();
	for (int i=0; i<num_objects; ++i)
	{
		propData data = propObjectMgr::GetBaseData(i);
		
		// If the name of this object is in the Items list, then export it
		if (std::find(i_Items.begin(), i_Items.end(), data.m_Name.GetString()) != i_Items.end())
		{
			propScriptObject *pScriptObject = propObjectMgr::GetObject(i);
			propPropObject *pObject = pScriptObject->GetPickObject();
			
			if (i_Exporter.IsExportAnimation())
			{
				std::string maya_name = i_Exporter.ExportGeometryLocation(data.m_Filename.GetValue(),
					data.m_Position.GetValue(),
					data.m_Orientation.GetQuaternion());

				mexpExporterUtil::PrepareTranslationAnimation(i_Exporter,
					maya_name,
					pScriptObject->ChannelPosition(),
					pObject->GetPropertyPosition());
				mexpExporterUtil::PrepareRotationAnimation(i_Exporter,
					maya_name,
					pScriptObject->ChannelOrientation(),
					pObject->GetPropertyOrientation());
			}
			else
			{
				// export current pose, which means exporting the current value
				// of the properties, not the values in the data structure.
				i_Exporter.ExportGeometryLocation(data.m_Filename.GetValue(),
					pObject->GetPropertyPosition().GetValue(),
					pObject->GetPropertyOrientation().GetQuaternion());
			}
		}
	}
}
