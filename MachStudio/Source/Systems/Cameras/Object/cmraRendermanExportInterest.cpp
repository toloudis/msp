/****************************************************************************\
**	cmraRendermanExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Object/cmraRendermanExportInterest.hpp"

#include "Graphics/cam/camCamera.hpp"

#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"

#include "Support/rman/rmanExporter.hpp"
#include "Support/rman/rmanMgr.hpp"

#include <algorithm>

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* cmraRendermanExportInterest::GetChunkDesc() const
{
	return "Cameras";
}

//--------------------------------------------------------------------
// Gather data for objects that will be exported
//--------------------------------------------------------------------
void cmraRendermanExportInterest::GatherSceneData( rmanSceneData &o_SceneData, rmanGlobalData &o_GlobalData ) const
{
	// put the names of the objects in the list
	const int num_objects = cmraObjectMgr::GetNumObjects();

	for (int i=0; i<num_objects; ++i)
	{
		cmraScriptObject *pScriptObject = cmraObjectMgr::GetObject(i);
		cmraCameraObject *pObject = pScriptObject->GetPickObject();

		//rmanMgr::SetNumCaptureDrivers( pScriptObject->GetNumCaptureDrivers() );
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void cmraRendermanExportInterest::Export( rmanExporter& i_Exporter, const rmanSceneData &i_SceneData )
{
}
