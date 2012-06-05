/*****************************************************************************
**	smdlVertexAnimation.hpp
**
**		A smdlVertexAnimation represents baked vertex animation.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_VERTEXANIMATION_HPP
#error smdlVertexAnimation.hpp multiply included
#endif
#define SMDL_VERTEXANIMATION_HPP

#ifndef ENT_ANIMATION_HPP
#include "Graphics/ent/entAnimation.hpp"
#endif
#ifndef ENT_ANIMINSTANCE_HPP
#include "Graphics/ent/entAnimInstance.hpp"
#endif
#ifndef G3D_CONSTANTS_HPP
#include "Graphics/g3d/g3dConstants.hpp"
#endif
#ifndef SMDL_VERTEXANIMKEYS_HPP
#include "Graphics/smdl/smdlVertexAnimKeys.hpp"
#endif

#include <map>
#include <string>


//============================================================================
//============================================================================
class smdlVertexAnimation : public entAnimation
{
	public:
		//--------------------------------------------------------------------
		//	The smdlVertexAnimation requires references to animation
		//	keys.  The frame animation does not own the keys in
		//	order to allow sharing of key data between animations
		//--------------------------------------------------------------------
		smdlVertexAnimation(const std::vector<smdlVertexAnimKeys>& i_VertexKeys, 
							float i_FrameRate = g3dConstants::c_fDefaultFrameRate);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~smdlVertexAnimation();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const std::vector<smdlVertexAnimKeys>& GetVertexKeys() const;

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
		const std::vector<smdlVertexAnimKeys>& m_VertexKeys;
};

//============================================================================
//	smdlVertexAnimInstance
//============================================================================
class smdlVertexAnimInstance : public entAnimInstance
{
	public:
		//--------------------------------------------------------------------
		//	The anFrameAnimInstance constructor requires the animation beginning
		// time and the animation used by this instance.
		//--------------------------------------------------------------------
		smdlVertexAnimInstance(	float i_TimeOrigin,
								const smdlVertexAnimation& i_Anim);

		//--------------------------------------------------------------------
		//	pure virtual destructor
		//--------------------------------------------------------------------
		virtual ~smdlVertexAnimInstance();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline const smdlVertexAnimation& GetAnim() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline const std::vector<smdlVertexAnimKeys>& GetVertexKeys() const;

		//--------------------------------------------------------------------
		//	The Clone function creates a new copy of the anFrameAnimInstance
		//	on the heap.
		//--------------------------------------------------------------------
		virtual anAnimInstance* Clone() const;

	private:
		const	smdlVertexAnimation& m_Anim;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const smdlVertexAnimation& smdlVertexAnimInstance::GetAnim() const
{
	return m_Anim;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const std::vector<smdlVertexAnimKeys>& smdlVertexAnimInstance::GetVertexKeys() const
{
	return m_Anim.GetVertexKeys();
}
