/****************************************************************************\
**	envtMRayExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Object/envtMRayExportInterest.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include "Graphics/mat/matTextureMgr.hpp"

#include "Systems/Environments/Data/envtData.hpp"
#include "Systems/Environments/Object/envtDefaultEnvironment.hpp"
#include "Systems/Environments/Object/envtObjectMgr.hpp"
#include "Systems/Environments/Object/envtSwlEnvironment.hpp"

#include "Support/evmt/evmtEnvironment.hpp"
#include "Support/mray/mrayExporter.hpp"
#include "Support/mray/mrayMgr.hpp"

#include <algorithm>


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* envtMRayExportInterest::GetChunkDesc() const
{
	return "EnvLights";
}

//--------------------------------------------------------------------
// Gather data for scene that will be exported
//--------------------------------------------------------------------
void envtMRayExportInterest::GatherSceneData( mraySceneData &o_SceneData , mrayGlobalData &o_GlobalData ) const
{

	// Default environemnt
	const int num_objects = envtObjectMgr::GetNumObjects();
	envtDefaultEnvironment* defaultEnv = envtObjectMgr::GetDefaultEnvironment();
	envtSwlEnvironment* swlEnv = envtObjectMgr::GetSwlEnvironment();
	envtData defaultData = defaultEnv->GetData();
	envtData swlData = swlEnv->GetData();
	evmtEnvironment* env = defaultEnv->GetEnvironment();
	evmtEnvironment* swlenv = swlEnv->GetEnvironment();

	// Gather swl environment light data
	mrayEnvData data;
	data.diffcolor = swlData.m_DiffuseColor.GetValue();
	data.diffFactor = swlData.m_DiffuseFactor.GetValue();
	data.diffAngle = swlData.m_DiffuseAngle.GetValue();
	data.speccolor = swlData.m_SpecularColor.GetValue();
	data.specFactor = swlData.m_SpecularFactor.GetValue();
	data.specAngle = swlData.m_SpecularAngle.GetValue();
	data.diffTex = swlData.m_Name.GetValue().GetString() + envDiffMapEndMray;
	data.specTex = swlData.m_Name.GetValue().GetString() + envSpecMapEndMray;
	o_GlobalData.m_EnvData = data;
	o_GlobalData.m_Options.m_bEnableIBL = swlEnv->GetPropertyEnable().GetValue();

	fsLocator diffTexLoc;
	matTexture * currDiffMat = NULL;

	// add default environment texture
	if ( defaultEnv->GetEnabledRamp() )
	{	
		diffTexLoc = o_GlobalData.m_TexturesLoc;
		std::string texNameWithExt = defaultData.m_Name.GetValue().GetString() + envDiffMapEndMray + ".png";
		diffTexLoc.Push( texNameWithExt.c_str() );
		currDiffMat = env->m_pRampTexture;
		//matTextureMgr::SaveTextureToFile( currDiffMat , diffTexLoc );
	}
	else
	{
		diffTexLoc = defaultData.m_DiffuseMapName.GetValue();
		currDiffMat = env->m_DiffuseMap;
	}

	mrayMgr::InsertPotentialTexture( defaultData.m_Name.GetValue().GetString() + envDiffMapEndMray, currDiffMat,
									 diffTexLoc, o_GlobalData, defaultEnv->GetEnabledRamp() );
	mrayMgr::InsertPotentialTexture( defaultData.m_Name.GetValue().GetString() + envSpecMapEndMray, env->m_SpecularMap,
									 defaultData.m_SpecularMapName.GetValue(), o_GlobalData, defaultEnv->GetEnabledRamp() );

	// add swl environment texture
	currDiffMat = NULL;
	if ( swlEnv->GetEnabledRamp() )
	{	
		diffTexLoc = o_GlobalData.m_TexturesLoc;
		std::string texNameWithExt = swlData.m_Name.GetValue().GetString() + envDiffMapEndMray + ".png";
		diffTexLoc.Push( texNameWithExt.c_str() );
		currDiffMat = swlenv->m_pRampTexture;
		//matTextureMgr::SaveTextureToFile( currDiffMat , diffTexLoc );
	}
	else
	{
		diffTexLoc = swlData.m_DiffuseMapName.GetValue();
		currDiffMat = swlenv->m_DiffuseMap;
	}

	mrayMgr::InsertPotentialTexture( swlData.m_Name.GetValue().GetString() + envDiffMapEndMray, currDiffMat,
									 diffTexLoc, o_GlobalData, swlEnv->GetEnabledRamp() );
	mrayMgr::InsertPotentialTexture( swlData.m_Name.GetValue().GetString() + envSpecMapEndMray, env->m_SpecularMap,
									 swlData.m_SpecularMapName.GetValue(), o_GlobalData, swlEnv->GetEnabledRamp() );

	
	// All additional environments
	for (int i=0; i<num_objects; ++i)
	{
		envtScriptObject *pScriptObject = envtObjectMgr::GetObject(i);
		envtEnvironmentObject *pObject = pScriptObject->GetPickObject();
		envtData currData = pObject->GetData();		
		evmtEnvironment* env = pObject->GetEnvironment();

		fsLocator diffTexLoc;
		matTexture * currDiffMat = NULL;

		if ( pObject->GetEnabledRamp() )
		{	
			diffTexLoc = o_GlobalData.m_TexturesLoc;
			std::string texNameWithExt = currData.m_Name.GetValue().GetString() + envDiffMapEndMray + ".png";
			diffTexLoc.Push( texNameWithExt.c_str() );
			currDiffMat = env->m_pRampTexture;
			//matTextureMgr::SaveTextureToFile( currDiffMat , diffTexLoc );
		}
		else
		{
			diffTexLoc = currData.m_DiffuseMapName.GetValue();
			currDiffMat = env->m_DiffuseMap;
		}

		mrayMgr::InsertPotentialTexture( currData.m_Name.GetValue().GetString() + envDiffMapEndMray, currDiffMat,
										 diffTexLoc, o_GlobalData, pObject->GetEnabledRamp() );
		mrayMgr::InsertPotentialTexture( currData.m_Name.GetValue().GetString() + envSpecMapEndMray, env->m_SpecularMap,
										 currData.m_SpecularMapName.GetValue(), o_GlobalData, pObject->GetEnabledRamp() );

	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void envtMRayExportInterest::Export( mrayExporter& i_Exporter, const mraySceneData &i_SceneData )
{
}
