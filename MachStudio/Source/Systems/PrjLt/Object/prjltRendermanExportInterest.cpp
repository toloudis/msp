/****************************************************************************\
**	prjltRendermanExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Prjlt/Object/prjltRendermanExportInterest.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Graphics/smdl/private/smdlSubdivSurface.hpp"

#include "Systems/Prjlt/Object/prjltObjectMgr.hpp"

#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Support/rman/rmanExporter.hpp"
#include "Support/rman/rmanMgr.hpp"


#include <algorithm>


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* prjltRendermanExportInterest::GetChunkDesc() const
{
	return "ProjLights";
}

//--------------------------------------------------------------------
// Gather data for scene that will be exported
//--------------------------------------------------------------------
void prjltRendermanExportInterest::GatherSceneData( rmanSceneData &o_SceneData, rmanGlobalData &o_GlobalData ) const
{
	// put the names of the objects in the list
	const int num_objects = prjltObjectMgr::GetNumObjects();

	for (int i=0; i<num_objects; ++i)
	{
		prjltScriptObject *pScriptObject = prjltObjectMgr::GetObject(i);
		prjltProjectedLightObject *pObject = pScriptObject->GetPickObject();
		prjltData objData = pObject->GetData();

		rmanProjLightData projLightData; 

		std::string lightName = pObject->GetPropertyName().GetValue().GetString();

		projLightData.m_Name = lightName;
		projLightData.m_Position = pObject->GetPosition();
		maPoint3d color(pObject->GetPropertyColor().GetValue().GetRed(), 
						pObject->GetPropertyColor().GetValue().GetGreen(), 
						pObject->GetPropertyColor().GetValue().GetBlue() );
		projLightData.m_Color = color;
		projLightData.m_Intensity = pObject->GetPropertyIntensity().GetValue();
		projLightData.m_Falloff = pObject->GetPropertyFalloff().GetValue();
		projLightData.m_FalloffStart = pObject->GetLight()->GetFalloffStart();		
		projLightData.m_Target = pObject->GetPropertyTarget().GetValue();
		projLightData.m_Angle = pObject->GetPropertyAngle().GetValue();
		projLightData.m_Scale = pObject->GetPropertyScale().GetValue();
		projLightData.m_Range = pObject->GetPropertyRange().GetValue();
		projLightData.m_Aspect = pObject->GetPropertyAspect().GetValue();
		projLightData.m_bEnableLight = pObject->GetLight()->IsEnabled();
		projLightData.m_bEnableDiffuse = pObject->GetLight()->IsDiffuseEnabled();
		projLightData.m_bEnableSpecular = pObject->GetLight()->IsSpecularEnabled();
		projLightData.m_bAffectsGlow = pObject->GetLight()->GetAffectsGlow();
		projLightData.m_ProjectedLight = pObject->GetLight();

		prjltData::LightType lightType = objData.m_LightType;
		bool directional = false;
		bool spotlight = false;
		bool pureDirectional = false;
		if ( lightType == prjltData::e_ProjectedLight )
		{
			directional = objData.m_bDirectional.GetValue();
			spotlight = objData.m_bConeLighting.GetValue();
		}
		else if ( lightType == prjltData::e_SpotLight )
		{
			spotlight = true;
		}
		else if ( lightType == prjltData::e_DirectionalLight )
		{
			directional = true;
			pureDirectional = true;
		}

		projLightData.m_Penumbra = pObject->GetPropertyPenumbra().GetValue();
		projLightData.m_bDirectional = directional;
		projLightData.m_bPureDirectional = pureDirectional;
		projLightData.m_bConeLighting = spotlight;
		projLightData.m_InnerAngle = pObject->GetLight()->GetInnerAngle();

		projLightData.m_ShadowSoftness = objData.m_LightSize.GetValue();
		projLightData.m_ShadowDepthBias = objData.m_DepthBias.GetValue();
		projLightData.m_ShadowIntensity = objData.m_ShadowIntensity.GetValue();
		projLightData.m_ShadowPCSS = objData.m_PCSSAdjust.GetValue();
		projLightData.m_ShadowMapRes = objData.m_DepthMapSize.GetValue();
		projLightData.m_ShadowQuality = objData.m_ShadowQuality.GetValue();
		projLightData.m_ShadowSource = objData.m_ShadowSource.GetValue();

		matTexture* projMat = NULL;
		std::string projMapName = "";
	
		bool usingRamp = objData.m_bEnabledRamp.GetValue() && pObject->GetRampTexture();
		bool usingTex = (!objData.m_bEnabledRamp.GetValue()) && pObject->GetTexture();

		fsLocator texLoc;

		if ( usingRamp )
		{
			texLoc = o_GlobalData.m_TexturesLoc;
			std::string texNameWithExt = lightName + "_RAMP.tif";
			texLoc.Push( texNameWithExt.c_str() );
			projMat = pObject->GetRampTexture();
		}
		else if ( usingTex )
		{
			texLoc = objData.m_TextureFilename.GetValue();
			projMat = pObject->GetTexture();
		}		

		rmanMgr::InsertPotentialTexture( lightName + projMapEnd, projMat , texLoc, o_GlobalData, usingRamp );	

		// Using projected map
		if ( projMat )
		{
			projMapName = lightName;
			projMapName.append(projMapEnd);
		}

		projLightData.m_Ramp = projMapName;

		maPoint3d shadowColor = maPoint3d( objData.m_ShadowColor.GetValue().GetRed(),
										   objData.m_ShadowColor.GetValue().GetGreen(),
										   objData.m_ShadowColor.GetValue().GetBlue());

		projLightData.m_ShadowColor = shadowColor;

		camCamera camera;
		pObject->GetLight()->OrientCamera( camera );
		maMatrix4x4 transform;
		camera.GetCameraMatrix( transform );	
		projLightData.m_CameraTransform = transform;

		if ( objData.m_ShadowSource.GetValue() )
		{
			std::string shadowMapFileName = lightName;
			shadowMapFileName.append("_SHADOWMAP.z");
			o_GlobalData.m_ShadowMapRibs.push_back( fsLocator( itString(shadowMapFileName.c_str()) ) );
		}

		o_GlobalData.m_SceneLights.push_back( pObject->GetPropertyName().GetValue().GetString() );

		o_SceneData.m_ProjLightData.push_back( projLightData );

	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void prjltRendermanExportInterest::Export( rmanExporter& i_Exporter,
										  const rmanSceneData &i_SceneData)
{
	const int num_objects = i_SceneData.m_ProjLightData.size();
	for (int i=0; i<num_objects; ++i)
	{
		i_Exporter.ExportProjLight(i_SceneData.m_ProjLightData[i]);
	}
}
