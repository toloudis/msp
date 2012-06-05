/*****************************************************************************
**  dirltSelectInterest.hpp
**
**      the Select interest for system mnmTest.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef DIRLT_SELECTINTEREST_HPP
#error dirltSelectInterest.hpp multiply included
#endif
#define DIRLT_SELECTINTEREST_HPP

#ifndef CMM_SELECTINTEREST_HPP
#include "cmmSelectInterest.hpp"
#endif


//============================================================================
//============================================================================
class dirltSelectInterest : public cmmSelectInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		dirltSelectInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~dirltSelectInterest();

		//--------------------------------------------------------------------
		//	Selected
		//--------------------------------------------------------------------
		virtual void Selected( const pick3dPickList* i_pSelList, pick3dPickObject* i_pSelObj );

		//--------------------------------------------------------------------
		//	DeSelected
		//--------------------------------------------------------------------
		virtual void DeSelected( pick3dPickObject* i_pSelObj );
};
