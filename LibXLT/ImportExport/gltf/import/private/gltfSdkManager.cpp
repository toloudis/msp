 /*****************************************************************************
**  gltfSdkManager.cpp
**
**      
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/gltf/import/private/gltfSdkManager.hpp"
#include "ImportExport/gltf/import/private/gltfSceneRead.hpp"

#include "Core/Dbg/dbgMsg.hpp"

#include <string.h>

#include <filesystem>
#include <mutex>
#include <system_error>

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

		//--------------------------------------------------------------------
		// LoadSceneShared's one-entry cache
		//--------------------------------------------------------------------
		struct SceneCache
		{
			std::string m_Path;
			std::uintmax_t m_Size = 0;
			std::filesystem::file_time_type m_WriteTime;
			std::shared_ptr<const tinygltf::Model> m_pModel;
		};
		std::mutex s_CacheMutex;
		SceneCache s_Cache;
		std::shared_ptr<const tinygltf::Model> s_pObjectsModel;
		std::shared_ptr<const std::vector<gltfSceneObject>> s_pObjects;
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
		std::lock_guard<std::mutex> lock(s_CacheMutex);
		s_Cache = SceneCache();
		s_pObjects.reset();
		s_pObjectsModel.reset();
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


	//------------------------------------------------------------------------
	// The parsed file, from the cache when its path, size and write time
	// are those of the cached one.
	//------------------------------------------------------------------------
	std::shared_ptr<const tinygltf::Model> LoadSceneShared(const char* pFilename)
	{
		std::error_code ec;
		const std::filesystem::path path = std::filesystem::u8path(pFilename);	// the name is UTF-8 (fsFileUtil)
		const std::uintmax_t size = std::filesystem::file_size(path, ec);
		if (ec)
		{
			DBG_ERROR("glTF error loading " << pFilename << ": " << ec.message());
			return NULL;
		}
		const std::filesystem::file_time_type write_time = std::filesystem::last_write_time(path, ec);

		std::lock_guard<std::mutex> lock(s_CacheMutex);
		if (s_Cache.m_pModel && s_Cache.m_Path == pFilename && s_Cache.m_Size == size && s_Cache.m_WriteTime == write_time)
			return s_Cache.m_pModel;

		std::shared_ptr<const tinygltf::Model> pModel(LoadScene(pFilename));
		if (pModel)
		{
			s_Cache.m_Path = pFilename;
			s_Cache.m_Size = size;
			s_Cache.m_WriteTime = write_time;
			s_Cache.m_pModel = pModel;
		}
		return pModel;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::shared_ptr<const std::vector<gltfSceneObject>> ReadSceneObjectsShared(
		const std::shared_ptr<const tinygltf::Model>& i_pModel)
	{
		DBG_ASSERT(i_pModel != NULL, "Cannot read glTF objects without a model");
		std::lock_guard<std::mutex> lock(s_CacheMutex);
		if (s_pObjectsModel == i_pModel && s_pObjects)
			return s_pObjects;

		auto objects = std::make_shared<const std::vector<gltfSceneObject>>(gltfSceneRead::ReadScene(*i_pModel));
		s_pObjectsModel = i_pModel;
		s_pObjects = objects;
		return objects;
	}

} // end of namespace

