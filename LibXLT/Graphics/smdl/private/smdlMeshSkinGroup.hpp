/*****************************************************************************
**	smdlMeshSkinGroup.hpp
**
**		smdlMeshSkinGroup handles a multi-material mesh that was a single
**	mesh in Maya, but is now multiple fragments in Terawatt. It handles skinning, 
**	morph target animation and direct vertex animation.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_MESHSKINGROUP_HPP
#error smdlMeshSkinGroup.hpp multiply included
#endif
#define SMDL_MESHSKINGROUP_HPP

#ifndef SMDL_SURFACE_HPP
#include "Graphics/smdl/private/smdlSurface.hpp"
#endif
#ifndef SMDL_VERTEXANIMKEYS_HPP
#include "Graphics/smdl/smdlVertexAnimKeys.hpp"
#endif 

#include <map>
#include <vector>


//============================================================================
//============================================================================
class g3dFragment;
class g3dSceneNode;
class maMatrix4x4;
class matMaterial;
class mdlFragInfo;
struct smdlCharacterSkin;


//============================================================================
//============================================================================
class smdlMeshSkinGroup : public smdlSurface
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		typedef std::vector<int> RemapArray;

		//--------------------------------------------------------------------
		//	smdlMeshSkinGroup represents a skinned mesh with multiple
		//	materials. It has to split the surface into multiple fragments
		//	and keep track of remapping arrays to animate them.
		//--------------------------------------------------------------------
		smdlMeshSkinGroup(const mdlFragInfo& i_FragInfo,
						  const smdlCharacterSkin& i_CharacterSkin,
						  const std::map<const matMaterial*,matMaterial*>& i_MaterialRemapping,
						  bool i_bVertexAnimation = false);

		//--------------------------------------------------------------------
		//	smdlMeshSkinGroup requires the fragments and bone vertex data.
		//	Assumes ownership of the fragments. Bone vertices can be shared
		//
		//	The i_SplitRemapping array matches from the vertices in the 
		//	fragments to the single array of bone vertices.  There should be
		//	one RemapArray per fragment and each RemapArray should have
		//	an index into the BoneVertices array per vertex in the
		//	fragment.
		//
		//	The i_FullVertexRemapping and i_FullNormalRemapping maps are 
		//	only needed if the mesh will be	vertex animated. These can
		//	be generated from the mdlFragInfos using mdlFragCreate.
		//
		//	The CharacterSkin info is only needed if the mesh has skinned
		//	influences for jointed animation and/or morph targets.
		//--------------------------------------------------------------------
		//smdlMeshSkinGroup(	const std::vector<g3dFragment*>& i_Fragments,
		//					const std::vector<RemapArray> &i_SplitRemapping,
		//					const std::vector<RemapArray> &i_FullVertexRemapping,
		//					const std::vector<RemapArray> &i_FullNormalRemapping,
		//					const smdlCharacterSkin& i_CharacterSkin,
		//					const std::string& i_MeshName,
		//					int i_NumOriginalVertices,
		//					int i_NumOriginalNormals);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		~smdlMeshSkinGroup();

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
		// Name for mesh, used to look for baked vertex animation
		//--------------------------------------------------------------------
		inline const std::string& GetMeshName() const;

		//--------------------------------------------------------------------
		// Const access to character skin
		//--------------------------------------------------------------------
		inline const smdlCharacterSkin& GetCharacterSkin() const;

		//--------------------------------------------------------------------
		//	TransformMesh - given the joint matrices and weights for the
		//		influence of the morph targets, transform the
		//		vertices based on the vertex influences.
		//--------------------------------------------------------------------
		void TransformMesh( const maMatrix4x4* i_BoneMatrices,
							std::vector<float> i_Weights );

		//--------------------------------------------------------------------
		// CheckAnimation checks compatibility of this animation 
		// with the model. It will return true if the animation can
		// be played on this model.
		//--------------------------------------------------------------------
		bool CheckAnimation( const smdlVertexAnimKeys &i_VertKeys ) const;

		//--------------------------------------------------------------------
		//	VertexTransformMesh - use baked vertex animation to transform
		//		the meshes in this group.
		//--------------------------------------------------------------------
		void VertexTransformMesh(float i_CurrentFrame,
								 const smdlVertexAnimKeys &i_VertKeys);

		//--------------------------------------------------------------------
		// Animation data is giving us the bounding box for the surface 
		//	before the actual animation is done. Set this bounding box 
		//	into the fragment.
		//--------------------------------------------------------------------
		void BBoxTransform(const maAxisBox &i_BBox);

		//--------------------------------------------------------------------
		// Access to mesh group name
		//--------------------------------------------------------------------
		std::string GetMeshGroupName();

		//--------------------------------------------------------------------
		// Access to vertices
		//--------------------------------------------------------------------
		std::vector<maPoint3d>* GetVertices();

		//--------------------------------------------------------------------
		// Access to normals
		//--------------------------------------------------------------------
		std::vector<maVector3d>* GetNormals();

		//--------------------------------------------------------------------
		// Access to indices
		//--------------------------------------------------------------------
		std::vector<envType::UInt32>* GetIndices();

		//--------------------------------------------------------------------
		// Access to fragments
		//--------------------------------------------------------------------
		std::vector<g3dFragment*>& GetFragments();

		//--------------------------------------------------------------------
		// Access to node
		//--------------------------------------------------------------------
		g3dSceneNode* GetNode();

	private:
		g3dSceneNode* m_pNode;
		std::string m_MeshName;
		std::vector<maPoint3d> m_BasePositions;
		std::vector<maVector3d> m_BaseNormals;
		std::vector<g3dFragment*> m_Fragments;
		//vector of  copies of the index buffer-s of the fragments
		std::vector< std::vector<envType::UInt32> > m_FragmentIndexVecs;
		std::vector<RemapArray> m_SplitRemapping;
		std::vector<RemapArray> m_FullVertexRemapping;
		std::vector<RemapArray> m_FullNormalRemapping;
		const smdlCharacterSkin& m_CharacterSkin;
		int m_nNumVertices;
		int m_NumOriginalVertices, m_NumOriginalNormals;

		// Arrays for transforming vertices and normals
		// and then mapping those vertices into split fragments
		std::vector<maPoint3d> m_XformedVerts;
		std::vector<maPoint3d> m_XformedNorms;
};


//--------------------------------------------------------------------
// Name for mesh, used to look for baked vertex animation
//--------------------------------------------------------------------
inline const std::string& smdlMeshSkinGroup::GetMeshName() const
{
	return m_MeshName;
}

//--------------------------------------------------------------------
// Const access to character skin
//--------------------------------------------------------------------
inline const smdlCharacterSkin& smdlMeshSkinGroup::GetCharacterSkin() const
{
	return m_CharacterSkin;
}