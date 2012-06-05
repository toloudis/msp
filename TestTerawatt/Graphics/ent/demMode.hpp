/*****************************************************************************
**  demMode.hpp
**
**      demMode is an abstract interface representing a program mode.
**	A program can only use one mode at a time.  Before a mode begins
**	(receives its first Think), the virtual Initialize function is called.  
**	After the mode ends (requests termination), the virtual DeInitialize
**	function is called.
**		When a mode decides that it is finished or that program flow needs
**	to go to a new mode, it should call the SetTerminateCondition function.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_MODE_HPP
#error demMode.hpp multiply included
#endif
#define DEM_MODE_HPP

class demMode
{
	public:

		//====================================================================
		//====================================================================
		demMode();

		//====================================================================
		//====================================================================
		virtual ~demMode() = 0;

		//====================================================================
		//	The mode should do it's per frame "work" in the Think function.
		//====================================================================
		virtual void Think() = 0;

		//====================================================================
		//	Initialize will be called before the first call of Think after
		//	the object is first created or DeInitialized.  During the
		//	lifetime of a mode, Initialize and DeInitialize may be called
		//	several times.
		//====================================================================
		virtual void Initialize() = 0;

		//====================================================================
		//	The object should clean up things that are not needed while the
		//	mode is not running in the DeInitialize function.
		//====================================================================
		virtual void DeInitialize() = 0;

		//====================================================================
		//	The TerminateCondition describes the behavior requested by the
		//	mode when it is finished.
		//
		//	e_Continue - don't terminate the mode.  (The default)
		//	e_TerminateAndPersist - DeInitialize the mode, and move on to the
		//		next mode higher up the stack (which was typically pushed on
		//		by the terminating mode).
		//	e_TerminateAndDestroy - DeInitialize the mode, delete it, and move
		//		on to the next mode higher up the stack, if available, or
		//		to the previous mode lower on the stack if not.
		//	e_TerminateAndRemove - Like e_TerminateAndDestroy, except does
		//		not delete the mode.  This allows the same mode object to be
		//		used (pushed) later.
		//====================================================================
		enum TerminateCondition
		{
			e_Continue,
			e_TerminateAndPersist,
			e_TerminateAndDestroy,
			e_TerminateAndRemove
		};

		//====================================================================
		//	GetTerminateCondition returns whichever terminate condition is
		//	requested by the mode.
		//====================================================================
		TerminateCondition GetTerminateCondition() const;

	protected:

		//====================================================================
		//	SetTerminateCondition should be called when a mode is ready to 
		//	end (it's not neccesary to call it to set "e_Continue", as this
		//	is the default).  The mode will 
		//====================================================================
		void SetTerminateCondition(TerminateCondition i_Condition);

	private:
		
		TerminateCondition m_Condition;
};