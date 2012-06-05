/****************************************************************************\
**	mdlFragCreate.hpp
**
**		mdlFragCreate supplies functions for creating fragments from 
**	fragment info structures. 
**
**		A generic API is defined and graphic systems can implement the
**	API in their specific way.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_FRAGCREATE_HPP
#error mdlFragCreate.hpp multiply included
#endif
#define MDL_FRAGCREATE_HPP

#include <string>
#include <vector>


//============================================================================
//============================================================================
class g3dFragment;
class g3dVertexBuffer;
class matMaterial;
class mdlFragCreateImpl;
class mdlFragInfo;
struct mdlHairInfo;
struct mdlSplitFragInfo;


//============================================================================
// Namespace defining the API that can be implemented by the graphics system
//============================================================================
namespace mdlFragCreate
{
	//------------------------------------------------------------------------
	//	Returns true if this fragment should use a bumpTriMeshBumpFrag
	//  instead of a g3dFragment
	//  Inputs should be number of texture coordinates per vertex
	//  and the special effect of the material to use.
	//------------------------------------------------------------------------
	bool UseBumpFrag(int i_NumTexCoords, 
					 bool i_EffectQualified);

	//------------------------------------------------------------------------
	//	Create a single fragment from a mdlSplitFragInfo struct
	//------------------------------------------------------------------------
	g3dFragment* CreateFragment(const mdlSplitFragInfo & i_Info,
								   bool i_Morphable = false);

	//------------------------------------------------------------------------
	//	Create a hair fragment from a mdlHairInfo struct
	//------------------------------------------------------------------------
	g3dFragment* CreateHairFragment(const mdlHairInfo &i_HairInfo,
									matMaterial *i_pMaterial);

	//------------------------------------------------------------------------
	// Set flag to enable fragment optimizations
	//------------------------------------------------------------------------
	void EnableFragmentOptimize(bool i_bEnabled);

	//------------------------------------------------------------------------
	// Alter the triangle and vertex ordering to best utilize the
	//	vertex cache in the GPU
	//------------------------------------------------------------------------
	void OptimizeFragment(mdlSplitFragInfo &io_Info);

	//------------------------------------------------------------------------
	// Create multiple fragments from a multiple material surface 
	//	structure such that they share the same vertex buffer.
	//	Returns pointer to shared vertex buffer, owned by the caller.
	//------------------------------------------------------------------------
	g3dVertexBuffer* CreateFragmentGroup(const mdlFragInfo &i_FragInfo,
										 std::vector<g3dFragment*> &o_Fragments,
										 bool i_Morphable = false);

	//------------------------------------------------------------------------
	// Update an existing multiple fragment group based on a new topology.
	// The assumption is that the number of vertices and indices has changed
	// (i.e. through a subdivision level change). 
	//------------------------------------------------------------------------
	void UpdateFragmentGroup(const mdlFragInfo &i_FragInfo,
							 g3dVertexBuffer &io_SharedVertexBuffer,
							 std::vector<g3dFragment*> &io_Fragments,
							 bool i_Morphable);

	//--------------------------------------------------------------------
	// Set new implementation method, returns pointer to last one
	// that was being used.  Both can be NULL.
	// Ownership for the pointer remains with the caller.
	//--------------------------------------------------------------------
	mdlFragCreateImpl* SetImplementation(mdlFragCreateImpl* i_pCreator);
}


//============================================================================
// Class to derive from in order to implement the mdlFragCreate API
//============================================================================
class mdlFragCreateImpl
{
public:
	//------------------------------------------------------------------------
	//	Returns true if this fragment should use a bumpTriMeshBumpFrag
	//  instead of a g3dFragment
	//  Inputs should be number of texture coordinates per vertex
	//  and the special effect of the material to use.
	//------------------------------------------------------------------------
	virtual bool UseBumpFrag(int i_NumTexCoords, 
					 bool i_EffectQualified) = 0;

	//------------------------------------------------------------------------
	//	Create a single fragment from a mdlSplitFragInfo struct
	//------------------------------------------------------------------------
	virtual g3dFragment* CreateFragment(const mdlSplitFragInfo & i_Info,
								   bool i_Morphable = false) = 0;

	//------------------------------------------------------------------------
	//	Create a hair fragment from a mdlHairInfo struct
	//------------------------------------------------------------------------
	virtual g3dFragment* CreateHairFragment(const mdlHairInfo &i_Info,
											matMaterial *i_pMaterial) = 0;

	//------------------------------------------------------------------------
	// Alter the triangle and vertex ordering to best utilize the
	//	vertex cache in the GPU
	//------------------------------------------------------------------------
	virtual void OptimizeFragment(mdlSplitFragInfo &io_Info) = 0;

	//------------------------------------------------------------------------
	// Create multiple fragments from a multiple material surface 
	//	structure such that they share the same vertex buffer.
	//	Returns pointer to shared vertex buffer, owned by the caller.
	//------------------------------------------------------------------------
	virtual g3dVertexBuffer* CreateFragmentGroup(const mdlFragInfo &i_FragInfo,
									 std::vector<g3dFragment*> &o_Fragments,
									 bool i_Morphable = false) = 0;

	//------------------------------------------------------------------------
	// Update an existing multiple fragment group based on a new topology.
	// The assumption is that the number of vertices and indices has changed
	// (i.e. through a subdivision level change). 
	//------------------------------------------------------------------------
	virtual void UpdateFragmentGroup(const mdlFragInfo &i_FragInfo,
									 g3dVertexBuffer &io_SharedVertexBuffer,
									 std::vector<g3dFragment*> &io_Fragments,
									 bool i_Morphable = false) = 0;
};
