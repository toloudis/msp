/*****************************************************************************
**  ltstIsolatable.hpp
**
**	Base class for a light or object that can be isolated in a certain view.
**	Lights should be disabled when not in the isolation set and should alter
**	their intensity based on the isolation alpha value for animating a fade
**	in and out.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef LTST_ISOLATABLE_HPP
#error ltstIsolatable.hpp multiply included
#endif
#define LTST_ISOLATABLE_HPP


//============================================================================
//============================================================================
class ltstIsolatable
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		ltstIsolatable();

		//--------------------------------------------------------------------
		//	IsolationVisible - considering the current isolation settings,
		//	should this object be visible or the light enabled?
		//--------------------------------------------------------------------
		bool GetIsolationVisible() const { return m_bIsolationVisible; }

		//--------------------------------------------------------------------
		//	IsolationAlpha - during an isolation animation, the alpha
		//  interpolates between 0 and 1. Use this alpha value to modify
		//  the light's intensity to fade the light on and off.
		//--------------------------------------------------------------------
		float GetIsolationAlpha() const { return m_IsolationAlpha; }

		//--------------------------------------------------------------------
		// Sets values of isolation state and calls virtual callback
		// IsolationChanged() if the state has changed.
		//--------------------------------------------------------------------
		void SetIsolationState(bool i_bVisible, float i_IsolationAlpha);

		//--------------------------------------------------------------------
		// Set goal state that will be animated to
		//--------------------------------------------------------------------
		void AnimateIsolationState(bool i_bVisible);

		//--------------------------------------------------------------------
		// Animate to a percentage towards goal state
		//--------------------------------------------------------------------
		void AnimateState(float i_Alpha);

		//--------------------------------------------------------------------
		// Finish animation, setting goal state directly
		//--------------------------------------------------------------------
		void FinishAnimationState();

	protected:
		//--------------------------------------------------------------------
		//	IsolationChanged - notification that this objects 
		//--------------------------------------------------------------------
		virtual void IsolationChanged() = 0;

	private:
		bool	m_bIsolationVisible;
		float	m_IsolationAlpha;
		bool	m_bGoalState;
		float	m_AnimBeginAlpha;
};
