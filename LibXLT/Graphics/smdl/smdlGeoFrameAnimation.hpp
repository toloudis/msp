/*****************************************************************************
**	smdlGeoFrameAnimation.hpp
**
**		A smdlGeoFrameAnimation
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_GEOFRAMEANIMATION_HPP
#error smdlGeoFrameAnimation.hpp multiply included
#endif
#define SMDL_GEOFRAMEANIMATION_HPP

#ifndef ENT_ANIMATION_HPP
#include "Graphics/ent/entAnimation.hpp"
#endif
#ifndef ENT_ANIMINSTANCE_HPP
#include "Graphics/ent/entAnimInstance.hpp"
#endif
#ifndef G3D_CONSTANTS_HPP
#include "Graphics/g3d/g3dConstants.hpp"
#endif
#ifndef SMDL_KEYROOTMAP_HPP
#include "Graphics/smdl/smdlKeyRootMap.hpp"
#endif 


//============================================================================
//============================================================================
class smdlGeoFrameAnimation : public entAnimation
{
	public:
		//--------------------------------------------------------------------
		//	The smdlGeoFrameAnimation requires a reference to animation
		//	keys.  The frame animation does not own the keys in
		//	order to allow sharing of key data between animations
		//--------------------------------------------------------------------
		explicit smdlGeoFrameAnimation(const smdlKeyRootMap& i_KeyRoots, 
			float i_FrameRate = g3dConstants::c_fDefaultFrameRate);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~smdlGeoFrameAnimation();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//const smdlTree<smdlGeoAnimKeys>& GetKeys() const;
		const smdlKeyRootMap& GetKeyRoots() const;

		//--------------------------------------------------------------------
		//	CreateAnimInstance creates an anAnimInstance, of a type
		//	appropriate to the type of the anAnimation.
		//--------------------------------------------------------------------
		virtual anAnimInstance* CreateAnimInstance(float i_TimeOrigin) const;

		//--------------------------------------------------------------------
		//	Clone returns a copy of "this" allocated on the heap.
		//--------------------------------------------------------------------
		virtual anAnimation* Clone() const;

		//--------------------------------------------------------------------
		// Name of joint that is root of this subanimation
		//--------------------------------------------------------------------
		//void SetNameOfRoot(const std::string& i_Name);
		//const std::string& GetNameOfRoot() const;

		//--------------------------------------------------------------------
		// Return true if this is a subanimation with a defined name of root
		//--------------------------------------------------------------------
		//bool HasRootName() const;

		//--------------------------------------------------------------------
		// AdditiveAnimation is true if the transformations are deltas
		//	from the base pose and can be used in additive subanimations.
		//--------------------------------------------------------------------
		void SetAdditiveAnimation(bool i_Delta);
		bool GetAdditiveAnimation() const;

		//--------------------------------------------------------------------
		// AttachToRootJoint is used to support older file formats
		//	on newer hierarchies. It means that the animation
		//	should attach to the first joint it finds in the hierarchy.
		//--------------------------------------------------------------------
		void SetAttachToRootJoint(bool i_Attach);
		bool GetAttachToRootJoint() const;

		//--------------------------------------------------------------------
		// Some animation that isn't coming from Maya should ignore the
		//	joint orientation built into the scene graph.
		//--------------------------------------------------------------------
		void SetIgnoreJointOrientation(bool i_bIgnore);
		bool GetIgnoreJointOrientation() const;

	private:
		const smdlKeyRootMap& m_KeyRoots;
		//const smdlTree<smdlGeoAnimKeys>& m_Keys;
		//std::string m_NameOfRoot;
		bool m_bAdditive;
		bool m_bAttachToRootJoint;
		bool m_bIgnoreJointOrientation;

};


//============================================================================
//	smdlGeoFrameAnimInstance
//============================================================================
class smdlGeoFrameAnimInstance : public entAnimInstance
{
	public:
		//--------------------------------------------------------------------
		//	The anFrameAnimInstance constructor requires the animation beginning
		// time and the animation used by this instance.
		//--------------------------------------------------------------------
		smdlGeoFrameAnimInstance(	float i_TimeOrigin,
								const smdlGeoFrameAnimation& i_Anim);

		//--------------------------------------------------------------------
		//	pure virtual destructor
		//--------------------------------------------------------------------
		virtual ~smdlGeoFrameAnimInstance();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline const smdlGeoFrameAnimation& GetAnim() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//inline const smdlTree<smdlGeoAnimKeys>& GetKeys() const;
		inline const smdlKeyRootMap& GetKeyRoots() const;

		//--------------------------------------------------------------------
		//	The Clone function creates a new copy of the anFrameAnimInstance
		//	on the heap.
		//--------------------------------------------------------------------
		virtual anAnimInstance* Clone() const;

	private:
		const	smdlGeoFrameAnimation& m_Anim;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
const smdlGeoFrameAnimation& smdlGeoFrameAnimInstance::GetAnim() const
{
	return m_Anim;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const smdlKeyRootMap& smdlGeoFrameAnimInstance::GetKeyRoots() const
{
	return m_Anim.GetKeyRoots();
}

