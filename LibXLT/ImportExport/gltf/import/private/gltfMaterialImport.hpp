/****************************************************************************\
**  gltfMaterialImport.hpp
**
**      gltfMaterialImport.hpp converts materials loaded with the FBX SDK
**	importer into our format fragments.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#pragma once
#ifdef GLTF_MATERIALIMPORT_HPP
#error gltfMaterialImport.hpp multiply included
#endif
#define GLTF_MATERIALIMPORT_HPP

#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 

#include "tiny_gltf.h"

#include <string>
#include <vector>

class fsLocator;
class fsResourceFinder;
class matMaterial;
class matTexture;
class mdlMaterialInfo;



//----------------------------------------------------------------------------
//	Any of these gltfMaterialImport functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//----------------------------------------------------------------------------
namespace gltfMaterialImport
{
	//--------------------------------------------------------------------
	// CreateSimpleMaterial - create simple grey phong material
	//--------------------------------------------------------------------
	void CreateSimpleMaterial(mdlMaterialInfo& o_MatInfo);

	//--------------------------------------------------------------------
	// GetMaterialKey - unique material name for a glTF material index,
	// used both as the material's name and its material table key (the
	// app looks materials up in the table by name). Index -1 (a primitive
	// with no material) maps to the glTF default material. Unnamed or
	// repeated names are made unique with the material index.
	//--------------------------------------------------------------------
	std::string GetMaterialKey(const tinygltf::Model* i_pModel, int i_MaterialIndex);

	//--------------------------------------------------------------------
	// Get materials from node containing a mesh
	//--------------------------------------------------------------------
	void GetNodeMaterials(const tinygltf::Model* i_pModel, tinygltf::Mesh* i_pMesh,
						  mdlMatInfoTable& io_MaterialTable,
						  //const fsResourceFinder& i_TextureFinder,
						  std::vector<matMaterial*>& o_Materials,
						  //std::vector<matTexture*>& o_Textures,
						  const bool i_bCreateMaterials,
						  const fsLocator& i_ContainingFile);

}

