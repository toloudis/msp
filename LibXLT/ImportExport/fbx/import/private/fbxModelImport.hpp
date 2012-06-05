/****************************************************************************\
**  fbxModelImport.hpp
**
**      fbxModelImport.hpp converts scenes loaded with the FBX SDK
**	importer into our format scene graph.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef FBX_MODELIMPORT_HPP
#error fbxModelImport.hpp multiply included
#endif
#define FBX_MODELIMPORT_HPP

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
//	Any of these fbxModelImport functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//----------------------------------------------------------------------------
namespace fbxModelImport
{

	//------------------------------------------------------------------------
	//	LoadModel converts the geometry in the scene from the FBX SDK 
	//	into our scene graph.
	//------------------------------------------------------------------------
	void LoadModel(	KFbxScene* pScene,
					const fsLocator& i_Locator,
					//const fsResourceFinder& i_TextureFinder,
					g3dSceneNode*& o_pSceneNode,
					std::vector<mdlSkinInfo>& o_SkinData,
					std::vector<g3dFragment*>& o_Fragments,
					mdlMatInfoTable& o_MaterialTable,
					std::vector<matMaterial*>& o_Materials);
					//std::vector<matTexture*>& o_Textures);


}

#endif // USE_FBX_IMPORTEXPORT
