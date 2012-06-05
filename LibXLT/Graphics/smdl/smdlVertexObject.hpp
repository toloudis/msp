/*****************************************************************************
**	smdlVertexObject.hpp
**
**		smdlVertexObject represents an object which is made up of
**	multiple fragments with baked vertex animation.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_VERTEXOBJECT_HPP
#error smdlVertexObject.hpp multiply included
#endif
#define SMDL_VERTEXOBJECT_HPP

#ifndef MDL_FRAGINFO_HPP
#include "Graphics/mdl/mdlFragInfo.hpp"
#endif
#ifndef SC_ANIMATABLEOBJECT_HPP
#include "Graphics/sc/scAnimatableObject.hpp"
#endif


//============================================================================
//============================================================================
class tmeshFrag;
class smdlVertexAnimInstance;
class smdlMeshSkinGroup;


//============================================================================
//============================================================================
class smdlVertexObject : public scAnimatableObject
{
	public:
		//--------------------------------------------------------------------
		//	smdlVertexObject is initialized with the fragment info. The 
		//	constructor will split the frag infos and create morphable 
		//	fragments that the object will then animate.
		//	Material remapping specifies if the material in the 
		//	shared mdlFragInfos should be overriden.
		//--------------------------------------------------------------------
		smdlVertexObject( const std::vector<mdlFragInfo>& i_FragInfos,
						  const std::map<const matMaterial*,matMaterial*>& i_MaterialRemapping);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~smdlVertexObject();

		//--------------------------------------------------------------------
		//	Animate
		//--------------------------------------------------------------------
		virtual void Animate( float i_SimulationTime );

		//--------------------------------------------------------------------
		// CheckAnimation checks compatibility of this animation 
		// with the model. It will return true if the animation can
		// be played on this model.
		//--------------------------------------------------------------------
		virtual bool CheckAnimation( const anFrameAnimation& i_GeoAnimation ) const;

		//--------------------------------------------------------------------
		//	SetAnimation makes the given animation into the current animation
		//	(but does not take ownership of it; the client must preserve it
		//	as long as the scAnimatableObject needs it).  Setting an animation
		//	with this function will not blend it - just clobber the old one.
		//	A pointer to the created animation instance is returned.
		//--------------------------------------------------------------------
		virtual anFrameAnimInstance* SetAnimation(
							const anFrameAnimation& i_GeoAnimation,
							float i_StartTime);

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
		//	Adds sub animation on top of base animation.
		//	i_bPreserve - sets flag saying if the subanimation should
		//		remain during changes to the full animation.
		//--------------------------------------------------------------------
		virtual anFrameAnimInstance* AddSubAnimation(
							const anFrameAnimation& i_GeoAnimation,
							float i_StartTime,
							bool i_bPreserve);

		//--------------------------------------------------------------------
		//	Remove sub animation 
		//--------------------------------------------------------------------
		virtual void RemoveSubAnimation( anFrameAnimInstance* i_SubAnimInstance);

		//--------------------------------------------------------------------
		// SetSubAnimationBlend sets how the given subanimation should 
		// blend with the base anim. Values from 0-1. The given subanimation
		// should have been returned from a call to AddSubAnimation first.
		//--------------------------------------------------------------------
		virtual void SetSubAnimationBlend( const anFrameAnimInstance* i_SubAnim, 
							float i_Blend);

		//--------------------------------------------------------------------
		//	ClearAllSubAnimations causes all sub animations to stop.
		//--------------------------------------------------------------------
		virtual void ClearAllSubAnimations();

		//--------------------------------------------------------------------
		//	ClearSubAnimations causes non-preserved sub animations to stop.
		//--------------------------------------------------------------------
		virtual void ClearSubAnimations();

	private:
		//--------------------------------------------------------------------
		// CheckDirty - return true if the object needs to animate the model.
		//		Clears the dirty bit so that if the time is different
		//		it will be dirty next call.
		//--------------------------------------------------------------------
		bool CheckDirty(float i_SimTime);

		std::vector<smdlMeshSkinGroup*> m_Meshes;

		smdlVertexAnimInstance* m_CurAnim;
		float m_CurStartTime;

		// dirty bits
		bool m_bDirty;
		float m_LastAnimateTime;
};
