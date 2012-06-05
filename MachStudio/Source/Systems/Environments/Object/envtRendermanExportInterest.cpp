/****************************************************************************\
**	envtRendermanExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Object/envtRendermanExportInterest.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Graphics/smdl/private/smdlSubdivSurface.hpp"

#include "Support/evmt/evmtEnvironment.hpp"
#include "Systems/Environments/Data/envtData.hpp"
#include "Systems/Environments/Object/envtDefaultEnvironment.hpp"
#include "Systems/Environments/Object/envtObjectMgr.hpp"

#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Support/rman/rmanExporter.hpp"
#include "Support/rman/rmanMgr.hpp"

#include <algorithm>


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* envtRendermanExportInterest::GetChunkDesc() const
{
	return "EnvLights";
}


//--------------------------------------------------------------------
// Gather data for scene that will be exported
//--------------------------------------------------------------------
void envtRendermanExportInterest::GatherSceneData( rmanSceneData &o_SceneData, rmanGlobalData &o_GlobalData ) const
{
	// Default environemnt
	const int num_objects = envtObjectMgr::GetNumObjects();
	envtDefaultEnvironment* defaultEnv = envtObjectMgr::GetDefaultEnvironment();
	envtData defaultData = defaultEnv->GetData();
	evmtEnvironment* env = defaultEnv->GetEnvironment();

	fsLocator diffTexLoc;
	matTexture * currDiffMat = NULL;
	bool isRamp = false;

	if ( defaultEnv->GetEnabledRamp() )
	{	
		diffTexLoc = o_GlobalData.m_TexturesLoc;
		std::string texNameWithExt = defaultData.m_Name.GetValue().GetString() + envDiffMapEnd + "_RAMP.tif";
		diffTexLoc.Push( texNameWithExt.c_str() );
		currDiffMat = env->m_pRampTexture;
		isRamp = true;
	}
	else
	{
		diffTexLoc = defaultData.m_DiffuseMapName.GetValue();
		currDiffMat = env->m_DiffuseMap;
	}

	rmanMgr::InsertPotentialTexture( defaultData.m_Name.GetValue().GetString() + envDiffMapEnd, currDiffMat,
									 diffTexLoc, o_GlobalData, isRamp );
	rmanMgr::InsertPotentialTexture( defaultData.m_Name.GetValue().GetString() + envSpecMapEnd, env->m_SpecularMap,
									 defaultData.m_SpecularMapName.GetValue(), o_GlobalData, isRamp );
	
	// All additional environments
	for (int i=0; i<num_objects; ++i)
	{
		envtScriptObject *pScriptObject = envtObjectMgr::GetObject(i);
		envtEnvironmentObject *pObject = pScriptObject->GetPickObject();
		envtData currData = pObject->GetData();		
		evmtEnvironment* env = pObject->GetEnvironment();

		fsLocator diffTexLoc;
		matTexture * currDiffMat = NULL;
		bool isRamp = false;

		if ( pObject->GetEnabledRamp() )
		{	
			diffTexLoc = o_GlobalData.m_TexturesLoc;
			std::string texNameWithExt = currData.m_Name.GetValue().GetString() + envDiffMapEnd + "_RAMP.tif";
			diffTexLoc.Push( texNameWithExt.c_str() );
			currDiffMat = env->m_pRampTexture;
			isRamp = true;
		}
		else
		{
			diffTexLoc = currData.m_DiffuseMapName.GetValue();
			currDiffMat = env->m_DiffuseMap;
		}

		rmanMgr::InsertPotentialTexture( currData.m_Name.GetValue().GetString() + envDiffMapEnd, currDiffMat,
										 diffTexLoc, o_GlobalData, isRamp );
		rmanMgr::InsertPotentialTexture( currData.m_Name.GetValue().GetString() + envSpecMapEnd, env->m_SpecularMap,
										 currData.m_SpecularMapName.GetValue(), o_GlobalData, isRamp );
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void envtRendermanExportInterest::Export( rmanExporter& i_Exporter,
										  const rmanSceneData &i_SceneData)
{

}
