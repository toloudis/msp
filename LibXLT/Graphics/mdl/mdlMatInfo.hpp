/****************************************************************************\
**	mdlMatInfo.hpp
**
**		Contains structures for passing around material data.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_MATINFO_HPP
#error mdlMatInfo.hpp multiply included
#endif
#define MDL_MATINFO_HPP

#ifndef MDL_MATERIALINFO_HPP
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#endif 


//============================================================================
//============================================================================
class matMaterial;
class fsResourceFinder;
class matTexture;


//============================================================================
// Reduced mdlMatInfo to an association between
// material information structure and a pointer to a material.
// This info struct does not own the material.
//============================================================================
struct mdlMatInfo
{
	mdlMatInfo();
	mutable matMaterial* m_pMaterial;
	mdlMaterialInfo m_Info;

	//------------------------------------------------------------------------
	// Create material pointer and set name, but don't load effects 
	//	or textures.
	//------------------------------------------------------------------------
	void CreateMaterial();

	//------------------------------------------------------------------------
	// Load textures from effect data into material.
	// This function creates the m_pMaterial pointer if needed.
	//------------------------------------------------------------------------
	void LoadTextures(const fsResourceFinder& i_TextureFinder,
					   std::vector<matTexture*>& o_Textures);

	//------------------------------------------------------------------------
	// Loads textures from this material info into any material, 
	// not just the m_pMaterial pointer.
	//------------------------------------------------------------------------
	void LoadTextures(matMaterial *o_pMaterial,
					  const fsResourceFinder& i_TextureFinder,
					  std::vector<matTexture*>& o_Textures);
};

