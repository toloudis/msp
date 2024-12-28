/****************************************************************************\
**  gltfMeshImport.hpp
**
**      gltfMeshImport.hpp converts meshes loaded with the FBX SDK
**	importer into our format fragments.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef GLTF_MESHIMPORT_HPP
#error gltfMeshImport.hpp multiply included
#endif
#define GLTF_MESHIMPORT_HPP

#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 

#include "tiny_gltf.h"

#include <vector>

class fsLocator;
//class fsResourceFinder;
class g3dFragment;
class g3dSceneNode;
class matMaterial;
//class matTexture;
struct mdlSkinInfo;

//----------------------------------------------------------------------------
//	Any of these gltfMeshImport functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//----------------------------------------------------------------------------
namespace gltfMeshImport
{
	//------------------------------------------------------------------------
	//	ConvertMesh converts the geometry in the mesh from the FBX SDK 
	//	into our fragment type.
	//------------------------------------------------------------------------
	void ConvertMesh( const tinygltf::Model* i_pModel, tinygltf::Mesh* i_Mesh,
					  g3dSceneNode*& io_pSceneNode,
					  //const fsResourceFinder& i_TextureFinder,
					  mdlMatInfoTable& io_MaterialTable,
					  std::vector<g3dFragment*>& o_Fragments,
					  std::vector<matMaterial*>& o_Materials,
					  const fsLocator& i_ContainingFile);
					  //std::vector<matTexture*>& o_Textures);


}

