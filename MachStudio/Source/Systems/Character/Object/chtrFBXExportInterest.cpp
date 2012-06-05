/****************************************************************************\
**	chtrFBXExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Object/chtrFBXExportInterest.hpp"

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
#include "Support/fbx/fbxExporter.hpp"
#include "Support/fbx/fbxMgr.hpp"

#include <algorithm>


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* chtrFBXExportInterest::GetChunkDesc() const
{
	return "Geometry";
}

//--------------------------------------------------------------------
// TestNode()
//--------------------------------------------------------------------
bool TestNode( g3dSceneNode * i_pTestNode , g3dSceneNode * i_pTreeNode )
{
	if ( i_pTestNode == i_pTreeNode ) return true;

	const int num_kids = i_pTreeNode->GetNumChildren();
	for ( int k = 0 ; k < num_kids ; k++ )
	{
		TestNode(i_pTestNode, i_pTreeNode->GetChild(k));
	}
	return false;
}

//--------------------------------------------------------------------
// GetFragInfoIdx()
//--------------------------------------------------------------------
int GetFragInfoIdx( fgmtScriptObject* i_fragScriptObject , g3dSceneNode * i_pNode )
{
	std::vector< std::vector<g3dSceneNode*> > nodes = i_fragScriptObject->GetFragmentNodes();
	for ( int i = 0 ; i < nodes.size() ; i++ )
	{
		for ( int j = 0 ; j < nodes[i].size() ; j++ )
		{
			bool foundNode = TestNode( nodes[i][j] , i_pNode );
			if ( foundNode ) return i;
		}
	}
	return -1;
}

//--------------------------------------------------------------------
// Gather data for scene that will be exported
//--------------------------------------------------------------------
void chtrFBXExportInterest::GatherSceneData( fbxSceneData &o_SceneData , bool &o_SceneHasBeenBaked ) const
{
	// put the names of the objects in the list
	const int num_objects = chtrObjectMgr::GetNumObjects();
	o_SceneHasBeenBaked = false;

	for (int i=0; i<num_objects; ++i)
	{
		chtrScriptObject *pScriptObject = chtrObjectMgr::GetObject(i); // cast it into fragment
		chtrObject *pObject = pScriptObject->GetPickObject();
		chtrData objData = pObject->GetData();

		fgmtScriptObject* fragScriptObject = dynamic_cast<fgmtScriptObject*>(pScriptObject);

		if ( fbxMgr::GetExportSelected() && !sel3dMgr::IsSelected(pObject) ) continue;

		int numSubdivs = 0;
		int numMeshGroups = 0;
		fbxGeometryData geometryData;

		if ( pObject->GetSubdivCharacter() )
		{
			numSubdivs = pObject->GetSubdivCharacter()->GetNumSubdivSurfaces();
			for ( int j = 0 ; j < numSubdivs ; j++ )
			{
				smdlSubdivSurface* subdivsurface = dynamic_cast<smdlSubdivSurface*>( pObject->GetSubdivCharacter()->GetSubdivSurfaces()[j] );				
				geometryData.m_Name = objData.m_Name.GetValue().GetString();
				geometryData.m_SubName = subdivsurface->GetSubdivName();
				geometryData.m_NumSubdivs = numSubdivs;
				geometryData.m_NumMeshGroups = 0;
				geometryData.m_Node = subdivsurface->RootNode();

				int node_idx = GetFragInfoIdx(fragScriptObject,subdivsurface->RootNode());
				if ( node_idx != -1 )
				{
					fgmtFragmentData fgmtData = fragScriptObject->GetFragmentDataElement(node_idx);
					if ( fgmtData.m_bHasBeenBaked.GetValue() == true ) o_SceneHasBeenBaked = true;
					geometryData.m_bHasBeenBaked = fgmtData.m_bHasBeenBaked.GetValue();
					geometryData.m_BakedPath = fgmtData.m_BakedTextureLocation.GetValue();
					geometryData.m_BakeExt = fgmtData.m_BakedTextureFormat.GetValue();
				}
				else
				{
					geometryData.m_bHasBeenBaked = false;
					geometryData.m_BakedPath = fsLocator();
					geometryData.m_BakeExt = "";
				}
				o_SceneData.m_GeometryData.push_back( geometryData );

			}

			numMeshGroups = pObject->GetSubdivCharacter()->GetNumMeshGroups();
			for ( int j = 0 ; j < numMeshGroups ; j++ )
			{
				smdlMeshSkinGroup * meshGroup = pObject->GetSubdivCharacter()->GetMeshGroups()[j];
				geometryData.m_Name = objData.m_Name.GetValue().GetString();
				geometryData.m_SubName = meshGroup->GetMeshGroupName();
				geometryData.m_NumSubdivs = 0;
				geometryData.m_NumMeshGroups = numMeshGroups;
				geometryData.m_Node = meshGroup->GetNode();

				int node_idx = GetFragInfoIdx(fragScriptObject,meshGroup->GetNode());
				if ( node_idx != -1 )
				{
					fgmtFragmentData fgmtData = fragScriptObject->GetFragmentDataElement(node_idx);
					if ( fgmtData.m_bHasBeenBaked.GetValue() == true ) o_SceneHasBeenBaked = true;
					geometryData.m_bHasBeenBaked = fgmtData.m_bHasBeenBaked.GetValue();
					geometryData.m_BakedPath = fgmtData.m_BakedTextureLocation.GetValue();
					geometryData.m_BakeExt = fgmtData.m_BakedTextureFormat.GetValue();
				}			
				else
				{
					geometryData.m_bHasBeenBaked = false;
					geometryData.m_BakedPath = fsLocator();
					geometryData.m_BakeExt = "";
				}

				o_SceneData.m_GeometryData.push_back( geometryData );
			}

		} 
		
		if ( numSubdivs == 0 && numMeshGroups == 0 )
		{
			geometryData.m_Name = objData.m_Name.GetValue().GetString();
			geometryData.m_SubName = "";
			geometryData.m_NumSubdivs = 0;
			geometryData.m_NumMeshGroups = 0;
			geometryData.m_Node = pObject->GetEntity()->GetObject()->GetBase();

			int node_idx = GetFragInfoIdx(fragScriptObject,(g3dSceneNode*)pObject->GetEntity()->GetObject()->GetBase());
			if ( node_idx != -1 )
			{
				fgmtFragmentData fgmtData = fragScriptObject->GetFragmentDataElement(node_idx);
				if ( fgmtData.m_bHasBeenBaked.GetValue() == true ) o_SceneHasBeenBaked = true;
				geometryData.m_bHasBeenBaked = fgmtData.m_bHasBeenBaked.GetValue();
				geometryData.m_BakedPath = fgmtData.m_BakedTextureLocation.GetValue();
				geometryData.m_BakeExt = fgmtData.m_BakedTextureFormat.GetValue();
			}
			else
			{
				geometryData.m_bHasBeenBaked = false;
				geometryData.m_BakedPath = fsLocator();
				geometryData.m_BakeExt = "";
			}

			o_SceneData.m_GeometryData.push_back( geometryData );
		}
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//--------------------------------------------------------------------
void chtrFBXExportInterest::Export( fbxExporter& i_Exporter, const fbxSceneData &i_SceneData )
{
	const int num_objects = i_SceneData.m_GeometryData.size();
	for (int i=0; i<num_objects; ++i)
	{
		// Export root node
		i_Exporter.ExportMesh(i_SceneData.m_GeometryData[i]);
	}
}
