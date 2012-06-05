/*****************************************************************************
**	scAnimatableLODObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Graphics/sc/scAnimatableLODObject.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"


//--------------------------------------------------------------------
//	The object will initially
//	be placed at the origin with unit scale and no rotation.
//--------------------------------------------------------------------
scAnimatableLODObject::scAnimatableLODObject()
:	m_CurrentObjectIndex(0)
{
	m_Objects.resize(0);

	scObject::GetBase()->SetRenderable( true );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
scAnimatableLODObject::~scAnimatableLODObject()
{
	//	from ~scOjbect()
	//if( m_pBase->GetParent() )
	//{
	//	m_pBase->GetParent()->RemoveChild(m_pBase);
	//}
	//delete m_pBase;
	//envSTLHelpers::DeleteContainer(m_ControlAnims);
}

//--------------------------------------------------------------------
// CheckAnimation checks compatibility of this animation 
// with the model. It will return true if the animation can
// be played on this model.
//--------------------------------------------------------------------
//virtual 
bool scAnimatableLODObject::CheckAnimation( const anFrameAnimation& i_GeoAnimation ) const
{
	// True if one of the objects can animate, or only if all?
	for (int i = 0 ; i < m_Objects.size() ; i++ )
	{
		if (m_Objects[i].m_pObject->CheckAnimation( i_GeoAnimation ))
			return true;
	}

	return false;
}

//--------------------------------------------------------------------
//	SetAnimation makes the given animation into the current animation
//	(but does not take ownership of it; the client must preserve it
//	as long as the scAnimatableLODObject needs it).  Setting an animation
//	with this function will not blend it - just clobber the old one.
//	A pointer to the created animation instance is returned.
//--------------------------------------------------------------------
anFrameAnimInstance* scAnimatableLODObject::SetAnimation(
											const anFrameAnimation& i_GeoAnimation,
											float i_StartTime )
{
	// TODO: this needs to be "joined" somehow

	anFrameAnimInstance* anFAI;

	int i;
	for ( i = 0 ; i < m_Objects.size() ; i++ )
	{
		anFAI = m_Objects[i].m_pObject->SetAnimation( i_GeoAnimation, i_StartTime );
	}

	return anFAI;
}

//--------------------------------------------------------------------
//	BlendAnimation causes a new animation to be blended to the current
//	animation over "i_BlendTime" length of time.  At the end of
//	i_BlendTime, the object's animation will be exactly the new
//	animation.
//	A pointer to the created animation instance is returned.
//--------------------------------------------------------------------
anFrameAnimInstance* scAnimatableLODObject::BlendAnimation(
						const anFrameAnimation& i_GeoAnimation,
						float i_StartTime,
						float i_BlendTime, 
						bool i_bSmoothBlend,
						float i_EaseInWeight, 
						float i_EaseOutWeight)
{
	// TODO: this needs to be "joined" somehow

	anFrameAnimInstance* anFAI;

	int i;
	for ( i = 0 ; i < m_Objects.size() ; i++ )
	{
		anFAI = m_Objects[i].m_pObject->BlendAnimation( i_GeoAnimation, i_StartTime, i_BlendTime,
										i_bSmoothBlend, i_EaseInWeight, i_EaseOutWeight);
	}

	return anFAI;
}

//--------------------------------------------------------------------
//	ClearAnimation causes all animation to stop.
//--------------------------------------------------------------------
void scAnimatableLODObject::ClearAnimation()
{
	int i;
	for ( i = 0 ; i < m_Objects.size() ; i++ )
	{
		m_Objects[i].m_pObject->ClearAnimation();
	}
}

//--------------------------------------------------------------------
//	Adds sub animation on top of base animation
//--------------------------------------------------------------------
anFrameAnimInstance* scAnimatableLODObject::AddSubAnimation(
					const anFrameAnimation& i_GeoAnimation,
					float i_StartTime,
					bool i_bPreserve)
{
	// TODO: this needs to be "joined" somehow

	anFrameAnimInstance* anFAI;

	int i;
	for ( i = 0 ; i < m_Objects.size() ; i++ )
	{
		anFAI = m_Objects[i].m_pObject->AddSubAnimation( i_GeoAnimation, i_StartTime, i_bPreserve );
	}

	return anFAI;
}

//--------------------------------------------------------------------
//	Remove sub animation 
//--------------------------------------------------------------------
void scAnimatableLODObject::RemoveSubAnimation( anFrameAnimInstance* i_SubAnimInstance )
{
	//TODO: this needs to be related to the return value of AddSubAnimation
	for (int i = 0 ; i < m_Objects.size() ; i++ )
	{
		m_Objects[i].m_pObject->RemoveSubAnimation( i_SubAnimInstance );
	}
}

//--------------------------------------------------------------------
// SetSubAnimationBlend sets how the given subanimation should 
// blend with the base anim. Values from 0-1. The given subanimation
// should have been returned from a call to AddSubAnimation first.
//--------------------------------------------------------------------
void scAnimatableLODObject::SetSubAnimationBlend( const anFrameAnimInstance* i_SubAnim, 
					float i_Blend )
{
	//TODO:  If the above function doesn't combine them, this won't work either.
	int i;
	for ( i = 0 ; i < m_Objects.size() ; i++ )
	{
		m_Objects[i].m_pObject->SetSubAnimationBlend( i_SubAnim, i_Blend );
	}
}

//--------------------------------------------------------------------
//	ClearAllSubAnimations causes all sub animations to stop.
//--------------------------------------------------------------------
void scAnimatableLODObject::ClearAllSubAnimations()
{
	int i;
	for ( i = 0 ; i < m_Objects.size() ; i++ )
	{
		m_Objects[i].m_pObject->ClearAllSubAnimations();
	}
}
//--------------------------------------------------------------------
//	ClearSubAnimations causes non-preserved sub animations to stop.
//--------------------------------------------------------------------
void scAnimatableLODObject::ClearSubAnimations()
{
	int i;
	for ( i = 0 ; i < m_Objects.size() ; i++ )
	{
		m_Objects[i].m_pObject->ClearSubAnimations();
	}
}

//--------------------------------------------------------------------
//	Add the object at the specified index.  if the index is -1 then
//	the object will be appended on the end.
//--------------------------------------------------------------------
void scAnimatableLODObject::AddObject( scAnimatableObject* i_pObject, float i_fDistance, bool i_bDefault, int i_Index )
{
	DBG_ASSERT( i_pObject != 0 , "Cannot add a NULL object" );
	if (!i_pObject)
		return;

	//	add the object to the list
	//
	int index = i_Index;
	if ( index == -1 )
	{
		index = m_Objects.size();
		m_Objects.resize( index + 1 );
	}
	else
	{
		DBG_ASSERT( index < m_Objects.size(), "Index out of range" );
		if (index >= m_Objects.size())
			return;
	}

	m_Objects[index].m_pObject = i_pObject;

	//	add the object to the base scene node
	//
	g3dSceneNode* pNode = i_pObject->GetBase();

	m_Objects[index].m_pBase		= pNode;
	m_Objects[index].m_bDefault		= i_bDefault;
	m_Objects[index].m_fDistance	= i_fDistance;

	scObject::GetBase()->AddChild( pNode );

	pNode->SetRenderable( false );

	if ( m_Objects.size() == 1 )			// TODO: temporary?
		pNode->SetRenderable( true );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void scAnimatableLODObject::RemoveObject( scAnimatableObject* i_pObject )
{
	DBG_ASSERT( i_pObject != 0 , "Cannot remove a NULL object" );
	if (!i_pObject)
		return;

	int index = 0;
	std::vector<level_of_detail_object>::iterator it = m_Objects.begin();
	for (; it != m_Objects.end(); ++it, ++index)
	{
		if ( m_Objects[index].m_pObject == i_pObject )
		{
			//	remove the object to the base scene node
			//
			scObject::GetBase()->RemoveChild( m_Objects[index].m_pBase );

			delete m_Objects[index].m_pObject;
			m_Objects[index].m_pObject = 0;
			break;
		}
	}
}

//--------------------------------------------------------------------
//	replace the object at the passed in index.  this function will
//	DELETE the old object.
//--------------------------------------------------------------------
//virtual 
void scAnimatableLODObject::ReplaceObject( int i_Index, scAnimatableObject* i_pObject, float i_fDistance, bool i_bDefault )
{
	DBG_ASSERT( i_Index < m_Objects.size(), "Index out of range" );
	if (i_Index >= m_Objects.size())
		return;

	if ( m_Objects[i_Index].m_pObject != 0 )
	{
		RemoveObject( m_Objects[i_Index].m_pObject );
		delete m_Objects[i_Index].m_pObject;
		m_Objects[i_Index].m_pObject = 0;
	}

	AddObject( i_pObject, i_fDistance, i_bDefault, i_Index );
}

//--------------------------------------------------------------------
//	check the origin point against the object's point to see if 
//	it should change it's LOD.
//--------------------------------------------------------------------
void scAnimatableLODObject::UpdateLOD( const maPoint3d& i_OriginPoint )
{
	DBG_ASSERT( m_Objects[m_CurrentObjectIndex].m_pObject != 0, "Invalid object" );
	if (m_Objects[m_CurrentObjectIndex].m_pObject == 0)
		return;

	float dist;
	dist = (m_Objects[m_CurrentObjectIndex].m_pObject->GetPosition() - i_OriginPoint).Length();

	//DBG_LOG3( "Cam (%6.3f, %6.3f, $6.3f)", i_OriginPoint.GetX(), i_OriginPoint.GetY(), i_OriginPoint.GetZ() );
	//DBG_LOG3( "Obj (%6.3f, %6.3f, $6.3f)", m_Objects[m_CurrentObjectIndex].m_pObject->GetPosition().GetX(), m_Objects[m_CurrentObjectIndex].m_pObject->GetPosition().GetY(), m_Objects[m_CurrentObjectIndex].m_pObject->GetPosition().GetZ() );
	//DBG_LOG1( "     Distance = %6.3f", dist );

	// based on the distance choose the correct model
	//
	int i, newindex;
	for ( i = m_Objects.size() - 1 ; i >= 0 ; --i )
	{
		if ( m_Objects[i].m_fDistance <= dist )
		{
			newindex = i;
			break;
		}
	}

	if ( newindex != m_CurrentObjectIndex )
	{
		//DBG_LOG2( "Animatable LOD change from %d to %d", m_CurrentObjectIndex, newindex );

		m_Objects[m_CurrentObjectIndex].m_pBase->SetRenderable( false );
		m_Objects[newindex].m_pBase->SetRenderable( true );

		m_CurrentObjectIndex = newindex;
	}
}

