/****************************************************************************\
**	prjltMRayExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Prjlt/Object/prjltMRayExportInterest.hpp"

#include "Systems/Prjlt/Object/prjltObjectMgr.hpp"

#include "Support/mray/mrayExporter.hpp"
#include "Support/mray/mrayMgr.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

namespace
{
	const int ShadowQualityNum = 4;
	const int ShadowQualityMap[ShadowQualityNum] = {5, 7, 9, 15};

void GetLeftAndUp(const prjltData& i_Data, maVector3d& o_Left, maVector3d& o_Up)
{
	maPoint3d i_Pos = i_Data.m_Position.GetWorldSpaceValue();
	maPoint3d i_Target = i_Data.m_Target.GetWorldSpaceValue();
	float i_Tilt = i_Data.m_Tilt.GetValue();
	float i_Scale = i_Data.m_Scale.GetValue() /* * m_ParentScale */;
	float i_Angle = i_Data.m_Angle.GetValue();
	bool i_bDirectional = i_Data.m_bDirectional.GetValue();

	// Get up vector (can't be (0,1,0), if the pitch is 90)
	maVector3d cam_up(0,1,0);
	maVector3d view_diff = i_Target - i_Pos;
	if ( view_diff.m_Z * view_diff.m_Z + view_diff.m_X * view_diff.m_X  < maConstants::c_fEpsilon )
		cam_up.Set(0,0,1);

	// Compute tilted up vector
	if (i_Tilt != 0.0f)
	{
		maRotation rot(view_diff, maConstants::c_fAngleToRad * i_Tilt);
		rot.RotateVector(cam_up);
	}

	// Construct view matrix
	maMatrix4x4 cam_matx;
	cam_matx.LookAt(i_Pos, i_Target, cam_up);

	maVector3d left(cam_matx.m_Mat[0], cam_matx.m_Mat[1], cam_matx.m_Mat[2]);
	maVector3d up  (cam_matx.m_Mat[4], cam_matx.m_Mat[5], cam_matx.m_Mat[6]);
	maVector3d view(-cam_matx.m_Mat[8], -cam_matx.m_Mat[9], -cam_matx.m_Mat[10]);

	// Apply the scale and range
	left	*= i_Scale;
	up		*= i_Scale;
	view	*= i_Scale;

	// Directional projected lights use orthographic frustrums and
	// therefore don't consider field of view
	if (i_bDirectional)
	{
		left *= 0.5f;
		up *= 0.5f;
	}
	else
	{
		// Incorporate field of view into icon
		float tan_fov = tanf((i_Angle / 2.0f) * maConstants::c_fAngleToRad);
		//if (i_Angle > 90)
		//{
		//	// In large angles, shorten view vector
		//	view /= tan_fov;
		//}
		//else if (i_Angle < 90)
		//{
		//	// In small angles, shorten left and up vectors
			left *= tan_fov;
			up *= tan_fov;
		//}
	}

	o_Left = left;
	o_Up = up;
}
};//namespace

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* prjltMRayExportInterest::GetChunkDesc() const
{
	return "ProjLights";
}

//--------------------------------------------------------------------
// Gather data for scene that will be exported
//--------------------------------------------------------------------
void prjltMRayExportInterest::GatherSceneData( mraySceneData &o_SceneData , mrayGlobalData &o_GlobalData ) const
{
	// put the names of the objects in the list
	const int num_objects = prjltObjectMgr::GetNumObjects();

	for (int i=0; i<num_objects; ++i)
	{
		prjltScriptObject *pScriptObject = prjltObjectMgr::GetObject(i);
		prjltProjectedLightObject *pObject = pScriptObject->GetPickObject();
		prjltData objData = pObject->GetData();

		// Fill in projLightData
		mrayProjLightData projLightData; 

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
		projLightData.m_bDirectional = directional;
		projLightData.m_bPureDirectional = pureDirectional;
		projLightData.m_bConeLighting = spotlight;

		projLightData.m_Name = objData.m_Name.GetString();
		projLightData.m_bEnabled = objData.m_Enabled.GetValue();

		std::string instanceName = projLightData.m_Name + "_inst";
		projLightData.m_InstanceName = instanceName;

		projLightData.m_Color = objData.m_Color.GetValue();
		projLightData.m_Intensity = objData.m_Intensity.GetValue();
		projLightData.m_Position = objData.m_Position.GetValue();
		projLightData.m_Target = objData.m_Target.GetValue();
		projLightData.m_Direction = objData.m_Target.GetValue() - objData.m_Position.GetValue();
		if (spotlight)
		{
			// Cone lighting has angle as iner "hot spot" lighting angle
			projLightData.m_InnerAngle = objData.m_Angle.GetValue();
			float total_angle = objData.m_Angle.GetValue() + objData.m_Penumbra.GetValue();
			maFunctions::Clamp(total_angle, 0.0f, 179.9f);
			projLightData.m_Angle = total_angle;
		}
		else
		{
			// Otherwise, inner angle is set to maximum and the full 
			// rectangular light frustrum is used.
			projLightData.m_Angle = objData.m_Angle.GetValue();
			projLightData.m_InnerAngle = 180.0f;
		}
		projLightData.m_bCastShadow = objData.m_ShadowSource.GetValue();
		projLightData.m_ShadowColor = objData.m_ShadowColor.GetValue();
		projLightData.m_ShadowIntensity = objData.m_ShadowIntensity.GetValue();
		projLightData.m_Falloff = objData.m_Falloff.GetValue();
		projLightData.m_Scale = objData.m_Scale.GetValue();
		projLightData.m_Range = objData.m_Range.GetValue();
		projLightData.m_Aspect = objData.m_Aspect.GetValue();
		projLightData.m_LightSize = objData.m_LightSize.GetValue();
		projLightData.m_DepthMapSize = objData.m_DepthMapSize.GetValue();
		projLightData.m_ShadowQuality = ShadowQualityMap[(objData.m_ShadowQuality.GetValue() % ShadowQualityNum)];
		projLightData.m_DepthBias = objData.m_DepthBias.GetValue();

		projLightData.m_bAreaLight = objData.m_bMRayAreaLight.GetValue();
		projLightData.m_AreaLightType = objData.m_MRayAreaLightType.GetValue();
		projLightData.m_bAreaLightVisible = objData.m_bMRayAreaLightVisible.GetValue();
		projLightData.m_AreaLightSampling = objData.m_MRayAreaLightSampling.GetValue();

		GetLeftAndUp(objData, projLightData.m_Left, projLightData.m_Up);
		camCamera cam;
		pObject->GetLight()->OrientCamera(cam);
		cam.GetProjectionMatrix(projLightData.m_ProjMatrix);
		cam.GetCameraMatrix(projLightData.m_CameraMatrix);

		// proj light texture:			
		std::string texName = objData.m_Name.GetString() + projMapEndMray;
		fsLocator texLoc;
		matTexture * currMat;
		if ( objData.m_bEnabledRamp.GetValue() )
		{	
			texLoc = o_GlobalData.m_TexturesLoc;
			std::string texNameWithExt = texName + ".png";
			texLoc.Push( texNameWithExt.c_str() );
			currMat = pObject->GetRampTexture();
			//matTextureMgr::SaveTextureToFile( currMat , texLoc );
		}
		else
		{
			texLoc = objData.m_TextureFilename.GetValue();
			currMat = pObject->GetTexture();
		}

		// only add texture if texture exists(?)
		mrayMgr::InsertPotentialTexture( texName, currMat , texLoc, o_GlobalData, objData.m_bEnabledRamp.GetValue() );
		if ( currMat )	projLightData.m_TextureName = texName;
		projLightData.m_bAffectsDiffuse = objData.m_bDiffuseEnabled.GetValue();
		projLightData.m_bAffectsSpecular = objData.m_bSpecularEnabled.GetValue();

		o_GlobalData.m_ProjLightInstances.push_back( instanceName );

		o_GlobalData.m_SceneLights.push_back( pObject->GetPropertyName().GetValue().GetString() );
		
		o_SceneData.m_ProjLightData.push_back( projLightData );
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void prjltMRayExportInterest::Export( mrayExporter& i_Exporter,
									  const mraySceneData &i_SceneData)
{
	const int num_objects = i_SceneData.m_ProjLightData.size();
	for (int i=0; i<num_objects; ++i)
	{
		// Export root node
		i_Exporter.ExportProjLight(i_SceneData.m_ProjLightData[i]);
	}
}

