/****************************************************************************\
**  fbxMaterialImport.hpp
**
**      fbxMaterialImport.hpp converts materials loaded with the FBX SDK
**	importer into our format fragments.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef FBX_MATERIALIMPORT_HPP
#error fbxMaterialImport.hpp multiply included
#endif
#define FBX_MATERIALIMPORT_HPP

#ifndef FBX_SDK_HPP
#include "ImportExport/fbx/fbxSdk.hpp"
#endif
#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 

#include <vector>

class fsLocator;
class fsResourceFinder;
class matMaterial;
class matTexture;
class mdlMaterialInfo;


#ifdef USE_FBX_IMPORTEXPORT

//----------------------------------------------------------------------------
//	Any of these fbxMaterialImport functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//----------------------------------------------------------------------------
namespace fbxMaterialImport
{
	//--------------------------------------------------------------------
	// CreateSimpleMaterial - create simple grey phong material
	//--------------------------------------------------------------------
	void CreateSimpleMaterial(mdlMaterialInfo& o_MatInfo);

	//--------------------------------------------------------------------
	// Get materials from node containing a mesh
	//--------------------------------------------------------------------
	void GetNodeMaterials(KFbxNode& i_Node,
						  mdlMatInfoTable& io_MaterialTable,
						  //const fsResourceFinder& i_TextureFinder,
						  std::vector<matMaterial*>& o_Materials,
						  //std::vector<matTexture*>& o_Textures,
						  const bool i_bCreateMaterials,
						  const fsLocator& i_ContainingFile);

}

#endif // USE_FBX_IMPORTEXPORT
