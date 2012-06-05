/*****************************************************************************
**	smdlMorphFrameAnimation.hpp
**
**		A smdlMorphFrameAnimation
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_MORPHFRAMEANIMATION_HPP
#error smdlMorphFrameAnimation.hpp multiply included
#endif
#define SMDL_MORPHFRAMEANIMATION_HPP

#ifndef ENT_ANIMATION_HPP
#include "Graphics/ent/entAnimation.hpp"
#endif
#ifndef ENT_ANIMINSTANCE_HPP
#include "Graphics/ent/entAnimInstance.hpp"
#endif
#ifndef SMDL_MORPHANIMKEYS_HPP
#include "Graphics/smdl/smdlMorphAnimKeys.hpp"
#endif
#ifndef G3D_CONSTANTS_HPP
#include "Graphics/g3d/g3dConstants.hpp"
#endif

#include <map>
#include <string>


//============================================================================
//============================================================================
class smdlMorphFrameAnimation : public entAnimation
{
	public:
		//--------------------------------------------------------------------
		//	The smdlMorphFrameAnimation requires a reference to animation
		//	keys.  The frame animation does not own the keys in
		//	order to allow sharing of key data between animations
		//--------------------------------------------------------------------
		explicit smdlMorphFrameAnimation(const std::map<std::string, smdlMorphAnimKeys>& i_Keys, 
			float i_FrameRate = g3dConstants::c_fDefaultFrameRate);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~smdlMorphFrameAnimation();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const std::map<std::string, smdlMorphAnimKeys>& GetKeys() const;

		//--------------------------------------------------------------------
		// Look up name in map and return keys for this target. May return
		// NULL if no keys for the given target name exist.
		//--------------------------------------------------------------------
		const smdlMorphAnimKeys* GetKeysForTarget(const std::string &i_Name) const;

		//--------------------------------------------------------------------
		//	CreateAnimInstance creates an anAnimInstance, of a type
		//	appropriate to the type of the anAnimation.
		//--------------------------------------------------------------------
		virtual anAnimInstance* CreateAnimInstance(float i_TimeOrigin) const;

		//--------------------------------------------------------------------
		//	Clone returns a copy of "this" allocated on the heap.
		//--------------------------------------------------------------------
		virtual anAnimation* Clone() const;

	private:
		const std::map<std::string, smdlMorphAnimKeys>& m_Keys;
};



//============================================================================
//	smdlMorphFrameAnimInstance
//============================================================================
class smdlMorphFrameAnimInstance : public entAnimInstance
{
	public:
		//--------------------------------------------------------------------
		//	The anFrameAnimInstance constructor requires the animation beginning
		// time and the animation used by this instance.
		//--------------------------------------------------------------------
		smdlMorphFrameAnimInstance(	float i_TimeOrigin,
								const smdlMorphFrameAnimation& i_Anim);

		//--------------------------------------------------------------------
		//	pure virtual destructor
		//--------------------------------------------------------------------
		virtual ~smdlMorphFrameAnimInstance();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline const smdlMorphFrameAnimation& GetAnim() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline const std::map<std::string, smdlMorphAnimKeys>& GetKeys() const;

		//--------------------------------------------------------------------
		//	The Clone function creates a new copy of the anFrameAnimInstance
		//	on the heap.
		//--------------------------------------------------------------------
		virtual anAnimInstance* Clone() const;

	private:
		const	smdlMorphFrameAnimation& m_Anim;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
const smdlMorphFrameAnimation& smdlMorphFrameAnimInstance::GetAnim() const
{
	return m_Anim;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::map<std::string, smdlMorphAnimKeys>& smdlMorphFrameAnimInstance::GetKeys() const
{
	return m_Anim.GetKeys();
}

