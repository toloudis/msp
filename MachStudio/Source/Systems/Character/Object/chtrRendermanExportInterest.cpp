/****************************************************************************\
**	chtrRendermanExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Object/chtrRendermanExportInterest.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include "Graphics/ent/entModelInstance.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Graphics/smdl/private/smdlMeshSkinGroup.hpp"
#include "Graphics/smdl/private/smdlSubdivSurface.hpp"

#include "Systems/Character/Object/chtrObjectMgr.hpp"

#include "Tool/api3d/api3dObjectEntity.hpp"

#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#include "Support/rman/rmanExporter.hpp"
#include "Support/rman/rmanMgr.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"

#include <algorithm>


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* chtrRendermanExportInterest::GetChunkDesc() const
{
	return "Geometry";
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void GatherTextures(std::string i_Name, const g3dSceneNode * i_pNode, g3dAmbientEnvState * i_pAmbient, rmanGlobalData &o_GlobalData)
{
	g3dFragment * currFrag = (g3dFragment*)i_pNode->GetFragment();

	if ( currFrag && !currFrag->GetFragmentName().empty() && !currFrag->IsShadowHull() )
	{
		std::vector<effParamTexture*> textures;
		const matMaterial* mat = currFrag->GetMaterial();
		mat->GetMaterialLayer(0)->GetShaderParams()->GetAllTextureParams(textures);
	
		std::string fragName = currFrag->GetFragmentName();
		std::string matName = currFrag->GetMaterial()->GetName();
		std::string objFragMatName = i_Name + "-" + fragName + "-" + matName;

		// add one by one to the big list, accounting for shared textures.
		for (int k = 0; k < textures.size(); k++)
		{
			effParamTexture* currParam = textures[k];
			std::string paramName = currParam->GetName();			
			std::string texName = objFragMatName + "-" + paramName;
			
			fsLocator texLoc = fsLocator();
			matTexture * currMat;
			bool isRamp = false;

			if ( currParam->GetRampTexture() )
			{	
				texLoc = o_GlobalData.m_TexturesLoc;
				std::string texNameWithExt = texName + "_RAMP.tif";
				texLoc.Push( texNameWithExt.c_str() );
				currMat = currParam->GetRampTexture();
				isRamp = true;
			}
			else
			{
				texLoc = currParam->Property().GetValue();
				currMat = currParam->GetTexture();
			}

			rmanMgr::InsertPotentialTexture( texName, currMat , texLoc, o_GlobalData, isRamp );
		}

		rmanMgr::InsertPotentialTexture( objFragMatName + dispMapEnd , mat->GetDisplacementData().m_pDisplacementMap,
										 mat->GetDisplacementData().m_NameDisplacementMap, o_GlobalData, false );
		rmanMgr::InsertPotentialTexture( objFragMatName + normMapEnd , mat->GetNormalsData().m_pNormalMap,
										 mat->GetNormalsData().m_NameNormalMap, o_GlobalData, false );

	}

	const int num_kids = i_pNode->GetNumChildren();
	for (int i=0; i<num_kids; i++)
	{
		GatherTextures(i_Name,i_pNode->GetChild(i),i_pAmbient,o_GlobalData);
	}

}

//--------------------------------------------------------------------
// Gather data for scene that will be exported
//--------------------------------------------------------------------
void chtrRendermanExportInterest::GatherSceneData( rmanSceneData &o_SceneData, rmanGlobalData &o_GlobalData ) const
{
	// put the names of the objects in the list
	const int num_objects = chtrObjectMgr::GetNumObjects();

	for (int i=0; i<num_objects; ++i)
	{
		chtrScriptObject *pScriptObject = chtrObjectMgr::GetObject(i);
		chtrObject *pObject = pScriptObject->GetPickObject();
		chtrData objData = pObject->GetData();

		int numSubdivs = 0;
		int numMeshGroups = 0;
		//g3dSceneNode * node;
		rmanGeometryData geometryData;

		if ( pObject->GetSubdivCharacter() )
		{
			numSubdivs = pObject->GetSubdivCharacter()->GetNumSubdivSurfaces();
			for ( int j = 0 ; j < numSubdivs ; j++ )
			{
				smdlSubdivSurface* subdivsurface = dynamic_cast<smdlSubdivSurface*>( pObject->GetSubdivCharacter()->GetSubdivSurfaces()[j] );
				geometryData.m_Name = objData.m_Name.GetValue().GetString();
				geometryData.m_SubName = subdivsurface->GetSubdivName();
				geometryData.m_bVisible = objData.m_bVisible.GetValue() & rlyrRenderLayerMgr::GetSingleObjectVisibility( objData.m_Name.GetValue().GetString() );
				geometryData.m_AmbientData = pObject->GetEntity()->GetAmbientState();	
				geometryData.m_NumSubdivs = numSubdivs;
				geometryData.m_pNode = subdivsurface->RootNode();
			
				GatherTextures(objData.m_Name.GetValue().GetString(), subdivsurface->RootNode(), pObject->GetEntity()->GetAmbientState(), o_GlobalData);

				o_SceneData.m_GeometryData.push_back( geometryData );
			}

			numMeshGroups = pObject->GetSubdivCharacter()->GetNumMeshGroups();
			for ( int j = 0 ; j < numMeshGroups ; j++ )
			{
				smdlMeshSkinGroup * meshGroup = pObject->GetSubdivCharacter()->GetMeshGroups()[j];
				geometryData.m_Name = objData.m_Name.GetValue().GetString();
				geometryData.m_SubName = meshGroup->GetMeshGroupName();
				geometryData.m_bVisible = objData.m_bVisible.GetValue() & rlyrRenderLayerMgr::GetSingleObjectVisibility( objData.m_Name.GetValue().GetString() );
				geometryData.m_AmbientData = pObject->GetEntity()->GetAmbientState();	
				geometryData.m_NumSubdivs = 0;
				geometryData.m_pNode = meshGroup->GetNode();
			
				GatherTextures(objData.m_Name.GetValue().GetString(),meshGroup->GetNode(), pObject->GetEntity()->GetAmbientState(), o_GlobalData);

				o_SceneData.m_GeometryData.push_back( geometryData );
			}

		} 
		
		if ( pObject->GetEntity()->GetAmbientState() && pObject->GetEntity()->GetObject()->GetBase() )
		{
			geometryData.m_Name = objData.m_Name.GetValue().GetString();
			geometryData.m_SubName = "";
			geometryData.m_bVisible = objData.m_bVisible.GetValue() & rlyrRenderLayerMgr::GetSingleObjectVisibility( objData.m_Name.GetValue().GetString() );
			geometryData.m_AmbientData = pObject->GetEntity()->GetAmbientState();
			geometryData.m_NumSubdivs = 0;
			geometryData.m_pNode = pObject->GetEntity()->GetObject()->GetBase();
			
			GatherTextures(objData.m_Name.GetValue().GetString(), pObject->GetEntity()->GetObject()->GetBase(), 
						   pObject->GetEntity()->GetAmbientState(), o_GlobalData);

			o_SceneData.m_GeometryData.push_back( geometryData );
		}

	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void chtrRendermanExportInterest::Export( rmanExporter& i_Exporter,
										  const rmanSceneData &i_SceneData)
{

	const int num_objects = i_SceneData.m_GeometryData.size();
	for (int i=0; i<num_objects; ++i)
	{
		// Export root node
		i_Exporter.ExportMesh(i_SceneData.m_GeometryData[i]);
	}
}
