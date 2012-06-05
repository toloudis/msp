/****************************************************************************\
**	lsetMRayExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Object/lsetMRayExportInterest.hpp"

#include "Support/mray/mrayExporter.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include "Graphics/ent/entModelInstance.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Graphics/smdl/private/smdlMeshSkinGroup.hpp"
#include "Graphics/smdl/private/smdlSubdivSurface.hpp"

#include "Systems/LightSets/Object/lsetObjectMgr.hpp"

#include "Tool/api3d/api3dObjectEntity.hpp"

#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#include "Support/ltst/ltstLightSet.hpp"
#include "Support/ltst/private/ltstLightSetLight.hpp"
#include "Support/ltst/private/ltstLightSetNode.hpp"
#include "Support/ltst/private/ltstLightSetObject.hpp"

#include <map>


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* lsetMRayExportInterest::GetChunkDesc() const
{
	return "LightSets";
}

//--------------------------------------------------------------------
// Gather data for scene that will be exported
//--------------------------------------------------------------------
void lsetMRayExportInterest::GatherSceneData( mraySceneData &o_SceneData , mrayGlobalData &o_GlobalData ) const
{
	// put the names of the objects in the list
	const int num_objects = lsetObjectMgr::GetNumObjects();	

	for ( int i = 0 ; i < num_objects ; i++ )
	{
		lsetScriptObject *pScriptObject = lsetObjectMgr::GetObject(i);
		lsetLightSetObject *pObject = pScriptObject->GetPickObject();
		lsetData data = pObject->GetData();

		mrayLightSetData lightSetData;

		std::vector<std::string> lightVec;
		std::vector<std::string> objVec;

		ltstLightSet* lightSet = pObject->GetLightSet();
		std::vector<ltstLightSetLight*> lightSetLights = lightSet->GetLights();

		for ( int j = 0 ; j < lightSetLights.size() ; j++ )
		{
			//lightVec.push_back( lightSetLights[j]->m_pNameObj->GetName().GetString() );
			lightSetData.m_Lights.push_back( lightSetLights[j]->m_pNameObj->GetName().GetString() );
		}

		std::map<ltstLightSetObject*, sObjectNodeInfo> lightSetObjectMap = lightSet->GetObjects();
		for(std::map<ltstLightSetObject*, sObjectNodeInfo>::const_iterator it = lightSetObjectMap.begin(); it != lightSetObjectMap.end(); ++it)
		{
			ltstLightSetObject * base_object = it->first;
			sObjectNodeInfo node_info = it->second;
			std::string base_name = base_object->m_pNameObj->GetName().GetString();

			if ( node_info.m_bRootLit )
			{
				std::vector<ltstLightSetNode*> lightSetNodes = base_object->GetNodes();
				for ( int k = 0 ; k < lightSetNodes.size() ; k++ )
				{
					std::string nodeName = lightSetNodes[k]->GetNodeName();
					std::string fragName = nodeName.substr(0,nodeName.find(" : "));
					std::string matName = nodeName.substr(nodeName.find(" : ")+3,nodeName.length()-1);	
					lightSetData.m_Objects.push_back( base_name + "-" + fragName + "-" + matName );
				}
			}
			else
			{				
				for ( int k = 0 ; k < node_info.m_bNodesLit.size() ; k++ )
				{
					if ( node_info.m_bNodesLit[k] == true )
					{
						std::string nodeName = base_object->GetNodes()[k]->GetNodeName();
						std::string fragName = nodeName.substr(0,nodeName.find(" : "));
						std::string matName = nodeName.substr(nodeName.find(" : ")+3,nodeName.length()-1);	
						lightSetData.m_Objects.push_back( base_name + "-" + fragName + "-" + matName );
					}
				}
			}
		}

		o_SceneData.m_LightSetData.push_back( lightSetData );
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void lsetMRayExportInterest::Export( mrayExporter& i_Exporter,
									 const mraySceneData &i_SceneData)
{
	// put the names of the objects in the list
	const int num_lightSets = lsetObjectMgr::GetNumObjects();	

	// Gather all light set objects
	std::vector< std::string > all_lights;
	for ( int i = 0 ; i < num_lightSets ; i++ )
	{
		std::vector< std::string > currLights = i_SceneData.m_LightSetData[i].m_Lights;
		for ( int j = 0 ; j < currLights.size() ; j++ )
		{
			if ( !envSTLHelpers::Contains(all_lights, currLights[j]) )
			{
				all_lights.push_back( currLights[j] );
			}
		}
	}

	// Gather all objects
	std::vector< std::string > all_objects;
	for ( int i = 0 ; i < num_lightSets ; i++ )
	{
		std::vector< std::string > currObjects = i_SceneData.m_LightSetData[i].m_Objects;
		for ( int j = 0 ; j < currObjects.size() ; j++ )
		{
			if ( !envSTLHelpers::Contains(all_objects, currObjects[j]) )
			{
				all_objects.push_back( currObjects[j] );
			}
		}
	}

	// Make map with each key as object name. Values are a vector of lights lit by it.
	std::map< std::string , std::vector< std::string > > objectLightMap;
	for ( int i = 0 ; i < all_objects.size() ; i++ )	
	{		
		std::vector< std::string > lightSetLights;
		for ( int j = 0 ; j < num_lightSets ; j++ )
		{
			std::vector< std::string > currObjects = i_SceneData.m_LightSetData[j].m_Objects;
			std::vector< std::string > currLights = i_SceneData.m_LightSetData[j].m_Lights;

			for ( int k = 0 ; k < currObjects.size() ; k++ )
			{
				if ( all_objects[i] == currObjects[k] )
				{					
					for ( int l = 0 ; l < currLights.size() ; l++ )
					{
						lightSetLights.push_back( currLights[l] );
					}
				}

			}
			
		}
		objectLightMap.insert( std::pair<std::string,std::vector< std::string >>(all_objects[i],lightSetLights) );
	}

	i_Exporter.UpdateGlobalLightSetLights( all_lights );
	i_Exporter.UpdateGlobalObjectLightMap( objectLightMap );
	//i_Exporter.GetGlobalData().m_ObjectLightMap = objectLightMap;
	//i_Exporter.GetGlobalData().m_LightSetLights = all_lights;

}
