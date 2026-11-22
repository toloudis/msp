/****************************************************************************\
**  gltfSceneRead.hpp
**
**      gltfSceneRead reads a glTF scene's node tree into the list of
**	MachStudio objects it becomes: groups, meshes, lights and cameras,
**	each with its identity (its glTF name path), its transform in
**	centimetres, and its light or camera values in MachStudio's units.
**	It needs no app or GPU, so it is tested on its own; the importer
**	creates the objects from it. The objects are in gltfSceneObjects.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GLTF_SCENEREAD_HPP
#error gltfSceneRead.hpp multiply included
#endif
#define GLTF_SCENEREAD_HPP

#ifndef GLTF_SCENEOBJECTS_HPP
#include "ImportExport/gltf/import/private/gltfSceneObjects.hpp"
#endif

#include "tiny_gltf.h"

#include <string>
#include <vector>

namespace gltfSceneRead
{
	// The default scene (else scene 0; none: empty), parents before children
	std::vector<gltfSceneObject> ReadScene(const tinygltf::Model& i_Model);
}
