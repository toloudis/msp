 /*****************************************************************************
**  fbxImport.cpp
**
**      The fbxImport implements importing functionality for the
**	Autodesk FBX SDK.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/fbx/import/fbxImport.hpp"

#include "ImportExport/fbx/fbxSdkManager.hpp"
#include "ImportExport/fbx/import/private/fbxModelImport.hpp"

#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsResourceTracker.hpp"
#include "Graphics/emdl/emdlCharacterTemplate.hpp"

namespace
{

#ifdef USE_FBX_IMPORTEXPORT
	// Make sure the scene is destroyed if exception is thrown
	struct kbxSceneWrapper
	{
		KFbxDocument* m_pScene;
		kbxSceneWrapper(KFbxDocument* i_pScene) : m_pScene(i_pScene) {}
		~kbxSceneWrapper() 
		{  
			if (m_pScene) 
				m_pScene->Destroy();
		}
	};

#endif // USE_FBX_IMPORTEXPORT
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fbxImport::fbxImport()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fbxImport::~fbxImport()
{

}

//----------------------------------------------------------------------------
//  LoadGeometry
//----------------------------------------------------------------------------
entModelTemplate* fbxImport::LoadGeometry( const fsLocator&	i_ModelLocator,
										   //const fsResourceFinder& i_TextureFinder,
										   entFragInfoSink* o_Sink)
{

#ifdef USE_FBX_IMPORTEXPORT
	//	What kind of model is it?
	std::string filename;
	fsFileUtil::LocatorToANSIFilename(i_ModelLocator, filename);
	if (fbxSdkManager::IsRecognizedFileFormat(filename.c_str())) 
	{
		KFbxScene* pScene = fbxSdkManager::LoadScene(filename.c_str());
		if (pScene)
		{
			// This will destroy the scene, including if an exception is thrown
			kbxSceneWrapper clean_up_scene(pScene); 

			fsResourceTracker::MarkBegin(i_ModelLocator);

			//	Use character template for all generalized hierarchical models now.
			std::auto_ptr<emdlCharacterTemplate> mdl_template(new emdlCharacterTemplate());

			g3dSceneNode* pBase;
			fbxModelImport::LoadModel(	pScene,
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

#endif // USE_FBX_IMPORTEXPORT

	// return NULL and let next implementation try to handle it
	return NULL;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
scObject* fbxImport::CreateObject( const entModelTemplate& i_Template,
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
entAnimKeys* fbxImport::LoadAnimKeys(const fsLocator& i_AnimLocator)
{
	// No animation implementation yet.
	return NULL;
}

//----------------------------------------------------------------------------
// Create animation from key data
//----------------------------------------------------------------------------
entAnimation* fbxImport::CreateAnimation(const entAnimKeys &i_KeyData)
{
	// No animation implementation yet.
	return NULL;
}
