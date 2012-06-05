/*****************************************************************************
**	smdlSubAnimation.cpp
**
**		smdlSubAnimation is a helper class for managing currently running
**	sub-animation instances.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlSubAnimation.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/smdl/smdlGeoFrameAnimation.hpp"


//--------------------------------------------------------------------
// This object takes over ownership of the animation instance
//--------------------------------------------------------------------
smdlSubAnimation::smdlSubAnimation(smdlGeoFrameAnimInstance* i_pAnimInstance)
:	m_pAnimInstance(i_pAnimInstance),
	m_Blend(1.0f),
	m_bPreserve(false)
{
	// the sub animation is additive if the loaded animation data
	// are deltas (if it was exported as an additive animation)
	m_bAdditive = i_pAnimInstance->GetAnim().GetAdditiveAnimation();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlSubAnimation::~smdlSubAnimation()
{
	delete m_pAnimInstance;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlSubAnimationIterator::smdlSubAnimationIterator(const std::string& i_RootName, 
			const smdlTree<smdlGeoAnimKeys>& i_AnimKeys,
			float i_CurrentFrame,
			g3dSceneNode* i_pRootNode,
			float i_Blend,
			bool i_bIsAdditive)
:	//m_AnimInstance(i_AnimInstance),
	m_bIsAttached(false),
	m_OffEnd(0),
	m_CurFrame(i_CurrentFrame),
	m_RootName(i_RootName),
	m_RootIterator(i_AnimKeys.GetIterator()),
	m_Iterator(i_AnimKeys.GetIterator()),	
	m_Blend(i_Blend),
	m_bIsAdditive(i_bIsAdditive)
	//m_bIsBlend(m_AnimInstance.GetBlend()),
	//m_bIsAdditive(m_AnimInstance.GetAdditive())
{
	//m_CurFrame = i_AnimInstance.GetAnimInstance()->ComputeFrame(i_SimulationTime);
	//m_RootName = i_AnimInstance.GetAnimInstance()->GetAnim().GetNameOfRoot();

	// If there is no root name, then just attach now to root of hierarchy
	if (m_RootName.empty())
	{
		m_bIsAttached = true;
		//m_Iterator = m_AnimInstance.GetAnimInstance()->GetKeys().GetIterator();
	}
	else if (i_pRootNode)
	{
		// Check to see if the root name of the animation is the 
		// same as the root node of the hierarchy
		if (i_pRootNode->CompareName(m_RootName.c_str()))
		{
			m_bIsAttached = true;
		}
	}
}

//--------------------------------------------------------------------
// move down in the hierarchy to the child with the given index.
// pass in the scene node that is the child in the hierarchy so
// that the iterator knows when to attach to the hierarchy.
//--------------------------------------------------------------------
void smdlSubAnimationIterator::MoveToChild(int i_Num, g3dSceneNode& i_Node)
{
	if (m_bIsAttached)
	{
		// If we don't have a child to go to, then we move into
		// the "OffEnd" state and just count how many moves to child and parent
		// until we match back up with our hierarchy again.
		//
		if (m_OffEnd > 0 || (i_Num >= m_Iterator.NumChildren()))
			m_OffEnd++;
		else
			m_Iterator.MoveToChild(i_Num);

	}
	else if (i_Node.CompareName(m_RootName.c_str()))
	{
		//m_Iterator = m_AnimInstance.GetAnimInstance()->GetKeys().GetIterator();
		m_Iterator = m_RootIterator;
		m_bIsAttached = true;
	}
}

//--------------------------------------------------------------------
// Move up in the hierarchy
//--------------------------------------------------------------------
void smdlSubAnimationIterator::MoveToParent()
{
	if (m_bIsAttached)
	{
		// If off end, subtract our depth
		if (m_OffEnd > 0)
			m_OffEnd--;
		// If we have a parent, move up. Otherwise, detach from hierarchy
		else if (m_Iterator.HasParent())
			m_Iterator.MoveToParent();
		else
			m_bIsAttached = false;
	}
}

//--------------------------------------------------------------------
// Return key data in iterator if attached, otherwise return NULL.
//--------------------------------------------------------------------
const smdlGeoAnimKeys* smdlSubAnimationIterator::GetData() const
{
	if (this->IsAttached())
	{
		return &(m_Iterator.GetData());
	}
	return NULL;
}

