/*****************************************************************************
**	tmlnChannelAnimationFull.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnChannelAnimationFull.hpp"

#include "Graphics/ent/entAnimation.hpp"
#include "Graphics/ent/entAnimInstance.hpp"
#include "Graphics/ent/entEntity.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelAnimationFull::tmlnChannelAnimationFull( const char* i_Name, api3dObjectEntity* i_pObject )
: tmlnChannel(i_Name), 
	m_pProp(i_pObject), 
	m_pAnimation(NULL), 
	m_AnimTime(0), 
	m_bBlending(false)
{
}

//--------------------------------------------------------------------
//  Test to see if this animation is compatible with the 
//	model attached to this channel. Returns true if compatible.
//--------------------------------------------------------------------
bool tmlnChannelAnimationFull::CheckAnimation( entAnimation* i_pAnimation )
{
	return m_pProp->GetEntity()->CheckAnimation( i_pAnimation );
}

//--------------------------------------------------------------------
//  Set the animation by name
//--------------------------------------------------------------------
//virtual
void  tmlnChannelAnimationFull::SetAnimation( entAnimation* i_pAnimation, float i_SimulationTime )
{
	//	set the animation
	if (   m_bBlending 
		|| (i_pAnimation != m_pAnimation) 
		|| (m_AnimTime != i_SimulationTime) )
	{
		m_pProp->GetEntity()->SetAnimation( i_pAnimation, i_SimulationTime, 0.0f );
	}

	m_pAnimation = i_pAnimation;
	m_AnimTime = i_SimulationTime;
	m_bBlending = false;

	// The user may be altering the frame rate while the
	// animation is currently playing, so we need to
	// make sure the anim_instance is always updated.
	entAnimInstance* pAnimInstance = m_pProp->GetEntity()->GetAnimInstance();
	if (pAnimInstance && m_pAnimation)
	{
		pAnimInstance->SetFrameRate( m_pAnimation->GetFrameRate() );
	}

	// Even if we didn't reset the animation above, we need to clear out the non-preserved
	// subanimations so that the channels don't add them in over and over.
	m_pProp->GetEntity()->ClearSubAnimations();
}

//--------------------------------------------------------------------
//  Blend from current anim to the given index
//--------------------------------------------------------------------
//virtual
void  tmlnChannelAnimationFull::BlendAnimation( entAnimation* i_pAnimation, 
											    float i_SimulationTime, 
											    float i_BlendTime,
											    bool i_bSmoothBlend,
											    float i_EaseInWeight,
											    float i_EaseOutWeight )
{
	//	check to see if we are blending between two animations
	if (m_pAnimation != 0 && m_pAnimation != i_pAnimation)
	{
		m_pProp->GetEntity()->BlendAnimation(m_pAnimation, m_AnimTime, i_pAnimation, 
											 i_SimulationTime, i_BlendTime, i_bSmoothBlend,
											 i_EaseInWeight, i_EaseOutWeight);
		m_bBlending = true;
	}
	else
	{
		SetAnimation(i_pAnimation, i_SimulationTime);
	}

	// The user may be altering the frame rate while the
	// animation is currently playing, so we need to
	// make sure the anim_instance is always updated.
	entAnimInstance* pAnimInstance = m_pProp->GetEntity()->GetAnimInstance();
	if (pAnimInstance && m_pAnimation)
	{
		pAnimInstance->SetFrameRate( m_pAnimation->GetFrameRate() );
	}
}


//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void tmlnChannelAnimationFull::Reset()
{
	//DBG_LOG( "ClearAnimation - Reset() channel " );
	m_pProp->GetEntity()->ClearAnimation();

	//bga - there is no sense of an idle animation anymore, that
	// was something that was loaded from CHD files. The new expression
	// code doesn't use that.
	//
	//// If we have an idle animation loaded into "0" slot in template, 
	//// use this in the Reset() case.
	//if (m_pProp->GetEntity()->DoesAnimationExist(0))
	//{
	//	const int idle_anim_index = 0;
	//	m_pProp->GetEntity()->SetAnimation(idle_anim_index, 0.0f);
	//}
	
	m_pAnimation = NULL;
	m_AnimTime = 0;
}


//--------------------------------------------------------------------
//  Force through an immediate Animate() call to the entity
//  that is animating. This updates the nodes so that
//  attchments can use the updated matrices.
//--------------------------------------------------------------------
void tmlnChannelAnimationFull::Animate(float i_Time)
{
	// Force through animation now, this gets the nodes updated earlier
	// so that the attachments can get up to date data.
	m_pProp->GetEntity()->Object()->AnimateMatrices(i_Time);

	// This would get expensive, I am changing the NodeReference
	// so that it sums the Transforms instead of using the
	// GetTotalTransform() function
	//m_pProp->GetEntity()->Object()->GetBase()->UpdateTotalTransform();

}
