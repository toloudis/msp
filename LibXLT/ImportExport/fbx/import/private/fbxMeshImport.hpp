/****************************************************************************\
**  fbxMeshImport.hpp
**
**      fbxMeshImport.hpp converts meshes loaded with the FBX SDK
**	importer into our format fragments.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef FBX_MESHIMPORT_HPP
#error fbxMeshImport.hpp multiply included
#endif
#define FBX_MESHIMPORT_HPP

#ifndef FBX_SDK_HPP
#include "ImportExport/fbx/fbxSdk.hpp"
#endif 
#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 


#include <vector>

class fsLocator;
//class fsResourceFinder;
class g3dFragment;
class g3dSceneNode;
class matMaterial;
//class matTexture;
struct mdlSkinInfo;

#ifdef USE_FBX_IMPORTEXPORT

//----------------------------------------------------------------------------
//	Any of these fbxMeshImport functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//----------------------------------------------------------------------------
namespace fbxMeshImport
{
	//------------------------------------------------------------------------
	//	ConvertMesh converts the geometry in the mesh from the FBX SDK 
	//	into our fragment type.
	//------------------------------------------------------------------------
	void ConvertMesh( KFbxMesh& i_Mesh,
					  g3dSceneNode*& io_pSceneNode,
					  //const fsResourceFinder& i_TextureFinder,
					  mdlMatInfoTable& io_MaterialTable,
					  std::vector<g3dFragment*>& o_Fragments,
					  std::vector<matMaterial*>& o_Materials,
					  const fsLocator& i_ContainingFile);
					  //std::vector<matTexture*>& o_Textures);


}

#endif // USE_FBX_IMPORTEXPORT
