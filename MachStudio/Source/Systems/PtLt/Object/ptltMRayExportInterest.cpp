/****************************************************************************\
**	ptltMRayExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Ptlt/Object/ptltMRayExportInterest.hpp"

#include "Systems/PtLt/Object/ptltObjectMgr.hpp"

#include "Support/mray/mrayExporter.hpp"

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* ptltMRayExportInterest::GetChunkDesc() const
{
	return "PointLights";
}

//--------------------------------------------------------------------
// Gather data for scene that will be exported
//--------------------------------------------------------------------
void ptltMRayExportInterest::GatherSceneData( mraySceneData &o_SceneData , mrayGlobalData &o_GlobalData ) const
{
	// put the names of the objects in the list
	const int num_objects = ptltObjectMgr::GetNumObjects();

	for (int i=0; i<num_objects; ++i)
	{
		ptltScriptObject *pScriptObject = ptltObjectMgr::GetObject(i);
		ptltPointLightObject *pObject = pScriptObject->GetPickObject();
		ptltData objData = pObject->GetData();		

		// Fill in pointLightData
		mrayPointLightData pointLightData; 
		pointLightData.m_Name = objData.m_Name.GetString();
		pointLightData.m_bEnabled = objData.m_Enabled.GetValue();
		std::string instanceName = pointLightData.m_Name + "_inst";
		pointLightData.m_InstanceName = instanceName;

		pointLightData.m_Color = objData.m_Color.GetValue();
		pointLightData.m_Intensity = objData.m_Intensity.GetValue();
		pointLightData.m_Position = objData.m_Position.GetValue();
		pointLightData.m_Falloff = objData.m_Falloff.GetValue();
		pointLightData.m_bAffectsDiffuse = objData.m_bDiffuseEnabled.GetValue();
		pointLightData.m_bAffectsSpecular = objData.m_bSpecularEnabled.GetValue();

		o_GlobalData.m_PointLightInstances.push_back( instanceName );

		o_GlobalData.m_SceneLights.push_back( pObject->GetPropertyName().GetValue().GetString() );

		o_SceneData.m_PointLightData.push_back( pointLightData );
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void ptltMRayExportInterest::Export( mrayExporter& i_Exporter,
									 const mraySceneData &i_SceneData)
{
	const int num_objects = i_SceneData.m_PointLightData.size();
	for (int i=0; i<num_objects; ++i)
	{
		// Export root node
		i_Exporter.ExportPointLight(i_SceneData.m_PointLightData[i]);
	}
}
