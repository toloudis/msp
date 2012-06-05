/********************************************************************************************\
**	mtrMaterialUtil.hpp
**
**		Routines for helping set material values from a mdlMaterialInfo.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef	MTR_MATERIALUTIL_HPP
#error	mtrMaterialUtil.hpp included recursively.
#endif
#define	MTR_MATERIALUTIL_HPP

#include <vector>


//============================================================================================
//	forward references
//============================================================================================
class entModelInstance;
class fsLocator;
class g3dFragment;
class g3dSceneNode;
class matMaterial;
class mdlMaterialInfo;
class scObject;


//============================================================================================
//	mtrMaterialUtil Functions
//============================================================================================
namespace mtrMaterialUtil
{
	//----------------------------------------------------------------------------
	//	Replace old material with new material in the given list of fragments
	//----------------------------------------------------------------------------
	void ReplaceMaterial( matMaterial* i_OldMaterial,
						  matMaterial* i_NewMaterial,
						  std::vector<g3dFragment*>& io_Fragments );

	//------------------------------------------------------------------------
	// Remove textures from material and its template
	//------------------------------------------------------------------------
	void RemoveTextures(matMaterial& io_Material,
						entModelInstance& i_ModelInstance);

	//------------------------------------------------------------------------
	// Remove all textures from material and its template
	//------------------------------------------------------------------------
	void RemoveAllTextures(matMaterial& io_Material,
						entModelInstance& i_ModelInstance);

	//--------------------------------------------------------------------
	// Gather fragments from template or object in order to
	//	have list of fragments for altering materials.
	//--------------------------------------------------------------------
	void GatherFragments(scObject *i_pObject, 
						 entModelInstance &i_Template,
						 std::vector<g3dFragment*> &o_Fragments);

	//--------------------------------------------------------------------
	// Gather nodes from the given object that have fragments
	//--------------------------------------------------------------------
	void GatherFragmentNodes(scObject *i_pObject, 
							 std::vector<g3dSceneNode*> &o_Nodes);

	//--------------------------------------------------------------------
	// Gather nodes from the given object that are using the 
	//	given material.
	//--------------------------------------------------------------------
	void GatherMaterialNodes(scObject *i_pObject, 
							 const matMaterial *i_pMaterial,
							 std::vector<g3dSceneNode*> &o_Nodes);

	//----------------------------------------------------------------------------
	//	Returns true if there are texture differences between the
	//	two material information data structures.
	//----------------------------------------------------------------------------
	//bool DidTextureChange(const mdlMaterialInfo &i_OldMatInfo,
	//					  const mdlMaterialInfo &i_NewMatInfo);

	//----------------------------------------------------------------------------
	//	Set Material properties from structure
	//----------------------------------------------------------------------------
	void SetMaterialData(matMaterial &o_Material,
						const mdlMaterialInfo &i_MaterialTemplate,
						entModelInstance& i_ModelInstance,
						const fsLocator& i_TextureDirectory,
						const fsLocator& i_GeneralTextureDirectory,
						bool i_bForceReload = false);

	//----------------------------------------------------------------------------
	//	Set Material properties from structure
	//----------------------------------------------------------------------------
	void SetMaterialLayerData(matMaterial &o_Material,
					int i_LayerIndex,
					const mdlMaterialInfo &i_MaterialTemplate,
					entModelInstance& i_ModelInstance,
					const fsLocator& i_TextureDirectory,
					const fsLocator& i_GeneralTextureDirectory);

	//------------------------------------------------------------------------
	// Merge Transparent textures from material and its template
	//------------------------------------------------------------------------
	void MergeTransparentTextures(matMaterial& io_Material,	const mdlMaterialInfo& i_MaterialTemplate);

};



