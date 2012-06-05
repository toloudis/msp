/****************************************************************************\
**	mnmTestSelectInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "mnmTestSelectInterest.hpp"

#include "tmlnScriptObject.hpp"

//	library
#include "dbgAssert.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmTestSelectInterest::mnmTestSelectInterest()
: sel3dSelectInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
mnmTestSelectInterest::~mnmTestSelectInterest()
{
}


//--------------------------------------------------------------------
//	Selected
//--------------------------------------------------------------------
//virtual
void mnmTestSelectInterest::Selected( const pick3dPickList* i_pSelList, pick3dPickObject* i_pSelObj )
{
	DBG_ASSERT0( i_pSelObj != 0, "Cannot select a NULL object" );

	//if ( dynamic_cast<tmlnScriptObject*>(i_pSelObj) )
	//{
	//	DBG_LOG0( "selected an tmlnScriptObject" );
	//}
}

//--------------------------------------------------------------------
//	DeSelected
//--------------------------------------------------------------------
//virtual
void mnmTestSelectInterest::DeSelected( pick3dPickObject* i_pSelObj )
{
	DBG_ASSERT0( i_pSelObj != 0, "Cannot deselect a NULL object" );

	//if ( dynamic_cast<tmlnScriptObject*>(i_pSelObj) )
	//{
	//	DBG_LOG0( " deselected an tmlnScriptObject" );
	//}
}
