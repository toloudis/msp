/*****************************************************************************
**	scControlAnim.hpp
**
**		scControlAnim defines a base class for animations that give
**	programmatic control over nodes in an object.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef SC_CONTROLANIM_HPP
#error scControlAnim.hpp multiply included
#endif
#define SC_CONTROLANIM_HPP


//============================================================================
//============================================================================
class scControlAnim
{
	public:
		//--------------------------------------------------------------------
		//  Constructor
		//--------------------------------------------------------------------
		scControlAnim();

		//--------------------------------------------------------------------
		//	Destructor
		//--------------------------------------------------------------------
		virtual ~scControlAnim();

		//--------------------------------------------------------------------
		//	Animate is called by the scObject after it has done its base
		//		animation
		//--------------------------------------------------------------------
		virtual void Animate(float i_SimulationTime) = 0;

		//--------------------------------------------------------------------
		// CheckDirty - return true if the object needs to animate the model.
		//		Clears the dirty bit so that if the time is different
		//		it will be dirty next call.
		//--------------------------------------------------------------------
		virtual bool CheckDirty(float i_SimTime) = 0;
};
