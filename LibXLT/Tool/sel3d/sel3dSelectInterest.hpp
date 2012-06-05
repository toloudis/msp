/****************************************************************************\
**	sel3dSelectInterest.hpp
**
**		A Select Interest is usually related to a system that cares about
**	objects being Selected.
**
**		An interest can decide which methods are appropriate for what
**	it needs to know. AddedToSelection and RemovedFromSelection can
**	be used to track individual object changes. SelectionChanged is called
**	any time the selection list changes at all. 
**
**	StudioGPU
**	Copyright(C) 2003-6 - All Rights Reserved
\****************************************************************************/
#ifdef SEL3D_SELECTINTEREST_HPP
#error sel3dSelectInterest.hpp multiply included
#endif
#define SEL3D_SELECTINTEREST_HPP


//============================================================================
//	Forward References
//============================================================================
class sel3dObject;
class pick3dPickList;


//============================================================================
//============================================================================
class sel3dSelectInterest
{
	public:
		//--------------------------------------------------------------------
		//	SelectionChanged - called when the selection list is changed
		//	at all. Many interests may only need to override this function.
		//--------------------------------------------------------------------
		virtual void SelectionChanged();

		//--------------------------------------------------------------------
		//	AddedToSelection - called when an object is added to the
		//		selection list.
		//--------------------------------------------------------------------
		virtual void AddedToSelection( sel3dObject* i_pSelObj );

		//--------------------------------------------------------------------
		//	RemovedFromSelection - called when an object is removed from
		//		the selection list.
		//--------------------------------------------------------------------
		virtual void RemovedFromSelection( sel3dObject* i_pSelObj );
};
