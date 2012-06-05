/*****************************************************************************
**  mnmTestSelectInterest.hpp
**
**      the Select interest for system mnmTest.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MNM_TESTSELECTINTEREST_HPP
#error mnmTestSelectInterest.hpp multiply included
#endif
#define MNM_TESTSELECTINTEREST_HPP

#include "sel3dSelectInterest.hpp"


//============================================================================
//============================================================================
class mnmTestSelectInterest : public sel3dSelectInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mnmTestSelectInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~mnmTestSelectInterest();

		//--------------------------------------------------------------------
		//	Selected
		//--------------------------------------------------------------------
		virtual void Selected( const pick3dPickList* i_pSelList, pick3dPickObject* i_pSelObj );

		//--------------------------------------------------------------------
		//	DeSelected
		//--------------------------------------------------------------------
		virtual void DeSelected( pick3dPickObject* i_pSelObj );
};
