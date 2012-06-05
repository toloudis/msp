/*****************************************************************************
**	smdlSubAnimation.hpp
**
**		smdlSubAnimation is a helper class for managing currently running
**	sub-animation instances.
**
**	The iterator can be in 1 of 3 states:
**		1) not attached yet, it is looking for the root of its animation
**		2) attached, it is iterating through its hierarchy now
**		3) off-end, it is iterating past the hierarchy it has
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_SUBANIMATION_HPP
#error smdlSubAnimation.hpp multiply included
#endif
#define SMDL_SUBANIMATION_HPP

#ifndef SMDL_GEOANIMKEYS_HPP
#include "Graphics/smdl/smdlGeoAnimKeys.hpp"
#endif
#ifndef SMDL_TREE_HPP
#include "Graphics/smdl/smdlTree.hpp"
#endif


//============================================================================
//============================================================================
class smdlGeoFrameAnimInstance;
class g3dSceneNode;


//============================================================================
//============================================================================
class smdlSubAnimation
{
	public:
		//--------------------------------------------------------------------
		// This object takes over ownership of the animation instance
		//--------------------------------------------------------------------
		smdlSubAnimation(smdlGeoFrameAnimInstance* i_pAnimInstance);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~smdlSubAnimation();

		//--------------------------------------------------------------------
		// Blend is a value from 0-1 that controls the influence of the
		//	subanimation.
		//--------------------------------------------------------------------
		float GetBlend() const
		{
			return m_Blend;
		}
		void SetBlend(float i_Blend)
		{
			m_Blend = i_Blend;
		}

		//--------------------------------------------------------------------
		// Additive is a boolean that controls how the subanimation blends
		//	with the base anim. If additive is true, it adds on top of it; 
		//	otherwise it replaces the animation underneath.
		//--------------------------------------------------------------------
		bool GetAdditive() const
		{
			return m_bAdditive;
		}
		void SetAdditive(bool i_bAdditive)
		{
			m_bAdditive = i_bAdditive;
		}

		//--------------------------------------------------------------------
		// Preserve is a boolean that controls whether the subanimation
		//	should remain during full body animation changes.
		//--------------------------------------------------------------------
		bool GetPreserve() const
		{
			return m_bPreserve;
		}
		void SetPreserve(bool i_bPreserve)
		{
			m_bPreserve = i_bPreserve;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline const smdlGeoFrameAnimInstance* GetAnimInstance() const
		{
			return m_pAnimInstance;
		}

	private:
		smdlGeoFrameAnimInstance* m_pAnimInstance;
		float m_Blend;
		bool m_bAdditive;
		bool m_bPreserve;
};


//============================================================================
// This class makes it simpler to iterate over a hierarchy and tell
// when the subanimation is active. It wraps around a smdlTree
// const_iterator when it is in the part of the hierarchy that
// is relevant for this sub animation.
//============================================================================
class smdlSubAnimationIterator
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		smdlSubAnimationIterator(const std::string& i_RootName, 
			const smdlTree<smdlGeoAnimKeys>& i_AnimKeys,
			float i_CurrentFrame, 
			g3dSceneNode* i_pRootNode,
			float i_Blend,
			bool i_bIsAdditive);

		//--------------------------------------------------------------------
		// sub-animations are only attached for part of the hierarchy,
		// if the sub-anim is not attached, GetData will return NULL
		//--------------------------------------------------------------------
		bool IsAttached() const { return (m_bIsAttached && (m_OffEnd == 0)); }

		//--------------------------------------------------------------------
		// move down in the hierarchy to the child with the given index.
		// pass in the scene node that is the child in the hierarchy so
		// that the iterator knows when to attach to the hierarchy.
		//--------------------------------------------------------------------
		void MoveToChild(int i_Num, g3dSceneNode& i_Node);

		//--------------------------------------------------------------------
		// Move up in the hierarchy
		//--------------------------------------------------------------------
		void MoveToParent();

		//--------------------------------------------------------------------
		// Return key data in iterator if attached, otherwise return NULL.
		//--------------------------------------------------------------------
		const smdlGeoAnimKeys* GetData() const;

		//--------------------------------------------------------------------
		// The Simulation Time passed in to the constructor has been 
		//	converted to a frame number within this given animation.
		//	This function returns that frame for use in looking up
		//	key frame data.
		//--------------------------------------------------------------------
		float GetCurrentFrame() const
		{
			return m_CurFrame;
		}

		//--------------------------------------------------------------------
		// Blend is a value from 0-1 that controls the influence of the
		//	subanimation.
		//--------------------------------------------------------------------
		float GetBlend() const
		{
			return m_Blend;
		}

		//--------------------------------------------------------------------
		// Additive is a boolean that controls how the subanimation blends
		//	with the base anim. If additive is true, it adds on top of it; 
		//	otherwise it replaces the animation underneath.
		//--------------------------------------------------------------------
		bool GetAdditive() const
		{
			return m_bIsAdditive;
		}

	private:
		//const smdlSubAnimation& m_AnimInstance;
		float m_CurFrame;
		bool m_bIsAttached;
		float m_Blend;
		bool m_bIsAdditive;
		smdlTree<smdlGeoAnimKeys>::const_iterator m_Iterator;
		smdlTree<smdlGeoAnimKeys>::const_iterator m_RootIterator;
		std::string m_RootName;
		int m_OffEnd; // depth when subanimation hierarchy is shorter than model
};
