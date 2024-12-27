/*****************************************************************************
**  gltfSdkManager.hpp
**
**      The gltfSdkManager holds the SDK Manager needed for importing
**	FBX files through the SDK.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#pragma once

#ifdef GLTF_SDKMANAGER_HPP
#error gltfSdkManager.hpp multiply included
#endif
#define GLTF_SDKMANAGER_HPP

#include "tiny_gltf.h"


namespace gltfSdkManager 
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Init();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanUp() throw();

	//------------------------------------------------------------------------
	// Is this a file that our gltf importer can recognize?
	//------------------------------------------------------------------------
	bool IsRecognizedFileFormat(const char* pFilename);

	//------------------------------------------------------------------------
	// Create a scene for the gltf with the given filename.
	//------------------------------------------------------------------------
	tinygltf::Model* LoadScene(const char* pFilename);

}

