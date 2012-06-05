/****************************************************************************\
**	cmraFBXExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Object/cmraFBXExportInterest.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include "Graphics/cam/camCamera.hpp"

#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"

#include "Support/fbx/fbxExporter.hpp"
#include "Support/fbx/fbxMgr.hpp"


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* cmraFBXExportInterest::GetChunkDesc() const
{
	return "Cameras";
}

//--------------------------------------------------------------------
// Gather data for scene that will be exported
//--------------------------------------------------------------------
void cmraFBXExportInterest::GatherSceneData( fbxSceneData &o_SceneData , bool &o_SceneHasBeenBaked ) const
{
	// put the names of the objects in the list
	const int num_objects = cmraObjectMgr::GetNumObjects();

	for (int i=0; i<num_objects; ++i)
	{
		cmraScriptObject *pScriptObject = cmraObjectMgr::GetObject(i);
		cmraCameraObject *pObject = pScriptObject->GetPickObject();
		cmraCameraData data = pObject->GetData();

		fbxCameraData cameraData;

		cameraData.m_Name = data.m_Name.GetValue().GetString();
		cameraData.m_Position = data.m_Position.GetValue();
		cameraData.m_Target = data.m_Target.GetValue();
		cameraData.m_FOV = data.m_FOV.GetValue();
		cameraData.m_NearClip = data.m_Near.GetValue();
		cameraData.m_FarClip = data.m_Far.GetValue();
		cameraData.m_bEnableALP = data.m_bEnableALP.GetValue();
		cameraData.m_FocalLength = data.m_FocalLength.GetValue();
		cameraData.m_HorizontalAperture = data.m_HorizontalAperture.GetValue();
		cameraData.m_bIsOrthographic = pObject->GetCamera().IsOrthographic();

		o_SceneData.m_CameraData.push_back( cameraData );

	}	
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//--------------------------------------------------------------------
void cmraFBXExportInterest::Export( fbxExporter& i_Exporter, const fbxSceneData &i_SceneData )
{
	const int num_objects = i_SceneData.m_CameraData.size();
	for (int i=0; i<num_objects; ++i)
	{
		// Export root node
		i_Exporter.ExportCamera(i_SceneData.m_CameraData[i]);
	}
}
