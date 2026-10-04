 /*****************************************************************************
**  gltfSdkManager.cpp
**
**      
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/gltf/import/private/gltfSdkManager.hpp"

#include "Core/Dbg/dbgMsg.hpp"

#include <string.h>

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
		//--------------------------------------------------------------------
		// case-insensitive check of the file extension
		//--------------------------------------------------------------------
		bool has_extension(const char* pFilename, const char* pExtension)
		{
			if (!pFilename)
				return false;
			const size_t name_len = strlen(pFilename);
			const size_t ext_len = strlen(pExtension);
			if (name_len < ext_len)
				return false;
			return _stricmp(pFilename + name_len - ext_len, pExtension) == 0;
		}

		bool IsAsciiFile(const char* pFilename)  { return has_extension(pFilename, ".gltf"); }
		bool IsBinaryFile(const char* pFilename) { return has_extension(pFilename, ".glb"); }
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
	// Is this a file that our glTF importer can recognize?
	// Only .gltf (JSON) and .glb (binary) files are handled.
	//------------------------------------------------------------------------
	bool IsRecognizedFileFormat(const char* pFilename)
	{
		return IsAsciiFile(pFilename) || IsBinaryFile(pFilename);
	}
	
	//------------------------------------------------------------------------
	// Create a scene for the glTF with the given filename.
	//------------------------------------------------------------------------
	tinygltf::Model* LoadScene(const char* pFilename)
	{
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		tinygltf::Model* pScene = new tinygltf::Model();

		bool ret = false;
		if (IsBinaryFile(pFilename))
			ret = loader.LoadBinaryFromFile(pScene, &err, &warn, pFilename); // for binary glTF(.glb)
		else
			ret = loader.LoadASCIIFromFile(pScene, &err, &warn, pFilename);

		if (!warn.empty()) {
			DBG_WARNING("glTF warning loading " << pFilename << ": " << warn);
		}
		if (!err.empty()) {
			DBG_ERROR("glTF error loading " << pFilename << ": " << err);
		}

		if (!ret) {
			// Clear out the failed scene object
			delete pScene;
			pScene = NULL;
		}

		return pScene;
	}


} // end of namespace


