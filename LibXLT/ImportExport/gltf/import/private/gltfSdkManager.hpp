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

#include <memory>
#include <vector>

struct gltfSceneObject;

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

	//------------------------------------------------------------------------
	// The parsed file, kept in a one-entry cache keyed by path, size and
	// last write time, so the per-node loads of one file parse it once and
	// a file changed on disk is parsed again. NULL if it doesn't parse.
	//------------------------------------------------------------------------
	std::shared_ptr<const tinygltf::Model> LoadSceneShared(const char* pFilename);

	//------------------------------------------------------------------------
	// The derived objects for a non-null shared model. Kept in a one-entry
	// cache by model ownership, so geometry and clip loads reuse the same
	// scene traversal and animation analysis. Reloaded models get new objects.
	//------------------------------------------------------------------------
	std::shared_ptr<const std::vector<gltfSceneObject>> ReadSceneObjectsShared(
		const std::shared_ptr<const tinygltf::Model>& i_pModel);

}
