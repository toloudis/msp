/****************************************************************************\
**	chtrMRayExportInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Object/chtrMRayExportInterest.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include "Graphics/ent/entModelInstance.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/Mat/matMetaFX.hpp"
#include "Graphics/Mat/matMetaFXParser.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Graphics/smdl/private/smdlMeshSkinGroup.hpp"
#include "Graphics/smdl/private/smdlSubdivSurface.hpp"

#include "Systems/Character/Object/chtrObjectMgr.hpp"

#include "Tool/api3d/api3dObjectEntity.hpp"

#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#include "ImportExport/mray/export/private/mrayExportUtil.hpp"
#include "ImportExport/mray/export/mrayExport.hpp"
#include "Support/mray/mrayExporter.hpp"
#include "Support/mray/mrayMgr.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"

#include <algorithm>


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
const char* chtrMRayExportInterest::GetChunkDesc() const
{
	return "Geometry";
}

//--------------------------------------------------------------------
// return true if the shader is metaSL. 
// This is important for passing parameters to the shader.
//--------------------------------------------------------------------
bool ExportShader(const matMaterial* i_Material, 
				  mrayGlobalData& o_GlobalData)
{
	// Get shader info
	fsLocator shaderLoc = i_Material->GetMaterialLayer(0)->GetShaderParams()->GetShaderName();
	itString shaderNameIt = shaderLoc.GetLastName();
	itString ext;
	shaderNameIt.GetExtension(ext);
	itStringUtil::ToLower(ext);
	bool bUseMetaSL = false;
	if (ext == itString("mfx"))
	{
		// Get the shader name
		std::string shaderFileName = itStringUtil::GetStdString(shaderNameIt);

		shaderLoc = matShaderMgr::ResolveShaderPath(shaderLoc);

		// Load shader from multiple format effect file (MFX)
		matMetaFX shader_data;
		matMetaFXParser::ReadMetaFX( shaderLoc, shader_data );

		// If we have metaSL data (have to check for correct version),
		// then load that data as the shader.
		DBG_ASSERT(shader_data.m_MetaSLSource.m_bHasData, "MFX shader without metaSL source!");
		bUseMetaSL = (shader_data.m_MetaSLSource.m_bHasData &&
			matMetaFX::CompareVersion(shader_data.m_MetaSLSource.m_Version, matMetaFX::GetCurrentMetaSLVersion()));

		if (bUseMetaSL)
		{
			shaderNameIt.StripExtension();
			mrayMgr::AddMetaSLShader(o_GlobalData, shaderNameIt, shaderLoc);
		}
	}
	return bUseMetaSL;
}

//--------------------------------------------------------------------
// GatherTextures()
//--------------------------------------------------------------------
void GatherTextures(std::string i_Name, const g3dSceneNode * i_pNode, g3dAmbientEnvState * i_pAmbient, 
					mrayGlobalData& o_GlobalData)
{
	g3dFragment * currFrag = (g3dFragment*)i_pNode->GetFragment();

	if ( currFrag && !currFrag->GetFragmentName().empty() && !currFrag->IsShadowHull())
	{
		std::vector<effParamTexture*> textures;
		const matMaterial* mat = currFrag->GetMaterial();

		bool isMetaSL = ExportShader(mat, o_GlobalData);

		mat->GetMaterialLayer(0)->GetShaderParams()->GetAllTextureParams(textures);
	
		std::string fragName = currFrag->GetFragmentName();

		std::string matName = currFrag->GetMaterial()->GetName();
		std::string objFragMatName = i_Name + "-" + fragName + "-" + matName;
		
		itString shaderNameIt = currFrag->GetMaterial()->GetMaterialLayer(0)->GetShaderParams()->GetShaderName().GetLastName();
		shaderNameIt.StripExtension();

		// add one by one to the big list, accounting for shared textures.
		for (int k = 0; k < textures.size(); k++)
		{
			effParamTexture* currParam = textures[k];
			std::string paramName = currParam->GetName();			
			std::string texName = mrayExport::GenerateTextureParamName(objFragMatName, paramName, isMetaSL);

			if ( mrayExportUtil::IsDeadParam(paramName,itStringUtil::GetStdString(shaderNameIt)) ) continue;
			
			fsLocator texLoc;
			matTexture * currMat;

			if ( currParam->GetRampTexture() )
			{	
				texLoc = o_GlobalData.m_TexturesLoc;
				std::string texNameWithExt = texName + ".png";
				texLoc.Push( texNameWithExt.c_str() );
				currMat = currParam->GetRampTexture();
				mrayMgr::InsertPotentialTexture( texName, currMat , texLoc, o_GlobalData, true );
			}
			else
			{	
				texLoc = currParam->Property().GetValue();
				currMat = currParam->GetTexture();
				mrayMgr::InsertPotentialTexture( texName, currMat , texLoc, o_GlobalData, (paramName == "microTex" || paramName == "reflectSmearMap") );
			}

		}

		if ( mat->GetDisplacementData().m_pDisplacementMap )
		{
			mrayMgr::InsertPotentialTexture( objFragMatName + dispMapEndMray , mat->GetDisplacementData().m_pDisplacementMap, 
											 mat->GetDisplacementData().m_NameDisplacementMap, o_GlobalData, false );				
		}

		if ( mat->GetNormalsData().m_pNormalMap )
		{
			mrayMgr::InsertPotentialTexture( objFragMatName + normMapEndMray , mat->GetNormalsData().m_pNormalMap, 
											 mat->GetNormalsData().m_NameNormalMap, o_GlobalData, true );
		}
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
void chtrMRayExportInterest::GatherSceneData( mraySceneData &o_SceneData , mrayGlobalData &o_GlobalData ) const
{
	// put the names of the objects in the list
	const int num_objects = chtrObjectMgr::GetNumObjects();

	for (int i=0; i<num_objects; ++i)
	{
		chtrScriptObject *pScriptObject = chtrObjectMgr::GetObject(i); // cast it into fragment
		chtrObject *pObject = pScriptObject->GetPickObject();
		chtrData objData = pObject->GetData();

		int numSubdivs = 0;
		int numMeshGroups = 0;
		mrayGeometryData geometryData;

		if ( pObject->GetSubdivCharacter() )
		{
			numSubdivs = pObject->GetSubdivCharacter()->GetNumSubdivSurfaces();
			for ( int j = 0 ; j < numSubdivs ; j++ )
			{
				smdlSubdivSurface* subdivsurface = dynamic_cast<smdlSubdivSurface*>( pObject->GetSubdivCharacter()->GetSubdivSurfaces()[j] );

				geometryData.m_Name = objData.m_Name.GetValue().GetString();
				geometryData.m_SubName = subdivsurface->GetSubdivName();
				geometryData.m_bVisible = objData.m_bVisible.GetValue() & rlyrRenderLayerMgr::GetSingleObjectVisibility( objData.m_Name.GetValue().GetString() );
				geometryData.m_NumSubdivs = numSubdivs;
				geometryData.m_NumMeshGroups = 0;
				geometryData.m_Node = subdivsurface->RootNode();
				geometryData.m_EnvData = pObject->GetEntity()->GetAmbientState();

				GatherTextures(objData.m_Name.GetValue().GetString(), subdivsurface->RootNode(), pObject->GetEntity()->GetAmbientState(),
							   o_GlobalData);

				o_SceneData.m_GeometryData.push_back( geometryData );
			}

			numMeshGroups = pObject->GetSubdivCharacter()->GetNumMeshGroups();
			for ( int j = 0 ; j < numMeshGroups ; j++ )
			{
				smdlMeshSkinGroup * meshGroup = pObject->GetSubdivCharacter()->GetMeshGroups()[j];

				geometryData.m_Name = objData.m_Name.GetValue().GetString();
				geometryData.m_SubName = meshGroup->GetMeshGroupName();
				geometryData.m_bVisible = objData.m_bVisible.GetValue() & rlyrRenderLayerMgr::GetSingleObjectVisibility( objData.m_Name.GetValue().GetString() );
				geometryData.m_NumSubdivs = 0;
				geometryData.m_NumMeshGroups = numMeshGroups;
				geometryData.m_Node = meshGroup->GetNode();
				geometryData.m_EnvData = pObject->GetEntity()->GetAmbientState();

				GatherTextures(objData.m_Name.GetValue().GetString(), meshGroup->GetNode(), pObject->GetEntity()->GetAmbientState(), 
							   o_GlobalData);

				o_SceneData.m_GeometryData.push_back( geometryData );
			}

		} 
		
		if ( pObject->GetEntity()->GetAmbientState() && pObject->GetEntity()->GetObject()->GetBase() )
		{

			geometryData.m_Name = objData.m_Name.GetValue().GetString();
			geometryData.m_SubName = "";
			geometryData.m_bVisible = objData.m_bVisible.GetValue() & rlyrRenderLayerMgr::GetSingleObjectVisibility( objData.m_Name.GetValue().GetString() );
			geometryData.m_NumSubdivs = 0;
			geometryData.m_NumMeshGroups = 0;
			geometryData.m_Node = pObject->GetEntity()->GetObject()->GetBase();
			geometryData.m_EnvData = pObject->GetEntity()->GetAmbientState();			

			GatherTextures(objData.m_Name.GetValue().GetString(), pObject->GetEntity()->GetObject()->GetBase(), pObject->GetEntity()->GetAmbientState(), 
						   o_GlobalData);

			o_SceneData.m_GeometryData.push_back( geometryData );
		}
	}
}

//--------------------------------------------------------------------
//	Export only the items in the given list.
//--------------------------------------------------------------------
void chtrMRayExportInterest::Export( mrayExporter& i_Exporter, const mraySceneData &i_SceneData )
{
	const int num_objects = i_SceneData.m_GeometryData.size();
	for (int i=0; i<num_objects; ++i)
	{
		// Export root node
		i_Exporter.ExportMesh(i_SceneData.m_GeometryData[i]);
	}
}
