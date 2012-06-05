/*****************************************************************************
**  entEntity.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/ent/entEntity.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/ent/entAnimation.hpp"
#include "Graphics/ent/entAnimInstance.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/sc/scAnimatableObject.hpp"

#include <algorithm>


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//void set_material_overide( g3dSceneNode* i_pNode,
	//							const std::vector<matMaterial*>& i_OrigMats,
	//							std::vector<matMaterial*>& i_UniqMats )
	//{
	//	const g3dFragment* pFrag = i_pNode->GetFragment();

	//	if( pFrag )
	//	{
	//		const matMaterial* orig_mat = pFrag->GetMaterial();

	//		// Find the index of the original material
	//		std::vector<matMaterial*>::const_iterator it =
	//				std::find( i_OrigMats.begin(), i_OrigMats.end(), orig_mat );

	//		if( it != i_OrigMats.end() )
	//		{
	//			// Get the new material at that index
	//			matMaterial* uniq_mat = i_UniqMats[ it - i_OrigMats.begin() ];

	//			// Set the material overide in the node
	//			i_pNode->SetMaterial( uniq_mat );
	//		}
	//	}

	//	// Repeat on the children
	//	std::vector<g3dSceneNode*> children = i_pNode->GetChildren();
	//	int nSize = children.size();
	//	for( int i = 0; i < nSize; ++i )
	//	{
	//		set_material_overide( children[i], i_OrigMats, i_UniqMats );
	//	}
	//}

}	// end of namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
entEntity::entEntity( scObject *i_pObject )
:	m_pObject(i_pObject),
	m_pCurAnimInstance(NULL),
	m_bAnimFinished( true ),
	m_bRenderable( true ),
	m_bActiveInRenderLayer( true ),
	m_bActiveInSceneMgr( true )
{
	m_pObject->GetBase()->UpdateTotalTransform();

	// Cast to animatable object, which might return NULL.
	// Entities can still handle objects that don't animate
	m_pAnimationObj = dynamic_cast<scAnimatableObject*>(m_pObject);
}

////--------------------------------------------------------------------
////--------------------------------------------------------------------
//entEntity::entEntity( const entModelTemplate& i_Template )
//:	m_Template(i_Template),
//	m_pObject(NULL),
//	m_pCurAnimInstance(NULL),
//	m_bAnimFinished( true ),
//	m_bRenderable( true )
//{
//	// Cast to entity template, which might return NULL.
//	// Entities can still handle objects that don't animate
//	m_pEntityTemplate = dynamic_cast<const entEntityTemplate*>(&i_Template);
//
//	m_pObject = entImport::CreateObject( i_Template );
//	m_pObject->GetBase()->UpdateTotalTransform();
//
//	// Cast to animatable object, which might return NULL.
//	// Entities can still handle objects that don't animate
//	m_pAnimationObj = dynamic_cast<scAnimatableObject*>(m_pObject);
//
//	//scObjectMgr::Add( m_pObject );
//	//
//	//if( i_pSceneParent )
//	//{
//	//	i_pSceneParent->AddChild( m_pObject->GetBase() );
//	//}
//}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
entEntity::~entEntity()
{
	delete m_pObject;
	//scObjectMgr::Destroy( m_pObject );

	//envSTLHelpers::DeleteContainer(m_UniqueMaterials);
}

//--------------------------------------------------------------------
//	update the total transform for the entity so the worldbox
//	will be updated.
//--------------------------------------------------------------------
const maAxisBox& entEntity::GetUpdatedWorldBox()
{
	m_pObject->GetBase()->UpdateTotalTransform();

	//const maAxisBox &box = m_pObject->GetBase()->GetWorldBox();
	//DBG_LOG3( "  Min( %8.3f, %8.3f, %8.3f )", box.GetMinX(), box.GetMinY(), box.GetMinZ() );
	//DBG_LOG3( "  Max( %8.3f, %8.3f, %8.3f )", box.GetMaxX(), box.GetMaxY(), box.GetMaxZ() );

	return m_pObject->GetBase()->GetWorldBox();
}

//--------------------------------------------------------------------
//  ComputeWorldBox
//--------------------------------------------------------------------
void entEntity::ComputeWorldBox(maAxisBox &o_Box) const
{
	m_pObject->GetBase()->UpdateTotalTransform();
	o_Box = m_pObject->GetBase()->GetWorldBox();

	//DBG_LOG3( "  Min( %8.3f, %8.3f, %8.3f )", o_Box.GetMinX(), o_Box.GetMinY(), o_Box.GetMinZ() );
	//DBG_LOG3( "  Max( %8.3f, %8.3f, %8.3f )", o_Box.GetMaxX(), o_Box.GetMaxY(), o_Box.GetMaxZ() );
}

//--------------------------------------------------------------------
//	GetWorldBox
//--------------------------------------------------------------------
const maAxisBox& entEntity::GetWorldBox() const
{
	return m_pObject->GetBase()->GetWorldBox();
}

//--------------------------------------------------------------------
//	SetPosition changes the position of the objects
//--------------------------------------------------------------------
void entEntity::SetPosition(const maPoint3d& i_Position)
{
	m_pObject->SetPosition(i_Position);
}

//--------------------------------------------------------------------
//	GetPosition returns the position of the object
//--------------------------------------------------------------------
const maPoint3d& entEntity::GetPosition() const
{
	return m_pObject->GetPosition();
}

//--------------------------------------------------------------------
//	SetOrientation changes the orientation of the objects
//--------------------------------------------------------------------
void entEntity::SetOrientation(const maRotation& i_Orientation)
{
	m_pObject->SetOrientation(i_Orientation);
}

//--------------------------------------------------------------------
//	GetOrientation returns the orientation of the object
//--------------------------------------------------------------------
const maRotation& entEntity::GetOrientation() const
{
	return m_pObject->GetOrientation();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void entEntity::SetPositionAndOrientation(	const maPoint3d& i_Position,
											const maRotation& i_Orientation)
{
	m_pObject->SetPositionAndOrientation(i_Position, i_Orientation);
}

//--------------------------------------------------------------------
//	SetScale changes the scale of the object
//--------------------------------------------------------------------
void entEntity::SetScale(const maPoint3d& i_Scale)
{
	m_pObject->SetScale(i_Scale);
}

//--------------------------------------------------------------------
//	GetScale returns the scale of the object
//--------------------------------------------------------------------
const maPoint3d& entEntity::GetScale() const
{
	return m_pObject->GetScale();
}

//--------------------------------------------------------------------
//	GetMatrix returns the matrix
//--------------------------------------------------------------------
void entEntity::GetMatrix( maMatrix4x4& o_Matrix ) const
{
	o_Matrix = m_pObject->GetBase()->GetTransform();
}

//--------------------------------------------------------------------
//	Think allows the prop to do maintenance of it's animations and
//	stuff.
//--------------------------------------------------------------------
void entEntity::Think(float i_SimulationTime)
{
	// The animation is done when it is not looping and
	// the final frame is played and
	if( m_pCurAnimInstance &&
		!m_pCurAnimInstance->GetAnim().GetLooping())
	{
		float end_frame = m_pCurAnimInstance->GetAnim().GetEndFrame();
		if (end_frame < 0)
			end_frame = m_pCurAnimInstance->GetAnim().GetNumFrames();
		if (m_pCurAnimInstance->ComputeFrame(i_SimulationTime ) >= end_frame )
		{
			m_bAnimFinished = true;
		}
	}

}

//--------------------------------------------------------------------
// DoesAnimationExist
//--------------------------------------------------------------------
//bool entEntity::DoesAnimationExist(int i_AnimIndex)
//{
//	if (!m_pEntityTemplate) return false;
//
//	// Look up anim by index from template
//	//
//	if (i_AnimIndex < m_pEntityTemplate->GetNumAnimations())
//	{
//		const entAnimation* dst_anim = m_pEntityTemplate->GetAnimation(i_AnimIndex);
//		if ( dst_anim )
//		{
//			return true;
//		}
//	}
//
//	return false;
//}

//--------------------------------------------------------------------
// CheckAnimation checks compatibility of this animation 
// with the model. It will return true if the animation can
// be played on this model.
//--------------------------------------------------------------------
bool entEntity::CheckAnimation( const entAnimation* i_pAnimation ) const
{
	if ( m_pAnimationObj && i_pAnimation )
	{
		return m_pAnimationObj->CheckAnimation(*i_pAnimation);
	}
	return false;
}

//--------------------------------------------------------------------
// SetAnimation by slot in entity template
//--------------------------------------------------------------------
//void entEntity::SetAnimation(int i_AnimIndex, float i_SimulationTime)
//{
//	if (!m_pEntityTemplate) return;
//
//	// Look up anim by index from template and set by pointer
//	//
//	const entAnimation* dst_anim = m_pEntityTemplate->GetAnimation(i_AnimIndex);
//	if ( dst_anim )
//	{
//		this->SetAnimation(	dst_anim, i_SimulationTime, dst_anim->GetTransitionTime() );
//	}
//	else
//	{
//		this->SetAnimation(	dst_anim, i_SimulationTime );
//	}
//}

//--------------------------------------------------------------------
// SetAnimation to given animation pointer.
//--------------------------------------------------------------------
void entEntity::SetAnimation(const entAnimation* i_BlendTo, float i_SimulationTime)
{
	if ( i_BlendTo )
		SetAnimation( i_BlendTo, i_SimulationTime, i_BlendTo->GetTransitionTime() );
	else
		SetAnimation( i_BlendTo, i_SimulationTime, 0.0f );
}

//--------------------------------------------------------------------
// SetAnimation to given animation pointer with the given blend time
//--------------------------------------------------------------------
void entEntity::SetAnimation(const entAnimation* i_BlendTo, float i_SimulationTime, float i_BlendTime)
{
	//	set animation in object list
	//
	if ( m_pAnimationObj )
	{
		if ( i_BlendTo )
		{
			anFrameAnimInstance *instance = (i_BlendTime > 0.0f) ?
				m_pAnimationObj->BlendAnimation(*i_BlendTo, i_SimulationTime, i_BlendTime) :
				m_pAnimationObj->SetAnimation(*i_BlendTo, i_SimulationTime);

			m_pCurAnimInstance = dynamic_cast<entAnimInstance*>(instance);
			m_bAnimFinished = false;
		}
		else
		{
			m_pAnimationObj->ClearAnimation();
			m_pCurAnimInstance = NULL;
			m_bAnimFinished = true;
		}
	}

}

//--------------------------------------------------------------------
// BlendAnimation sets two animations pointers at once to blend
//	them for a certain amount of time
//--------------------------------------------------------------------
void entEntity::BlendAnimation( const entAnimation* i_Anim1, float i_StartTime1,
						const entAnimation* i_Anim2, float i_StartTime2,
						float i_fBlendTime, bool i_bSmoothBlend,
					    float i_EaseInWeight, float i_EaseOutWeight )
{
	if ( m_pAnimationObj )
	{
		// Clear out everything first
		ClearAnimation();

		// Set first base animation
		m_pAnimationObj->SetAnimation(*i_Anim1, i_StartTime1);

		// Blend in second
		anFrameAnimInstance *instance =
			m_pAnimationObj->BlendAnimation(*i_Anim2, i_StartTime2, i_fBlendTime,
										 i_bSmoothBlend, i_EaseInWeight, i_EaseOutWeight);
		m_pCurAnimInstance = dynamic_cast<entAnimInstance*>(instance);
		m_bAnimFinished = false;
	}
}

//--------------------------------------------------------------------
// ClearAnimation stops entity from animating
//--------------------------------------------------------------------
void entEntity::ClearAnimation()
{
	if ( m_pAnimationObj )
	{
		m_pAnimationObj->ClearAnimation();
		m_pAnimationObj->ClearSubAnimations();
		m_pCurAnimInstance = NULL;
		m_bAnimFinished = true;
	}
}

//--------------------------------------------------------------------
//	ClearSubAnimations causes non-preserved sub animations to stop.
//--------------------------------------------------------------------
void entEntity::ClearSubAnimations()
{
	if ( m_pAnimationObj )
	{
		m_pAnimationObj->ClearSubAnimations();
	}
}

//--------------------------------------------------------------------
// AddSubAnimation adds this animation on top of base animation.
// Returns a handle that is not owned by the caller.
//--------------------------------------------------------------------
entSubAnimation* entEntity::AddSubAnimation( const entAnimation* i_SubAnim, 
											 float i_StartTime,
											 bool i_bPreserve)
{
	if ( m_pAnimationObj )
	{
		anFrameAnimInstance *anim_inst = m_pAnimationObj->AddSubAnimation(*i_SubAnim, i_StartTime, i_bPreserve);
		return anim_inst; // typedef'ed to entSubAnimation
	}
	return NULL;
}


//--------------------------------------------------------------------
// RemoveSubAnimation removes currently running subanimation.
// The given pointer should have been returned from a previous call 
// to AddSubAnimation. Notice that ClearAnimation will remove all
// sub animations automatically.
//--------------------------------------------------------------------
void entEntity::RemoveSubAnimation(entSubAnimation* i_pSubAnim)
{
	if ( m_pAnimationObj )
	{
		m_pAnimationObj->RemoveSubAnimation(i_pSubAnim);
	}
}

//--------------------------------------------------------------------
// SetSubAnimationBlend sets how the given subanimation should 
// blend with the base anim. Values from 0-1. The given subanimation
// should have been returned from a call to AddSubAnimation first.
//--------------------------------------------------------------------
void entEntity::SetSubAnimationBlend( const entSubAnimation* i_SubAnim, 
									  float i_Blend)
{
	if ( m_pAnimationObj )
	{
		m_pAnimationObj->SetSubAnimationBlend(i_SubAnim, i_Blend);
	}
}

//--------------------------------------------------------------------
// SetRenderable sets the rendering on all the objects in the entity
//--------------------------------------------------------------------
void entEntity::SetRenderable( bool i_bRenderable )
{
	// nothing to do
	if ( i_bRenderable == m_bRenderable)
		return;

	m_bRenderable = i_bRenderable;
	m_pObject->SetRenderable(i_bRenderable);
}

//--------------------------------------------------------------------
//	ActiveInRenderLayer sets whether the object is visible
//	in render layer
//--------------------------------------------------------------------
void entEntity::SetActiveInRenderLayer(bool i_bRenderable)
{
	if (i_bRenderable == m_bActiveInRenderLayer)
		return;

	m_bActiveInRenderLayer = i_bRenderable;
	m_pObject->SetActiveInRenderLayer(i_bRenderable);
}

bool entEntity::GetActiveInRenderLayer() const
{
	return m_bActiveInRenderLayer;
}

//--------------------------------------------------------------------
//	ActiveInRenderLayer sets whether the object is visible
//	in scene manager
//--------------------------------------------------------------------
void entEntity::SetActiveInSceneMgr(bool i_bRenderable)
{
	if (i_bRenderable == m_bActiveInSceneMgr)
		return;

	m_bActiveInSceneMgr = i_bRenderable;
	m_pObject->SetActiveInSceneMgr(i_bRenderable);
}

bool entEntity::GetActiveInSceneMgr() const
{
	return m_bActiveInSceneMgr;
}

//--------------------------------------------------------------------
//  GetNamedNode returns a pointer to the node with the given name
//  or NULL if it does not exist.
//--------------------------------------------------------------------
const g3dSceneNode* entEntity::GetNamedNode( const char* i_Name ) const
{
	return m_pObject->GetBase()->GetNamedNode( i_Name );
}

g3dSceneNode* entEntity::GetNamedNode( const char* i_Name )
{
	return m_pObject->GetBase()->GetNamedNode( i_Name );
}

//--------------------------------------------------------------------
//	GetUniqueMaterials returns the list of unique materials
//  used by this object.  Unique materials will be
//	created the first time this is called.
//--------------------------------------------------------------------
//std::vector<matMaterial*>& entEntity::GetUniqueMaterials()
//{
//	if (m_UniqueMaterials.empty())
//		create_unique_materials();
//	return m_UniqueMaterials;
//}
//const std::vector<matMaterial*>& entEntity::GetUniqueMaterials() const
//{
//	if (m_UniqueMaterials.empty())
//		create_unique_materials();
//	return m_UniqueMaterials;
//}

//--------------------------------------------------------------------
// private - creates a list of materials unique to this
//	instance so that material events can happen to
//	this entity without affecting other entities
//	using the same template.
//--------------------------------------------------------------------
//void entEntity::create_unique_materials() const
//{
//	DBG_ASSERT(m_UniqueMaterials.empty(), "Already created unique materials");
//
//	const std::vector<matMaterial*> &mat_list = m_Template.GetMaterials();
//
//	int num_mats = mat_list.size();
//	m_UniqueMaterials.resize(num_mats);
//	for (int i=0; i<num_mats; i++)
//	{
//		// Let copy constructor make clone
//		m_UniqueMaterials[i] = new matMaterial(*mat_list[i]);
//	}
//
//	set_material_overide( m_pObject->GetBase(), mat_list, m_UniqueMaterials );
//}