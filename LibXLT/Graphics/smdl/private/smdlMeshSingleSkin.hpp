/*****************************************************************************
**	smdlMeshSingleSkin.hpp
**
**		smdlMeshSingleSkin handles a single skinned polygon mesh
**	with no remapping.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_MESHSINGLESKIN_HPP
#error smdlMeshSingleSkin.hpp multiply included
#endif
#define SMDL_MESHSINGLESKIN_HPP

#ifndef MA_MATRIX4X4_HPP
#include "Core/Ma/maMatrix4x4.hpp"
#endif 
#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif 
#ifndef SMDL_SURFACE_HPP
#include "Graphics/smdl/private/smdlSurface.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class g3dFragment;
class g3dSceneNode;
class maMatrix4x4;
struct mdlSplitFragInfo;
struct smdlBoneVertex;


//============================================================================
//============================================================================
class smdlMeshSingleSkin  : public smdlSkinnedMeshSurface
{
	public:
		//--------------------------------------------------------------------
		//	smdlMeshSingleSkin requires the skinning and bone vertex data. 
		//	Assumes ownership of the fragment. Bone vertices can be shared
		//--------------------------------------------------------------------
		smdlMeshSingleSkin(	const mdlSplitFragInfo& i_SplitFragInfo,
							const std::vector<smdlBoneVertex>& i_BoneVertices,
							const maMatrix4x4& i_BindPose);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~smdlMeshSingleSkin();

		//--------------------------------------------------------------------
		// Return node that contains the fragments in this model in a small
		//	sub-scene graph
		//--------------------------------------------------------------------
		g3dSceneNode* RootNode();

		//--------------------------------------------------------------------
		//	Visible - set/get whether the given surface is renderable
		//--------------------------------------------------------------------
		virtual void SetVisible(bool i_bVisible);
		virtual bool GetVisible() const;

		//--------------------------------------------------------------------
		//	TransformMesh - given the joint matrices, transform the
		//		vertices based on the vertex influences.
		//--------------------------------------------------------------------
		virtual void TransformMesh( const maMatrix4x4* i_BoneMatrices );

	private:
		g3dSceneNode* m_pNode;
		g3dFragment* m_pFragment;
		const std::vector<smdlBoneVertex>& m_BoneVertices;
		maMatrix4x4 m_BindPose;
		std::vector<maPoint3d> m_BasePositions;
		std::vector<maVector3d> m_BaseNormals;
		int m_nNumVertices;
};
