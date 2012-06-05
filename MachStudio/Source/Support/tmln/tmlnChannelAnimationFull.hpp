/*****************************************************************************
**	tmlnChannelAnimationFull.hpp
**
**	 Adapter for setting character animation
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_CHANNELANIMATIONFULL_HPP
#error tmlnChannelAnimationFull.hpp multiply included
#endif
#define TMLN_CHANNELANIMATIONFULL_HPP

#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif

//============================================================================
//============================================================================
class api3dObjectEntity;
class entAnimation;


//============================================================================
//============================================================================
class tmlnChannelAnimationFull : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelAnimationFull( const char* i_Name, api3dObjectEntity* i_pObject );

	//--------------------------------------------------------------------
	//  Test to see if this animation is compatible with the 
	//	model attached to this channel. Returns true if compatible.
	//--------------------------------------------------------------------
	bool CheckAnimation( entAnimation* i_pAnimation );

	//--------------------------------------------------------------------
	//  Set the animation by name
	//--------------------------------------------------------------------
	virtual void  SetAnimation( entAnimation* i_pAnimation, float i_SimulationTime);

	//--------------------------------------------------------------------
	//  Blend from current anim to the given index
	//--------------------------------------------------------------------
	virtual void  BlendAnimation( entAnimation* i_pAnimation, 
								  float i_SimulationTime, 
								  float i_BlendTime,
								  bool i_bSmoothBlend,
								  float i_EaseInWeight,
								  float i_EaseOutWeight );

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

	//--------------------------------------------------------------------
	//  Force through an immediate Animate() call to the entity
	//  that is animating. This updates the nodes so that
	//  attchments can use the updated matrices.
	//--------------------------------------------------------------------
	void Animate(float i_Time);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline entAnimation* GetAnimation();

private:
	api3dObjectEntity* m_pProp;
	entAnimation* m_pAnimation;
	float m_AnimTime;
	bool m_bBlending;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline entAnimation* tmlnChannelAnimationFull::GetAnimation()
{
	return m_pAnimation;
}
