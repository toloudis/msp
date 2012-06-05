/****************************************************************************\
**	ltstIsolatable.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/ltst/ltstIsolatable.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ltstIsolatable::ltstIsolatable()
: m_bIsolationVisible(true),
  m_IsolationAlpha(1)
{
}

//--------------------------------------------------------------------
// Sets values of isolation state and calls virtual callback
// IsolationChanged() if the state has changed.
//--------------------------------------------------------------------
void ltstIsolatable::SetIsolationState(bool i_bVisible, float i_IsolationAlpha)
{
	if ((m_bIsolationVisible != i_bVisible) || (m_IsolationAlpha != i_IsolationAlpha))
	{
		m_bIsolationVisible = i_bVisible;
		m_IsolationAlpha = i_IsolationAlpha;
		this->IsolationChanged();
	}
}

//--------------------------------------------------------------------
// Set goal state that will be animated to
//--------------------------------------------------------------------
void ltstIsolatable::AnimateIsolationState(bool i_bVisible)
{
	m_bGoalState = i_bVisible;
	m_AnimBeginAlpha = m_IsolationAlpha;
}

//--------------------------------------------------------------------
// Animate to a percentage towards goal state
//--------------------------------------------------------------------
void ltstIsolatable::AnimateState(float i_Alpha)
{
	// Only need to animate if we are going to or from a visible state
	if (m_bGoalState || m_bIsolationVisible)
	{
		float goal_fade = (m_bGoalState) ? 1.0f : 0.0f;
		float fade = m_AnimBeginAlpha + (goal_fade - m_AnimBeginAlpha) * i_Alpha;
		SetIsolationState(true, fade);
	}
}

//--------------------------------------------------------------------
// Finish animation, setting goal state directly
//--------------------------------------------------------------------
void ltstIsolatable::FinishAnimationState()
{
	if (m_bGoalState)
		SetIsolationState(true, 1.0f);
	else
		SetIsolationState(false, 0.0f);
}
