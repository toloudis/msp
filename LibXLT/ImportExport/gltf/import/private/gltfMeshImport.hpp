/****************************************************************************\
**  gltfMeshImport.hpp
**
**      gltfMeshImport.hpp converts meshes loaded with tinygltf
**	into our format fragments.
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
	//	glTF units are meters, MSP works in centimeters
	//------------------------------------------------------------------------
	const float c_UnitScale = 100.0f;

	//------------------------------------------------------------------------
	//	ConvertMesh converts the geometry in the glTF mesh 
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

