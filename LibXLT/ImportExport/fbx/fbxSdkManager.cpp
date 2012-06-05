 /*****************************************************************************
**  fbxSdkManager.cpp
**
**      
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/fbx/fbxSdkManager.hpp"

#include "Core/Dbg/dbgMsg.hpp"


#ifdef USE_FBX_IMPORTEXPORT

//#include <fbxfilesdk/kfbxio/kfbxstreamoptionsfbx.h>

// kfbxiosettings will replace stream options in the near future
//#include <fbxfilesdk/kfbxio/kfbxiosettings.h>

namespace fbxSdkManager
{

	namespace
	{
		KFbxSdkManager* l_pSdkManager = NULL;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Init()
	{
		// Create the FBX SDK manager which is the 
		// object allocator for almost all the classes in the SDK.
		l_pSdkManager = KFbxSdkManager::Create();

		KFbxIOSettings * ios = KFbxIOSettings::Create(l_pSdkManager, IOSROOT);
		l_pSdkManager->SetIOSettings(ios);

	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanUp() throw()
	{
		// Delete the FBX SDK manager. All the objects that have been allocated 
		// using the FBX SDK manager and that haven't been explicitely destroyed 
		// are automatically destroyed at the same time.
		if (l_pSdkManager) 
			l_pSdkManager->Destroy();
		l_pSdkManager = NULL;
	}

	
	//------------------------------------------------------------------------
	// Is this a file that our FBX importer can recognize?
	//------------------------------------------------------------------------
	bool IsRecognizedFileFormat(const char* pFilename)
	{
		//DBG_ASSERT(l_pSdkManager, "FBX SDK Manager not created, call Init() first.");
		//
		//int lFileFormat = -1;
		//return (l_pSdkManager->GetIOPluginRegistry()->DetectFileFormat(pFilename, lFileFormat));
		return true;
	}
	
	//------------------------------------------------------------------------
	// Create a scene for the FBX with the given filename.
	//------------------------------------------------------------------------
	KFbxScene* LoadScene(const char* pFilename)
	{
		DBG_ASSERT(l_pSdkManager, "FBX SDK Manager not created, call Init() first.");

		KFbxScene* pScene = NULL;

		//KFbxStreamOptionsFbxReader* lImportOptions = KFbxStreamOptionsFbxReader::Create(l_pSdkManager, "");

		// Get the file version number generate by the FBX SDK.
		//KFbxIO::GetCurrentVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		// Create an importer.
		KFbxImporter* lImporter = KFbxImporter::Create(l_pSdkManager,"");

		int lFileFormat = -1;
		//if (!l_pSdkManager->GetIOPluginRegistry()->DetectFileFormat(pFilename, lFileFormat))
		//{
		//	// Unrecognizable file format. Try to fall back to native format.
		//	lFileFormat = l_pSdkManager->GetIOPluginRegistry()->GetNativeReaderFormat();
		//}
		//lImporter->SetFileFormat(lFileFormat);

		// Initialize the importer by providing a filename.
		const bool bImportStatus = lImporter->Initialize(pFilename);
		//lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if( bImportStatus )
		{
			// Create the entity that will hold the scene.
			pScene = KFbxScene::Create(l_pSdkManager,"");

			// Import the scene.
			//if (!lImporter->Import(pScene, lImportOptions))
			if (!lImporter->Import(pScene))
			{
				// Clear out the failed scene object
				pScene->Destroy();
				pScene = NULL;
			}
		}

		// Destroy the importer.
		//if(lImportOptions)
		//	lImportOptions->Destroy();
		//lImportOptions=NULL;
		lImporter->Destroy();

		return pScene;
	}

	//------------------------------------------------------------------------
	// GetManager()
	//------------------------------------------------------------------------
	KFbxSdkManager* GetManager()
	{
		return l_pSdkManager;
	}


} // end of namespace


#endif // USE_FBX_IMPORTEXPORT
