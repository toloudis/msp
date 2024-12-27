 /*****************************************************************************
**  fbxSdkManager.cpp
**
**      
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/gltf/import/private/gltfSdkManager.hpp"

#include "Core/Dbg/dbgMsg.hpp"

// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"

namespace gltfSdkManager
{

	namespace
	{
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Init()
	{
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanUp() throw()
	{
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
	tinygltf::Model* LoadScene(const char* pFilename)
	{
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		tinygltf::Model* pScene = new tinygltf::Model();

		bool ret = loader.LoadASCIIFromFile(pScene, &err, &warn, pFilename);
		if (!warn.empty()) {
			DBG_WARNING("Warn: " << warn);
		}
		if (!err.empty()) {
			DBG_ERROR("Err: " << err);
		}

		if (!ret) {
			ret = loader.LoadBinaryFromFile(pScene, &err, &warn, pFilename); // for binary glTF(.glb)
			if (!warn.empty()) {
				DBG_WARNING("Warn: " << warn);
			}
			if (!err.empty()) {
				DBG_ERROR("Err: " << err);
			}
		}

		if (!ret) {
			// report out any errors.. 
			// 
			// Clear out the failed scene object
			delete pScene;
			pScene = NULL;
		}

		return pScene;
	}


} // end of namespace


