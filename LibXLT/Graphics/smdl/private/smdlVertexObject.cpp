/*****************************************************************************
**	smdlVertexObject.hpp
**
**		smdlVertexObject represents an object which is made up of
**	multiple fragments with baked vertex animation.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/smdlVertexObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
#include "Graphics/smdl/private/smdlMeshSkinGroup.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"
#include "Graphics/smdl/smdlVertexAnimation.hpp"

#include <sstream>


//============================================================================
//============================================================================
namespace
{
	typedef std::vector<int> RemapArray;

	// Shared empty skin info since our models are
	// not influenced by joints.
	const std::vector<RemapArray> c_EmptyRemapping;
	const smdlCharacterSkin c_EmptySkinningInfo;

	struct sMeshGroup
	{
		std::vector<g3dFragment*> m_Fragments;
		std::vector<RemapArray> m_VertexRemapping;
		std::vector<RemapArray> m_NormalRemapping;
	};
}

//--------------------------------------------------------------------
//	smdlVertexObject is initialized with the fragment info. The 
//	constructor will split the frag infos and create morphable 
//	fragments that the object will then animate.
//--------------------------------------------------------------------
smdlVertexObject::smdlVertexObject( const std::vector<mdlFragInfo>& i_FragInfos,
									const std::map<const matMaterial*,matMaterial*>& i_MaterialRemapping )
:	m_CurAnim(NULL),
	m_CurStartTime(0.0f),
	m_bDirty(true),
	m_LastAnimateTime(-1.0f)
{
	const int num_frags = i_FragInfos.size();
	//std::vector<sMeshGroup> mesh_groups(num_frags);
	//for (int i=0; i<num_frags; ++i)
	//{
	//	sMeshGroup &mesh_group = mesh_groups[i];

	//	// Expand out remapping of vertices:
	//	std::vector<int> primary_vremap(	i_FragInfos[i].m_Vertices.size(), 0 );
	//	int num_orig_verts = i_FragInfos[i].m_VertexRemap.size();
	//	std::multimap<int, int>::const_iterator it, end = i_FragInfos[i].m_VertexRemap.end();
	//	for( it = i_FragInfos[i].m_VertexRemap.begin(); it != end; ++it )
	//	{
	//		int nOldVertexIndex = it->first;
	//		int nNewVertexIndex = it->second;
	//		primary_vremap[nNewVertexIndex] = nOldVertexIndex;
	//	}
	//	std::vector<int> primary_nremap(	i_FragInfos[i].m_Vertices.size(), 0 );
	//	int num_orig_norms = i_FragInfos[i].m_NormalRemap.size();
	//	for( it = i_FragInfos[i].m_NormalRemap.begin(); it != i_FragInfos[i].m_NormalRemap.end(); ++it )
	//	{
	//		primary_nremap[it->second] = it->first;
	//	}

	//	// Split fragment by material
	//	std::vector<mdlSplitFragInfo> split_frag_info;
	//	mdlFragUtil::SplitFragments(i_FragInfos[i], split_frag_info);

	//	// Create tmesh fragment per material chunk
	//	int num_split_frags = split_frag_info.size();
	//
	//	mesh_group.m_Fragments.resize(num_split_frags);
	//	for (int j=0; j<num_split_frags; j++)
	//	{
	//		const mdlSplitFragInfo &info = split_frag_info[j];
	//		g3dFragment* pFragment = mdlFragCreate::CreateFragment(info, true);
	//		//m_Fragments.push_back( pFragment );
	//		mesh_group.m_Fragments[j] = pFragment;
	//		//mesh_group.m_Remapping[j] = info.m_RemapArray;
	//	}

	//	// Combine remappings for vertex animation
	//	mdlFragUtil::GenerateFullRemappings(i_FragInfos[i], 
	//										  split_frag_info, 
	//										  mesh_group.m_VertexRemapping, 
	//										  mesh_group.m_NormalRemapping);
	//}

	// Now create the mesh groups to manage the fragments
	m_Meshes.resize(num_frags);
	for (int i=0; i<num_frags; ++i)
	{
		//sMeshGroup &mesh_group = mesh_groups[i];
		//m_Meshes[i] = new smdlMeshSkinGroup(mesh_group.m_Fragments, 
		//									c_EmptyRemapping,
		//									mesh_group.m_VertexRemapping, 
		//									mesh_group.m_NormalRemapping,
		//									c_EmptySkinningInfo,
		//									i_FragInfos[i].m_Name,
		//									i_FragInfos[i].m_NumOrigVertices,
		//									i_FragInfos[i].m_NumOrigNormals);
		const bool bVertexAnimation = true;
		m_Meshes[i] = new smdlMeshSkinGroup(i_FragInfos[i], 
											c_EmptySkinningInfo, 
											i_MaterialRemapping,
											bVertexAnimation);

		// Add fragment to our scene graph
		this->GetBase()->AddChild( m_Meshes[i]->RootNode() );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlVertexObject::~smdlVertexObject()
{
	envSTLHelpers::DeleteContainer(m_Meshes);

	// delete current animation
	delete m_CurAnim;
}

//--------------------------------------------------------------------
//	Animate
//--------------------------------------------------------------------
void smdlVertexObject::Animate( float i_fSimulationTime )
{
	if (!this->CheckDirty(i_fSimulationTime))
		return;

	// Base class (probably not needed here since no control anims)
	scAnimatableObject::Animate( i_fSimulationTime );

	if ( m_CurAnim )
	{
		float cur_frame = m_CurAnim->ComputeFrame(i_fSimulationTime);

		const int num_meshes = m_CurAnim->GetVertexKeys().size();
		DBG_ASSERT(num_meshes == m_Meshes.size(), "Num anim meshes not equal to object " << num_meshes << " != " << m_Meshes.size());
		if (num_meshes != m_Meshes.size())
			return;

		if (num_meshes == m_Meshes.size())
		{
			for (int m=0; m<num_meshes; m++)
			{
				const smdlVertexAnimKeys &keys = m_CurAnim->GetVertexKeys()[m];
				m_Meshes[m]->VertexTransformMesh(cur_frame, keys);
			}
		}
	}
}

//--------------------------------------------------------------------
// CheckAnimation checks compatibility of this animation 
// with the model.  It will return true if the animation can
// be played on this model.
//--------------------------------------------------------------------
//virtual 
bool smdlVertexObject::CheckAnimation( const anFrameAnimation& i_GeoAnimation ) const
{
	const smdlVertexAnimation *pVertexAnim = dynamic_cast<const smdlVertexAnimation*>( &i_GeoAnimation );
	if (pVertexAnim)
	{
		const int num_meshes = pVertexAnim->GetVertexKeys().size();
		if (num_meshes == m_Meshes.size())
		{
			for (int m=0; m<num_meshes; m++)
			{
				const smdlVertexAnimKeys &keys = pVertexAnim->GetVertexKeys()[m];
				if (!m_Meshes[m]->CheckAnimation(keys))
					return false;
			}
			return true;
		}
		else
		{
			DBG_WARNING("Number of anim meshes " << num_meshes << " not equal to object " << m_Meshes.size());
		}
	}
	return false;
}


//--------------------------------------------------------------------
//	SetAnimation makes the given animation into the current animation
//	(but does not take ownership of it; the client must preserve it
//	as long as the scAnimatableObject needs it).  Setting an animation
//	with this function will not blend it - just clobber the old one.
//	A pointer to the created animation instance is returned.
//--------------------------------------------------------------------
//virtual 
anFrameAnimInstance* smdlVertexObject::SetAnimation(
					const anFrameAnimation& i_Animation,
					float i_StartTime)
{
	m_bDirty = true;
	this->ClearAnimation();

	m_CurAnim = dynamic_cast<smdlVertexAnimInstance*>(
					i_Animation.CreateAnimInstance(i_StartTime));

	m_CurStartTime = i_StartTime;
	return m_CurAnim;
}

//--------------------------------------------------------------------
//	BlendAnimation causes a new animation to be blended to the current
//	animation over "i_BlendTime" length of time.  At the end of
//	i_BlendTime, the object's animation will be exactly the new
//	animation.
//	A pointer to the created animation instance is returned.
//--------------------------------------------------------------------
//virtual 
anFrameAnimInstance* smdlVertexObject::BlendAnimation(
						const anFrameAnimation& i_GeoAnimation,
						float i_StartTime,
						float i_BlendTime, 
						bool i_bSmoothBlend,
						float i_EaseInWeight, 
						float i_EaseOutWeight)
{
	//	no blending for vertex animation for now
	return this->SetAnimation(i_GeoAnimation, i_StartTime);
}

//--------------------------------------------------------------------
//	ClearAnimation causes all animation to stop.
//--------------------------------------------------------------------
//virtual 
void smdlVertexObject::ClearAnimation()
{
	m_bDirty = true;
	delete m_CurAnim;
	m_CurAnim = NULL;

}

//--------------------------------------------------------------------
//	Adds sub animation on top of base animation.
//	i_bPreserve - sets flag saying if the subanimation should
//		remain during changes to the full animation.
//--------------------------------------------------------------------
//virtual 
anFrameAnimInstance* smdlVertexObject::AddSubAnimation(
					const anFrameAnimation& i_GeoAnimation,
					float i_StartTime,
					bool i_bPreserve)
{
	return NULL;
}

//--------------------------------------------------------------------
//	Remove sub animation 
//--------------------------------------------------------------------
//virtual 
void smdlVertexObject::RemoveSubAnimation( anFrameAnimInstance* i_SubAnimInstance)
{

}

//--------------------------------------------------------------------
// SetSubAnimationBlend sets how the given subanimation should 
// blend with the base anim. Values from 0-1. The given subanimation
// should have been returned from a call to AddSubAnimation first.
//--------------------------------------------------------------------
//virtual 
void smdlVertexObject::SetSubAnimationBlend( const anFrameAnimInstance* i_SubAnim, 
											float i_Blend)
{

}

//--------------------------------------------------------------------
//	ClearAllSubAnimations causes all sub animations to stop.
//--------------------------------------------------------------------
//virtual 
void smdlVertexObject::ClearAllSubAnimations()
{

}

//--------------------------------------------------------------------
//	ClearSubAnimations causes non-preserved sub animations to stop.
//--------------------------------------------------------------------
//virtual 
void smdlVertexObject::ClearSubAnimations()
{

}

//--------------------------------------------------------------------
// CheckDirty - return true if the object needs to animate the model.
//		Clears the dirty bit so that if the time is different
//		it will be dirty next call.
//--------------------------------------------------------------------
bool smdlVertexObject::CheckDirty(float i_SimTime)
{
	if ((!m_bDirty) && (i_SimTime == m_LastAnimateTime))
		return false;

	m_bDirty = false;
	m_LastAnimateTime = i_SimTime;
	return true;
}

