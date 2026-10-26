/****************************************************************************\
**  gltfModelImport.hpp
**
**      gltfModelImport.hpp converts scenes loaded with the FBX SDK
**	importer into our format scene graph.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#pragma once
#ifdef GLTF_MODELIMPORT_HPP
#error gltfModelImport.hpp multiply included
#endif
#define GLTF_MODELIMPORT_HPP

#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 

#include "tiny_gltf.h"

#ifndef GLTF_SCENEOBJECTS_HPP
#include "ImportExport/gltf/import/private/gltfSceneObjects.hpp"
#endif

#include <vector>

class fsLocator;
class maVector3d;
//class fsResourceFinder;
class g3dFragment;
class g3dSceneNode;
class matMaterial;
//class matTexture;
struct mdlSkinInfo;

//----------------------------------------------------------------------------
//	Any of these gltfModelImport functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//----------------------------------------------------------------------------
namespace gltfModelImport
{

	//------------------------------------------------------------------------
	//	LoadModel converts the geometry in the scene from the FBX SDK 
	//	into our scene graph.
	//------------------------------------------------------------------------
	void LoadModel(	tinygltf::Model* pScene,
					const fsLocator& i_Locator,
					//const fsResourceFinder& i_TextureFinder,
					g3dSceneNode*& o_pSceneNode,
					std::vector<mdlSkinInfo>& o_SkinData,
					std::vector<g3dFragment*>& o_Fragments,
					mdlMatInfoTable& o_MaterialTable,
					std::vector<matMaterial*>& o_Materials);
					//std::vector<matTexture*>& o_Textures);

	//------------------------------------------------------------------------
	//	LoadNodeMesh converts one mesh object's mesh into our scene graph:
	//	one base node named after the glTF node (no node transform; the
	//	object placing it carries that). gltfModelImport.cpp has the three
	//	cases: skinned, morph-animated and static.
	//------------------------------------------------------------------------
	void LoadNodeMesh(	const tinygltf::Model* i_pScene,
						const gltfSceneObject& i_Object,
						const fsLocator& i_Locator,
						g3dSceneNode*& o_pSceneNode,
						std::vector<mdlSkinInfo>& o_SkinData,
						std::vector<g3dFragment*>& o_Fragments,
						mdlMatInfoTable& o_MaterialTable,
						std::vector<matMaterial*>& o_Materials);
}

