/****************************************************************************\
**  mdlFragCreate.cpp
**
**      mdlFragCreate supplies functions for creating fragments from 
**	fragment info structures.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlFragCreate.hpp"

#include "Core/dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
namespace mdlFragCreate
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	namespace
	{
		bool l_bDisableOptimization = false;
		mdlFragCreateImpl* l_pImplementation = NULL;
	}

	//------------------------------------------------------------------------
	//	Returns true if this fragment should use a bumpTriMeshBumpFrag
	//  instead of a g3dFragment
	//------------------------------------------------------------------------
	bool UseBumpFrag(int i_NumTexCoords, 
					 bool i_EffectQualified)
	{
		DBG_ASSERT(l_pImplementation, "mdlFragCreate: No implementation");
		if (!l_pImplementation) return false;
		return l_pImplementation->UseBumpFrag(i_NumTexCoords, i_EffectQualified);
	}

	//------------------------------------------------------------------------
	//	Create a single fragment from a mdlSplitFragInfo struct
	//------------------------------------------------------------------------
	g3dFragment* CreateFragment(const mdlSplitFragInfo & i_FragInfo,
								bool i_Morphable)
	{
		DBG_ASSERT(l_pImplementation, "mdlFragCreate: No implementation");
		if (!l_pImplementation) return NULL;
		return l_pImplementation->CreateFragment(i_FragInfo, i_Morphable);
	}

	//------------------------------------------------------------------------
	//	Create a hair fragment from a mdlHairInfo struct
	//------------------------------------------------------------------------
	g3dFragment* CreateHairFragment(const mdlHairInfo &i_HairInfo, matMaterial *i_pMaterial)
	{
		DBG_ASSERT(l_pImplementation, "mdlFragCreate: No implementation");
		if (!l_pImplementation) return NULL;
		return l_pImplementation->CreateHairFragment(i_HairInfo, i_pMaterial);
	}

	//------------------------------------------------------------------------
	// Set flag to enable fragment optimizations
	//------------------------------------------------------------------------
	void EnableFragmentOptimize(bool i_bEnabled)
	{
		l_bDisableOptimization = !i_bEnabled;
	}

	//------------------------------------------------------------------------
	// Alter the triangle and vertex ordering to best utilize the
	//	vertex cache in the GPU
	//------------------------------------------------------------------------
	void OptimizeFragment(mdlSplitFragInfo &io_Info)
	{
		if (l_bDisableOptimization)
		{
			// Disabling function globally as test
			return;
		}

		DBG_ASSERT(l_pImplementation, "mdlFragCreate: No implementation");
		if (l_pImplementation)
			l_pImplementation->OptimizeFragment(io_Info);
	}

	//------------------------------------------------------------------------
	// Create multiple fragments from a multiple material surface 
	//	structure such that they share the same vertex buffer.
	//	Returns pointer to shared vertex buffer, owned by the caller.
	//------------------------------------------------------------------------
	g3dVertexBuffer* CreateFragmentGroup(const mdlFragInfo &i_FragInfo,
										 std::vector<g3dFragment*> &o_Fragments,
										 bool i_Morphable)
	{
		DBG_ASSERT(l_pImplementation, "mdlFragCreate: No implementation");
		if (!l_pImplementation) return NULL;
		return l_pImplementation->CreateFragmentGroup(i_FragInfo, o_Fragments, i_Morphable);
	}

	//------------------------------------------------------------------------
	// Update an existing multiple fragment group based on a new topology.
	// The assumption is that the number of vertices and indices has changed
	// (i.e. through a subdivision level change). 
	//------------------------------------------------------------------------
	void UpdateFragmentGroup(const mdlFragInfo &i_FragInfo,
							 g3dVertexBuffer &io_SharedVertexBuffer,
							 std::vector<g3dFragment*> &io_Fragments,
							 bool i_Morphable)
	{
		DBG_ASSERT(l_pImplementation, "mdlFragCreate: No implementation");
		if (l_pImplementation)
			l_pImplementation->UpdateFragmentGroup(i_FragInfo, io_SharedVertexBuffer, io_Fragments, i_Morphable);
	}

	//--------------------------------------------------------------------
	// Set new implementation method, returns pointer to last one
	// that was being used.  Both can be NULL.
	// Ownership for the pointer remains with the caller.
	//--------------------------------------------------------------------
	mdlFragCreateImpl* SetImplementation(mdlFragCreateImpl* i_pCreator)
	{
		mdlFragCreateImpl* old_impl = l_pImplementation;
		l_pImplementation = i_pCreator;
		return old_impl;
	}

}	// end of namespace
