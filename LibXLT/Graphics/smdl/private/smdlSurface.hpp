/*****************************************************************************
**	smdlSurface.hpp
**
**		smdlSurface is abstract base class for polygon and subdiv groups
**	that can be contained by an object.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_SURFACE_HPP
#error smdlSurface.hpp multiply included
#endif
#define SMDL_SURFACE_HPP

#ifndef ENV_THREAD_HPP
#include "Core/env/envThread.hpp"
#endif
#ifndef SMDL_VERTEXANIMKEYS_HPP
#include "Graphics/smdl/smdlVertexAnimKeys.hpp"
#endif 


//============================================================================
//============================================================================
class g3dFragment;
class g3dSceneNode;
class g3dVertexBuffer;
class maMatrix4x4;
struct smdlCharacterSkin;


//============================================================================
//============================================================================
class smdlSurface
{
	public:
		//--------------------------------------------------------------------
		//	Visible - set/get whether the given surface is renderable
		//--------------------------------------------------------------------
		virtual void SetVisible(bool i_bVisible) = 0;
		virtual bool GetVisible() const = 0;

		//--------------------------------------------------------------------
		// Get access to the mutex to use to synchronize all 
		// animation threads.
		//--------------------------------------------------------------------
		static envMutex& GetSurfaceMutex();

		//--------------------------------------------------------------------
		// LockFragment - Only the "Unlock" actually makes any D3D calls.
		//	So, any synchronization of threads needs to be done on the
		//	unlock call.
		//--------------------------------------------------------------------
		unsigned char* LockFragment(g3dFragment *i_pFragment);

		//--------------------------------------------------------------------
		// UnlockFragment - Only the "Unlock" actually makes any D3D calls.
		//	So, any synchronization of threads needs to be done on the
		//	unlock call.
		//--------------------------------------------------------------------
		void UnlockFragment(g3dFragment *i_pFragment);

		//--------------------------------------------------------------------
		// LockVertexBuffer - Only the "Unlock" actually makes any D3D calls.
		//	So, any synchronization of threads needs to be done on the
		//	unlock call.
		//--------------------------------------------------------------------
		unsigned char* LockVertexBuffer(g3dVertexBuffer *i_pVertexBuffer);

		//--------------------------------------------------------------------
		// UnlockVertexBuffer - Only the "Unlock" actually makes any D3D calls.
		//	So, any synchronization of threads needs to be done on the
		//	unlock call.
		//--------------------------------------------------------------------
		void UnlockVertexBuffer(g3dVertexBuffer *i_pVertexBuffer);

	protected:
		static envMutex sm_SurfaceMutex;
};


//============================================================================
//============================================================================
class smdlSkinnedMeshSurface : public smdlSurface
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~smdlSkinnedMeshSurface() {}

		//--------------------------------------------------------------------
		//	TransformMesh - given the joint matrices, transform the
		//		vertices based on the vertex influences.
		//--------------------------------------------------------------------
		virtual void TransformMesh( const maMatrix4x4* i_BoneMatrices ) = 0;
};


//============================================================================
//virtual base class for surfaces that get enhanced information (i.e. tessellation, subdivision)
//============================================================================
class smdlEnhancedSurface : public smdlSurface
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~smdlEnhancedSurface(){}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual const smdlCharacterSkin& GetCharacterSkin() const = 0;

	//--------------------------------------------------------------------
	// Return node that contains the fragments in this model in a small
	//	sub-scene graph
	//--------------------------------------------------------------------
	virtual g3dSceneNode* RootNode() = 0;

	//--------------------------------------------------------------------
	// Set the current subdivision level for the surface.
	//--------------------------------------------------------------------
	virtual void SetCurrentSubdivLevel(int i_SubdivLevel) = 0;

	//--------------------------------------------------------------------
	// Name for subdiv, used to look for baked vertex animation
	//--------------------------------------------------------------------
	virtual const std::string& GetSubdivName() const = 0;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual int GetNumFacesAtLevel( int i_SubdivLevel ) const = 0;

	//--------------------------------------------------------------------
	// CheckAnimation checks compatibility of this animation 
	// with the model. It will return true if the animation can
	// be played on this model.
	//--------------------------------------------------------------------
	virtual bool CheckAnimation( const smdlVertexAnimKeys &i_VertKeys ) const = 0;

	//--------------------------------------------------------------------
	//	TransformSubdiv - given the joint matrices and weights for the
	//		influence of the morph targets, transform the
	//		base mesh based on the vertex influences. Then
	//		propagate the base mesh changes through to the current
	//		subdivision level.
	//--------------------------------------------------------------------
	virtual void TransformSubdiv( const maMatrix4x4* i_BoneMatrices,
		const std::vector<float>& i_Weights ) = 0;

	//--------------------------------------------------------------------
	//	VertexTransformSubdiv - use baked vertex animation to transform
	//		the base mesh of the subdivision surface.
	//--------------------------------------------------------------------
	virtual void VertexTransformSubdiv(float i_CurrentFrame,
		const smdlVertexAnimKeys &i_VertKeys) = 0;

	//--------------------------------------------------------------------
	// Animation data is giving us the bounding box for the surface 
	//	before the actual animation is done. Set this bounding box 
	//	into the fragment.
	//--------------------------------------------------------------------
	virtual void BBoxTransform(const maAxisBox &i_BBox) = 0;
};
