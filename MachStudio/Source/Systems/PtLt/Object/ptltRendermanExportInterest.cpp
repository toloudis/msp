/****************************************************************************\
**	ptltRendermanExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Ptlt/Object/ptltRendermanExportInterest.hpp"


#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Graphics/smdl/private/smdlSubdivSurface.hpp"

#include "Systems/PtLt/Object/ptltObjectMgr.hpp"

#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Support/rman/rmanExporter.hpp"
#include "Support/rman/rmanMgr.hpp"


#include <algorithm>


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* ptltRendermanExportInterest::GetChunkDesc() const
{
	return "PointLights";
}

//--------------------------------------------------------------------
// Gather data for scene that will be exported
//--------------------------------------------------------------------
void ptltRendermanExportInterest::GatherSceneData( rmanSceneData &o_SceneData, rmanGlobalData &o_GlobalData ) const
{
	// put the names of the objects in the list
	const int num_objects = ptltObjectMgr::GetNumObjects();

	for (int i=0; i<num_objects; ++i)
	{
		ptltScriptObject *pScriptObject = ptltObjectMgr::GetObject(i);
		ptltPointLightObject *pObject = pScriptObject->GetPickObject();

		rmanPointLightData pointLightData;

		pointLightData.m_Name = pObject->GetPropertyName().GetValue().GetString();
		pointLightData.m_Position = pObject->GetPosition();
		maPoint3d color(pObject->GetPropertyColor().GetValue().GetRed(), 
						pObject->GetPropertyColor().GetValue().GetGreen(), 
						pObject->GetPropertyColor().GetValue().GetBlue() );
		pointLightData.m_Color = color;
		pointLightData.m_Intensity = pObject->GetPropertyIntensity().GetValue();
		pointLightData.m_Falloff = pObject->GetPropertyFalloff().GetValue();
		pointLightData.m_FalloffStart = pObject->GetLight()->GetFalloffStart();		
		pointLightData.m_bEnableLight = pObject->GetLight()->IsEnabled();
		pointLightData.m_bEnableDiffuse = pObject->GetLight()->IsDiffuseEnabled();
		pointLightData.m_bEnableSpecular = pObject->GetLight()->IsSpecularEnabled();
		pointLightData.m_bAffectsGlow = pObject->GetLight()->GetAffectsGlow();

		o_GlobalData.m_SceneLights.push_back( pObject->GetPropertyName().GetValue().GetString() );

		o_SceneData.m_PointLightData.push_back( pointLightData );
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void ptltRendermanExportInterest::Export( rmanExporter& i_Exporter,
										  const rmanSceneData &i_SceneData)
{
	const int num_objects = i_SceneData.m_PointLightData.size();
	for (int i=0; i<num_objects; ++i)
	{
		i_Exporter.ExportPointLight(i_SceneData.m_PointLightData[i]);
	}
}
