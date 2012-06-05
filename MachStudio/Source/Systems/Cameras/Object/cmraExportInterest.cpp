/****************************************************************************\
**	cmraExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Object/cmraExportInterest.hpp"

#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"

#include "Support/mexp/mexpExporter.hpp"
#include "Support/mexp/mexpExporterUtil.hpp"

#include <algorithm>

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* cmraExportInterest::GetChunkDesc() const
{
	return "Cameras";
}

//--------------------------------------------------------------------
// Gather names of items that could potentially be exported.
//--------------------------------------------------------------------
void cmraExportInterest::GatherItemNames(std::vector<std::string> &o_Items) const
{
	// put the names of the objects in the list
	const int num_objects = cmraObjectMgr::GetNumObjects();
	o_Items.resize( num_objects );
	for (int i=0; i<num_objects; ++i)
	{
		o_Items[i] = cmraObjectMgr::GetObject(i)->GetName().GetString();
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void cmraExportInterest::Export( mexpExporter& i_Exporter,
						const std::vector<std::string> &i_Items)
{
	const int num_objects = cmraObjectMgr::GetNumObjects();
	for (int i=0; i<num_objects; ++i)
	{
		cmraCameraData data = cmraObjectMgr::GetBaseData(i);
		// If the name of this object is in the Items list, then export it
		if (std::find(i_Items.begin(), i_Items.end(), data.m_Name.GetString()) != i_Items.end())
		{
			cmraScriptObject *pScriptObject = cmraObjectMgr::GetObject(i);
			cmraCameraObject *pObject = pScriptObject->GetPickObject();

			if (i_Exporter.IsExportAnimation())
			{
				std::string maya_name = i_Exporter.ExportCamera(data.m_Name.GetString(),
					data.m_Position.GetValue(),
					data.m_Target.GetValue(),
					data.m_FOV.GetValue());

				mexpExporterUtil::PrepareCameraPositionAnimation(i_Exporter,
					maya_name,
					pScriptObject->PositionChannel(),
					pObject->GetPropertyPosition());
				mexpExporterUtil::PrepareCameraTargetAnimation(i_Exporter,
					maya_name,
					pScriptObject->TargetChannel(),
					pObject->GetPropertyTarget());
				mexpExporterUtil::PrepareCameraFOVAnimation(i_Exporter,
					maya_name,
					pScriptObject->FOVChannel(),
					pObject->GetPropertyFieldOfView());
			}
			else
			{
				// export current pose, which means exporting the current value
				// of the properties, not the values in the data structure.
				i_Exporter.ExportCamera(data.m_Name.GetString(),
					pObject->GetPropertyPosition().GetValue(),
					pObject->GetPropertyTarget().GetValue(),
					pObject->GetPropertyFieldOfView().GetValue());
			}
		}
	}
}
