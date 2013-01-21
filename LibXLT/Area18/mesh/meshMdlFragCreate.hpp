/****************************************************************************\
**  meshMdlFragCreate.hpp
**
**      meshMdlFragCreate supplies functions for controlling what
**	types of fragments are created by the mayPackage. In the
**	future, this may provide convience functions for loading
**	textures and creating fragments from mdlFragInfo structs.
**
** Area17
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAY_FRAGCREATE_HPP
#error meshMdlFragCreate.hpp multiply included
#endif
#define MAY_FRAGCREATE_HPP

#ifndef MDL_FRAGCREATE_HPP
#include "Graphics/mdl/mdlFragCreate.hpp"
#endif

//----------------------------------------------------------------------------
//	Any of these meshMdlFragCreate functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//----------------------------------------------------------------------------
class meshMdlFragCreate : public mdlFragCreateImpl
{
public:
	enum Choice
	{
		e_Always = 0,
		e_WhenAppropriate,
		e_Never
	};

	//------------------------------------------------------------------------
	// Controls when to use bump fragments.  Some apps may want to use
	// bump fragments always and some may never want to use them.
	// This is "e_WhenAppropriate" by default.
	//------------------------------------------------------------------------
	static Choice GetBumpFragChoice();
	static void SetBumpFragChoice(Choice i_Choice);

	//------------------------------------------------------------------------
	//	Returns true if this fragment should use a bumpTriMeshBumpFrag
	//  instead of a g3dFragment
	//  Inputs should be number of texture coordinates per vertex
	//  and the special effect of the material to use.
	//------------------------------------------------------------------------
	virtual bool UseBumpFrag(int i_NumTexCoords, bool i_EffectQualified);

	//------------------------------------------------------------------------
	//	Create a single fragment from a mdlSplitFragInfo struct
	//------------------------------------------------------------------------
	virtual g3dFragment* CreateFragment(const mdlSplitFragInfo & i_Info,
								   bool i_Morphable = false);

	//------------------------------------------------------------------------
	//	Create a hair fragment from a mdlHairInfo struct
	//------------------------------------------------------------------------
	g3dFragment* CreateHairFragment(const mdlHairInfo &i_HairInfo,
									matMaterial *i_pMaterial);

	//------------------------------------------------------------------------
	// Alter the triangle and vertex ordering to best utilize the
	//	vertex cache in the GPU
	//------------------------------------------------------------------------
	virtual void OptimizeFragment(mdlSplitFragInfo &io_Info);

	//------------------------------------------------------------------------
	// Create multiple fragments from a multiple material surface 
	//	structure such that they share the same vertex buffer.
	//	Returns pointer to shared vertex buffer, owned by the caller.
	//------------------------------------------------------------------------
	virtual g3dVertexBuffer* CreateFragmentGroup(const mdlFragInfo &i_FragInfo,
									 std::vector<g3dFragment*> &o_Fragments,
									 bool i_Morphable = false);

	//------------------------------------------------------------------------
	// Update an existing multiple fragment group based on a new topology.
	// The assumption is that the number of vertices and indices has changed
	// (i.e. through a subdivision level change). 
	//------------------------------------------------------------------------
	virtual void UpdateFragmentGroup(const mdlFragInfo &i_FragInfo,
									 g3dVertexBuffer &io_SharedVertexBuffer,
									 std::vector<g3dFragment*> &io_Fragments,
									 bool i_Morphable = false);


private:
	static Choice sm_Choice;
};
