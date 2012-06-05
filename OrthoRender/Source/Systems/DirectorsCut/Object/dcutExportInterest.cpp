/****************************************************************************\
**	dcutExportInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Object/dcutExportInterest.hpp"

#include "Systems/DirectorsCut/Timeline/dcutChannelCamera.hpp"
#include "Systems/DirectorsCut/Object/dcutDirectorsCutObject.hpp"
#include "Systems/DirectorsCut/Object/dcutObjectMgr.hpp"
#include "Systems/DirectorsCut/Object/dcutScriptObject.hpp"

#include "Tool/cam3d/cam3dMgr.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/mexp/mexpExporter.hpp"
#include "Support/mexp/mexpRecorderDirectorsCut.hpp"

#include <algorithm>

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* dcutExportInterest::GetChunkDesc() const
{
	return "Director's Cuts";
}

//--------------------------------------------------------------------
// Gather names of items that could potentially be exported.
//--------------------------------------------------------------------
void dcutExportInterest::GatherItemNames(std::vector<std::string> &o_Items) const
{
	// put the names of the objects in the list
	const int num_objects = dcutObjectMgr::GetNumObjects();
	o_Items.resize( num_objects );
	for (int i=0; i<num_objects; ++i)
	{
		o_Items[i] = dcutObjectMgr::GetObject(i)->GetName().GetString();
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void dcutExportInterest::Export( mexpExporter& io_Exporter,
						const std::vector<std::string> &i_Items)
{
	const int num_objects = dcutObjectMgr::GetNumObjects();
	for (int i=0; i<num_objects; ++i)
	{
		dcutScriptObject *pScriptObject = dcutObjectMgr::GetObject(i);
		dcutDirectorsCutObject *pObject = pScriptObject->GetPickObject();

		// If the name of this object is in the Items list, then export it
		if (std::find(i_Items.begin(), i_Items.end(), pObject->GetName().GetString()) != i_Items.end())
		{
			// Use the current camera index to export the base camera values
			int cam_index = pObject->GetCameraIndex();
			const camCamera *pCamera = NULL;
			if (cam_index >= 0 && cam_index < camsCameraMgr::GetNumCameras())
				pCamera = camsCameraMgr::GetCamera(cam_index);

			// If we can't get a camera, then export the editor camera
			if (!pCamera)
			{
				pCamera = &cam3dMgr::GetEditorCamera();
			}
				
			std::string maya_name = io_Exporter.ExportCamera(pObject->GetName().GetString(),
					pCamera->GetPosition(),
					pCamera->GetTarget(),
					pCamera->GetFOV());


			if (io_Exporter.IsExportAnimation())
			{
				if (pScriptObject->CameraChannel().GetNumDrivers() > 0)
				{
					io_Exporter.AddChannelRecorder( 
						new mexpRecorderDirectorsCut(maya_name, i)  );
				}
			}
		}
	}
}
