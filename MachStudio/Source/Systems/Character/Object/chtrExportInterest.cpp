/****************************************************************************\
**	chtrExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Object/chtrExportInterest.hpp"

#include "Systems/Character/Object/chtrObjectMgr.hpp"

#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Support/mexp/mexpExporter.hpp"
#include "Support/mexp/mexpExporterUtil.hpp"
#include "Graphics/sc/scObject.hpp"

#include <algorithm>


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* chtrExportInterest::GetChunkDesc() const
{
	return "Objects";
}

//--------------------------------------------------------------------
// Gather names of items that could potentially be exported.
//--------------------------------------------------------------------
void chtrExportInterest::GatherItemNames(std::vector<std::string> &o_Items) const
{
	// put the names of the objects in the list
	const int num_objects = chtrObjectMgr::GetNumObjects();
	o_Items.resize( num_objects );
	for (int i=0; i<num_objects; ++i)
	{
		o_Items[i] = chtrObjectMgr::GetObject(i)->GetName().GetString();
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void chtrExportInterest::Export( mexpExporter& i_Exporter,
						const std::vector<std::string> &i_Items)
{
	const int num_objects = chtrObjectMgr::GetNumObjects();
	for (int i=0; i<num_objects; ++i)
	{
		chtrData data = chtrObjectMgr::GetBaseData(i);
		// If the name of this object is in the Items list, then export it
		if (std::find(i_Items.begin(), i_Items.end(), data.m_Name.GetString()) != i_Items.end())
		{
			chtrScriptObject *pScriptObject = chtrObjectMgr::GetObject(i);
			chtrObject *pObject = pScriptObject->GetPickObject();

			std::string maya_name;
			if (i_Exporter.IsExportAnimation())
			{
				maya_name = i_Exporter.ExportGeometryLocation(data.m_Filename.GetValue().GetLastName(),
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
				maya_name = i_Exporter.ExportGeometryLocation(data.m_Filename.GetValue().GetLastName(),
					pObject->GetPropertyPosition().GetValue(),
					pObject->GetPropertyOrientation().GetQuaternion());
			}

			// Export scene graph hierarchy if requested.
			if (i_Exporter.IsExportJoints())
			{
				mexpExporterUtil::ExportHierarchy(i_Exporter,
					maya_name,
					pObject->GetEntity()->GetObject()->GetBase());
			}
		}
	}
}
