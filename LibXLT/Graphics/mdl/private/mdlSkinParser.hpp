/****************************************************************************\
**	mdlSkinParser.hpp
**
**		mdlSkinParser.hpp supplies functions used to import meshes 
**	from an older version of our Maya plugin. 
**
**	In this format, the vertices and normals have different indices
**	and they have to be sorted together in order to be rendered in D3D.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_SKINPARSER_HPP
#error mdlSkinParser.hpp multiply included
#endif
#define MDL_SKINPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 
#ifndef MDL_SKINUTIL_HPP
#include "Graphics/mdl/mdlSkinUtil.hpp"
#endif 

#include <vector>


//============================================================================
//	Forward References
//============================================================================
class chReader;
class chWriter;
class mdlFragInfo;
class mdlSubdivInfo;
struct smdlBoneVertex;
struct smdlCharacterSkin;
struct smdlMorphTarget;


//============================================================================
//	Any of these mdlSkinParser functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlSkinParser
{
	//------------------------------------------------------------------------
	// Read in a morph target for a subdivision surface skin in a 
	// character model.
	//------------------------------------------------------------------------
	void ReadMRPH(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					const mdlSubdivInfo& i_SubdivInfo,
					smdlMorphTarget& o_MorphTarget);

	//------------------------------------------------------------------------
	// Read in a morph target for a polygon mesh skin in a 
	// character model.
	//------------------------------------------------------------------------
	void ReadMRPH(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					const mdlFragInfo& i_FragInfo,
					smdlMorphTarget& o_MorphTarget);

	//------------------------------------------------------------------------
	// Read tree of how joints influence a mesh
	//------------------------------------------------------------------------
	void ReadINFT(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					int& io_nJointIndex,
					std::vector<mdlSkinUtil::JointInfluence>& o_JointInfluences );

	//------------------------------------------------------------------------
	// Read vector of influences of how joints influence a surface
	//------------------------------------------------------------------------
	void ReadBINF(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					std::vector<smdlBoneVertex>& o_BoneVertices );

	//------------------------------------------------------------------------
	// Read in a skin in multiple skin-single skeleton character model.
	// The shared_ptr arguments should be NULL when being passed in,
	// depending on what is read, they will be filled in with data.
	// Either a subdiv or a frag info will be read, not both.
	//------------------------------------------------------------------------
	void ReadSKIN(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					smdlCharacterSkin &o_Skin,
					shared_ptr<mdlSubdivInfo>& o_SubdivInfo,
					shared_ptr<mdlFragInfo>& o_MeshInfo,
					mdlMatInfoTable &i_MaterialTable );

	//------------------------------------------------------------------------
	// Write joint influences for a surface.
	//------------------------------------------------------------------------
	//void WriteINFG(	chWriter& o_Writer,
	//				const mdlSkinInfo &i_SkinInfo );
	void WriteBINF(	chWriter& o_Writer,
					const std::vector<smdlBoneVertex> &i_BoneVertices );

	//------------------------------------------------------------------------
	// Write a morph target for a surface
	//------------------------------------------------------------------------
	void WriteMRPH(	chWriter& o_Writer,
					const smdlMorphTarget &i_MorphTarget );

	//------------------------------------------------------------------------
	// Write skinning information and a surface. One function for meshes
	//	and one function for subdivs.
	//------------------------------------------------------------------------
	void WriteSKIN(	chWriter& o_Writer,
					const smdlCharacterSkin &i_SkinInfo,
					const mdlSubdivInfo &i_SubdivInfo );
	void WriteSKIN(	chWriter& o_Writer,
					const smdlCharacterSkin &i_SkinInfo,
					const mdlFragInfo &i_MeshInfo );
}

