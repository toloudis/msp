/****************************************************************************\
**  gltfSceneImport.hpp
**
**      gltfSceneImport is the glTF iscnSceneImportImpl (registered by
**	gltfPackage): it reads a .gltf or .glb file's objects
**	(gltfSceneFile::ReadObjects) and turns them into MachStudio's terms
**	(Convert; gltfSceneImport.cpp has the mapping).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GLTF_SCENEIMPORT_HPP
#error gltfSceneImport.hpp multiply included
#endif
#define GLTF_SCENEIMPORT_HPP

#ifndef ISCN_SCENEIMPORT_HPP
#include "ImportExport/iscn/iscnSceneImport.hpp"
#endif
#ifndef GLTF_SCENEOBJECTS_HPP
#include "ImportExport/gltf/import/private/gltfSceneObjects.hpp"
#endif

#include <string>
#include <vector>

class gltfSceneImport : public iscnSceneImportImpl
{
public:
	virtual bool IsRecognized(const fsLocator& i_File) const;
	virtual std::string GetFilePatterns() const;
	virtual bool Read(const fsLocator& i_File, const iscnSceneSettings& i_Settings, iscnSceneDesc& o_Scene);

	//------------------------------------------------------------------------
	// The objects gltfSceneRead gives, as MachStudio's objects; i_FileName
	// names the file in warnings
	//------------------------------------------------------------------------
	static void Convert(const std::vector<gltfSceneObject>& i_Objects, const std::string& i_FileName,
						const iscnSceneSettings& i_Settings, iscnSceneDesc& o_Scene);
};
