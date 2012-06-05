/*****************************************************************************
**	scAnimatableLODObject.hpp
**
**		scAnimatableLODObject is an abstract base for Level of Detail 
**	scene objects which can be animated from frame animation.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef SC_ANIMATABLELODOBJECT_HPP
#error scAnimatableLODObject.hpp multiply included
#endif
#define SC_ANIMATABLELODOBJECT_HPP

#ifndef AN_FRAMEANIMATION_HPP
#include "Graphics/an/anFrameAnimation.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef SC_ANIMATABLEOBJECT_HPP
#include "Graphics/sc/scAnimatableObject.hpp"
#endif


//============================================================================
//============================================================================
class scAnimatableLODObject : public scAnimatableObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		scAnimatableLODObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~scAnimatableLODObject();

		//--------------------------------------------------------------------
		// CheckAnimation checks compatibility of this animation 
		// with the model. It will return true if the animation can
		// be played on this model.
		//--------------------------------------------------------------------
		virtual bool CheckAnimation( const anFrameAnimation& i_GeoAnimation ) const;

		//--------------------------------------------------------------------
		//	SetAnimation makes the given animation into the current animation
		//	(but does not take ownership of it; the client must preserve it
		//	as long as the scAnimatableLODObject needs it).  Setting an animation
		//	with this function will not blend it - just clobber the old one.
		//	A pointer to the created animation instance is returned.
		//--------------------------------------------------------------------
		virtual anFrameAnimInstance* SetAnimation(
													const anFrameAnimation& i_GeoAnimation,
													float i_StartTime );

		//--------------------------------------------------------------------
		//	BlendAnimation causes a new animation to be blended to the current
		//	animation over "i_BlendTime" length of time.  At the end of
		//	i_BlendTime, the object's animation will be exactly the new
		//	animation.
		//	A pointer to the created animation instance is returned.
		//--------------------------------------------------------------------
		virtual anFrameAnimInstance* BlendAnimation(
								const anFrameAnimation& i_GeoAnimation,
								float i_StartTime,
								float i_BlendTime, 
								bool i_bSmoothBlend = false,
								float i_EaseInWeight = 1.0f, 
								float i_EaseOutWeight = 1.0f);

		//--------------------------------------------------------------------
		//	ClearAnimation causes all animation to stop.
		//--------------------------------------------------------------------
		virtual void ClearAnimation();

		//--------------------------------------------------------------------
		//	Adds sub animation on top of base animation
		//--------------------------------------------------------------------
		virtual anFrameAnimInstance* AddSubAnimation(
							const anFrameAnimation& i_GeoAnimation,
							float i_StartTime,
							bool i_bPreserve);

		//--------------------------------------------------------------------
		//	Remove sub animation 
		//--------------------------------------------------------------------
		virtual void RemoveSubAnimation( anFrameAnimInstance* i_SubAnimInstance );

		//--------------------------------------------------------------------
		// SetSubAnimationBlend sets how the given subanimation should 
		// blend with the base anim. Values from 0-1. The given subanimation
		// should have been returned from a call to AddSubAnimation first.
		//--------------------------------------------------------------------
		virtual void SetSubAnimationBlend( const anFrameAnimInstance* i_SubAnim, 
							float i_Blend );

		//--------------------------------------------------------------------
		//	ClearAllSubAnimations causes all sub animations to stop.
		//--------------------------------------------------------------------
		virtual void ClearAllSubAnimations();

		//--------------------------------------------------------------------
		//	ClearSubAnimations causes non-preserved sub animations to stop.
		//--------------------------------------------------------------------
		virtual void ClearSubAnimations();

		//--------------------------------------------------------------------
		//	Add the object at the specified index.  if the index is -1 then
		//	the object will be appended on the end.
		//--------------------------------------------------------------------
		virtual void AddObject( scAnimatableObject* i_pObject, float i_fDistance, bool i_bDefault, int i_Index = -1 );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void RemoveObject( scAnimatableObject* i_pObject );

		//--------------------------------------------------------------------
		//	replace the object at the passed in index.  this function will
		//	DELETE the old object.
		//--------------------------------------------------------------------
		virtual void ReplaceObject( int i_Index, scAnimatableObject* i_pObject, float i_fDistance, bool i_bDefault );

		//--------------------------------------------------------------------
		//	GetBase returns the scene node representing the root of the
		//	object.  This is also the node which affects visibility of the
		//	object, and positioning of the object.
		//--------------------------------------------------------------------
		virtual inline const g3dSceneNode* GetBase() const;
		virtual inline g3dSceneNode* GetBase();

		//--------------------------------------------------------------------
		//	check the origin point against the object's point to see if 
		//	it should change it's LOD.
		//--------------------------------------------------------------------
		void UpdateLOD( const maPoint3d& i_OriginPoint );

	private:
		struct level_of_detail_object
		{
			scAnimatableObject* m_pObject;
			g3dSceneNode*		m_pBase;
			bool				m_bDefault;
			float				m_fDistance;
		};

		int	m_CurrentObjectIndex;
		std::vector<level_of_detail_object>	m_Objects;
};


//--------------------------------------------------------------------
//	GetBase returns the scene node representing the root of the
//	object.  This is also the node which affects visibility of the
//	object, and positioning of the object.
//--------------------------------------------------------------------
inline const g3dSceneNode* scAnimatableLODObject::GetBase() const
{
	return scObject::GetBase();
}

inline g3dSceneNode* scAnimatableLODObject::GetBase()
{
	return scObject::GetBase();
}
