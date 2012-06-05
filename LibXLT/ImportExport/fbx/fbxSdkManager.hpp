/*****************************************************************************
**  fbxSdkManager.hpp
**
**      The fbxSdkManager holds the SDK Manager needed for importing
**	FBX files through the SDK.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef FBX_SDKMANAGER_HPP
#error fbxSdkManager.hpp multiply included
#endif
#define FBX_SDKMANAGER_HPP

#ifndef FBX_SDK_HPP
#include "ImportExport/fbx/fbxSdk.hpp"
#endif 

#ifdef USE_FBX_IMPORTEXPORT

namespace fbxSdkManager 
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Init();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanUp() throw();

	//------------------------------------------------------------------------
	// Is this a file that our FBX importer can recognize?
	//------------------------------------------------------------------------
	bool IsRecognizedFileFormat(const char* pFilename);

	//------------------------------------------------------------------------
	// Create a scene for the FBX with the given filename.
	//------------------------------------------------------------------------
	KFbxScene* LoadScene(const char* pFilename);

	//------------------------------------------------------------------------
	// GetManager()
	//------------------------------------------------------------------------
	KFbxSdkManager* GetManager();
}


#endif // USE_FBX_IMPORTEXPORT
