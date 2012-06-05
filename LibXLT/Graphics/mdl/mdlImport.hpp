/****************************************************************************\
**	mdlImport.hpp
**
**		mdlImport.hpp supplies functions used to import files from Maya
**	(written by our Maya plugin).
**
**		When refactored correctly, mdlReader will specialize in reading
**	files into data structures for processing and mayImport will specialize 
**	in reading files into our graphics objects in order to be rendered.
**
**	StudioGPU
**	Copyright(C) 2003-8 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_IMPORT_HPP
#error mdlImport.hpp multiply included
#endif
#define MDL_IMPORT_HPP

#ifndef ENV_EXCEPTIONX_HPP
#include "Core/env/envExceptionX.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef MDL_FRAGINFO_HPP
#include "Graphics/mdl/mdlFragInfo.hpp"
#endif
#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MDL_SPLITFRAGINFO_HPP
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
#endif
#ifndef SMDL_TREE_HPP
#include "Graphics/smdl/smdlTree.hpp"
#endif

#include <map>
#include <string>
#include <vector>


//============================================================================
//============================================================================
class entFragInfoSink;
class fsLocator;
class fsResourceFinder;
class g3dFragment;
class g3dSceneNode;
class matMaterial;
class matTexture;
class mdlSubdivInfo;
struct mdlHairInfo;
struct mdlSkinInfo;
struct smdlBoneVertex;
struct smdlCharacterSkin;


//============================================================================
//	Any of these mdlImport functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlImport
{
	//------------------------------------------------------------------------
	//	LoadWorldFragments loads a bunch of fragments which are already
	//	placed in the world, and their associated textures and materials.
	//------------------------------------------------------------------------
	void LoadWorldFragments(const fsLocator& i_File,
							//const fsResourceFinder& i_TextureFinder,
							std::vector<g3dFragment*>& o_Fragments,
							std::vector<g3dFragment*>& o_LowResFragments,
							std::vector<g3dFragment*>& o_HighResFragments,
							mdlMatInfoTable& o_MaterialTable,
							std::vector<matMaterial*>& o_Materials,
							//std::vector<matTexture*>& o_Textures,
							entFragInfoSink* o_Sink = NULL);

	//------------------------------------------------------------------------
	//	LoadFragInfos loads fragment info and materials from a file, but
	//  does not split or create fragments.
	//------------------------------------------------------------------------
	void LoadFragInfos(	const fsLocator& i_File,
						//const fsResourceFinder& i_TextureFinder,
						std::vector<mdlFragInfo>& o_FragInfos,
						mdlMatInfoTable& o_MaterialTable,
						std::vector<matMaterial*>& o_Materials,
						//std::vector<matTexture*>& o_Textures,
						entFragInfoSink* o_Sink = NULL );

	//------------------------------------------------------------------------
	//	LoadModel loads a hierarchical model from a file.  It will create
	//	fragments and assign materials to them as specified in the file.
	//------------------------------------------------------------------------
	//void LoadHierarchicalModel(	const fsLocator& i_File,
	//							const fsResourceFinder& i_TextureFinder,
	//							g3dSceneNode*& o_pSceneNode,
	//							std::vector<g3dFragment*>& o_Fragments,
	//							std::vector<matMaterial*>& o_Materials,
	//							std::vector<matTexture*>& o_Textures,
	//							entFragInfoSink* o_Sink = NULL);

	//------------------------------------------------------------------------
	//	LoadModel loads a single-skin model from a file. 
	//------------------------------------------------------------------------
	void LoadSingleSkinModel(	const fsLocator& i_File,
								//const fsResourceFinder& i_TextureFinder,
								g3dSceneNode*& o_pJointTree,
								std::vector<smdlBoneVertex>& o_BoneData,
								std::vector<mdlFragInfo>& o_FragInfos,
								mdlMatInfoTable& o_MaterialTable,
								std::vector<matMaterial*>& o_Materials,
								//std::vector<matTexture*>& o_Textures,
								entFragInfoSink* o_Sink = NULL );

	//------------------------------------------------------------------------
	//	LoadCharacterModel loads information for a compound character model.
	//		Character models have multiple skins attached to a single 
	//		skeleton and express emotions through morph targets.
	//------------------------------------------------------------------------
	void LoadCharacterModel(	const fsLocator& i_File,
								//const fsResourceFinder& i_TextureFinder,
								g3dSceneNode*& o_pJointTree,
								std::vector<smdlCharacterSkin>& o_SkinData,
								std::vector<mdlSubdivInfo>& o_SubdivInfos,
								std::vector<mdlFragInfo>& o_FragInfos,
								std::vector<g3dFragment*>& o_Fragments,
								mdlMatInfoTable& o_MaterialTable,
								std::vector<matMaterial*>& o_Materials,
								//std::vector<matTexture*>& o_Textures,
								entFragInfoSink* o_Sink = NULL );

	//------------------------------------------------------------------------
	//	LoadGeneralHierarchicalModel loads a generalized hierarchical model 
	//	from a file.  It handles nodes and joints, static fragments and
	//	skined surfaces.  Static fragments are created directly in the
	//	node graph, skinned surfaces are returned in the mdlSkinInfo vector
	//	and need to be created by the object.
	//------------------------------------------------------------------------
	void LoadGeneralHierarchicalModel(	const fsLocator& i_Locator,
										//const fsResourceFinder& i_TextureFinder,
										g3dSceneNode*& o_pSceneNode,
										//std::vector<smdlJoint*>& o_JointRoots,
										std::vector<mdlSkinInfo>& o_SkinData,
										std::vector< shared_ptr<mdlHairInfo> >& o_HairData,
										std::vector<g3dFragment*>& o_Fragments,
										mdlMatInfoTable& o_MaterialTable,
										std::vector<matMaterial*>& o_Materials,
										//std::vector<matTexture*>& o_Textures,
										entFragInfoSink* o_Sink);

}
