 /*****************************************************************************
**  gltfImport.cpp
**
**      The gltfImport implements importing functionality for the
**	Autodesk FBX SDK.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/gltf/import/gltfImport.hpp"

#include "ImportExport/gltf/import/private/gltfModelImport.hpp"
#include "ImportExport/gltf/import/private/gltfSdkManager.hpp"
#include "ImportExport/gltf/import/private/gltfSceneRead.hpp"
#include "ImportExport/gltf/import/private/gltfPlacement.hpp"
#include "ImportExport/gltf/import/private/gltfAnimKeys.hpp"

#ifndef DBG_MSG_HPP
#include "Core/Dbg/dbgMsg.hpp"
#endif
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsResourceTracker.hpp"
#include "Graphics/emdl/emdlCharacterTemplate.hpp"
#ifndef EMDL_CHARACTERANIMKEYS_HPP
#include "Graphics/emdl/emdlCharacterAnimKeys.hpp"
#endif
#ifndef G3D_CONSTANTS_HPP
#include "Graphics/G3d/g3dConstants.hpp"
#endif

namespace
{

	// Make sure the scene is destroyed if exception is thrown
	struct kgltfSceneWrapper
	{
		tinygltf::Model* m_pScene;
		kgltfSceneWrapper(tinygltf::Model* i_pScene) : m_pScene(i_pScene) {}
		~kgltfSceneWrapper() 
		{  
			if (m_pScene) {
				delete m_pScene;
				m_pScene = NULL;
			}
		}
	};

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
gltfImport::gltfImport()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
gltfImport::~gltfImport()
{

}

//----------------------------------------------------------------------------
//  LoadGeometry
//----------------------------------------------------------------------------
entModelTemplate* gltfImport::LoadGeometry( const fsLocator&	i_ModelLocator,
										   //const fsResourceFinder& i_TextureFinder,
										   entFragInfoSink* o_Sink)
{

	//	What kind of model is it?
	std::string filename;
	fsFileUtil::LocatorToANSIFilename(i_ModelLocator, filename);
	if (gltfSdkManager::IsRecognizedFileFormat(filename.c_str())) 
	{
		tinygltf::Model* pScene = gltfSdkManager::LoadScene(filename.c_str());
		if (pScene)
		{
			// This will destroy the scene, including if an exception is thrown
			kgltfSceneWrapper clean_up_scene(pScene); 

			fsResourceTracker::MarkBegin(i_ModelLocator);

			//	Use character template for all generalized hierarchical models now.
			std::unique_ptr<emdlCharacterTemplate> mdl_template(new emdlCharacterTemplate());

			g3dSceneNode* pBase;
			gltfModelImport::LoadModel(	pScene,
										i_ModelLocator,
										//i_TextureFinder,
										pBase,
										//joint_roots,
										mdl_template->SkinnedSurfaces(),
										mdl_template->Fragments(),
										mdl_template->MaterialTable(),
										mdl_template->Materials());
										//mdl_template->Textures() );

			mdl_template->SetModel( pBase );
			mdl_template->SetFilename( i_ModelLocator );

			fsResourceTracker::MarkEnd(i_ModelLocator);
			return mdl_template.release();
		}
	}

	// return NULL and let next implementation try to handle it
	return NULL;
}

//----------------------------------------------------------------------------
//  LoadGeometryPart: one node's mesh
//----------------------------------------------------------------------------
entModelTemplate* gltfImport::LoadGeometryPart( const fsLocator& i_ModelLocator,
												const std::string& i_Part )
{
	std::string filename;
	fsFileUtil::LocatorToANSIFilename(i_ModelLocator, filename);
	if (!gltfSdkManager::IsRecognizedFileFormat(filename.c_str()))
		return NULL;	// let the next implementation try

	std::shared_ptr<const tinygltf::Model> pScene = gltfSdkManager::LoadSceneShared(filename.c_str());
	if (!pScene)
		return NULL;

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(*pScene);
	const gltfSceneObject* pObject = gltfPlacement::FindMeshObject(objects, i_Part);
	if (!pObject)
	{
		DBG_WARNING("glTF file " << filename << " has no mesh node " << i_Part);
		return NULL;
	}

	fsResourceTracker::MarkBegin(i_ModelLocator);

	std::unique_ptr<emdlCharacterTemplate> mdl_template(new emdlCharacterTemplate());

	g3dSceneNode* pBase;
	gltfModelImport::LoadNodeMesh(	pScene.get(),
									*pObject,
									i_ModelLocator,
									pBase,
									mdl_template->SkinnedSurfaces(),
									mdl_template->Fragments(),
									mdl_template->MaterialTable(),
									mdl_template->Materials());

	mdl_template->SetModel( pBase );
	mdl_template->SetFilename( i_ModelLocator );

	fsResourceTracker::MarkEnd(i_ModelLocator);
	return mdl_template.release();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
scObject* gltfImport::CreateObject( const entModelTemplate& i_Template,
								    entModelInstance &o_Instance,
								    const fsResourceFinder& i_TextureFinder,
									const std::vector< shared_ptr<mdlMaterialInfo> > &i_MaterialOverrides )
{
	// Since we are using the emdlCharacterTemplate, we can let the base implementation
	// create the correct object for us.
	return NULL;
}


//----------------------------------------------------------------------------
// Load shared animation key info from file
//----------------------------------------------------------------------------
entAnimKeys* gltfImport::LoadAnimKeys(const fsLocator& i_AnimLocator)
{
	// No animation implementation yet.
	return NULL;
}

//----------------------------------------------------------------------------
// Load one clip built for one mesh node
//----------------------------------------------------------------------------
entAnimKeys* gltfImport::LoadAnimKeysPart(const fsLocator& i_AnimLocator,
										  const std::string& i_Part,
										  const std::string& i_Clip)
{
	std::string filename;
	fsFileUtil::LocatorToANSIFilename(i_AnimLocator, filename);
	if (!gltfSdkManager::IsRecognizedFileFormat(filename.c_str()))
		return NULL;	// let the next implementation try

	std::shared_ptr<const tinygltf::Model> pScene = gltfSdkManager::LoadSceneShared(filename.c_str());
	if (!pScene)
		return NULL;

	const std::vector<gltfSceneObject> objects = gltfSceneRead::ReadScene(*pScene);
	const gltfSceneObject* pObject = gltfPlacement::FindMeshObject(objects, i_Part);
	if (!pObject)
	{
		DBG_WARNING("glTF file " << filename << " has no mesh node " << i_Part << " for animation " << i_Clip);
		return NULL;
	}

	gltfClip clip;
	if (!gltfAnimRead::ReadClip(*pScene, i_Clip, g3dConstants::c_fDefaultFrameRate, clip))
	{
		DBG_WARNING("glTF file " << filename << " has no animation " << i_Clip << " (for " << i_Part << ")");
		return NULL;
	}

	fsResourceTracker::MarkBegin(i_AnimLocator);
	emdlCharacterAnimKeys* pKeys = gltfAnimKeys::Build(*pScene, *pObject, clip, g3dConstants::c_fDefaultFrameRate);
	fsResourceTracker::MarkEnd(i_AnimLocator);
	if (!pKeys)
		DBG_WARNING("glTF file " << filename << ": animation " << i_Clip << " doesn't move " << i_Part);
	return pKeys;
}

//----------------------------------------------------------------------------
// Create animation from key data
//----------------------------------------------------------------------------
entAnimation* gltfImport::CreateAnimation(const entAnimKeys &i_KeyData)
{
	// emdlImport::CreateAnimation makes the animation from the
	// emdlCharacterAnimKeys LoadAnimKeysPart builds.
	return NULL;
}
