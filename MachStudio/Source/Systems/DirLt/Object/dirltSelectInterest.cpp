/****************************************************************************\
**	dirltSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "dirltSelectInterest.hpp"

#include "dirltObjectMgr.hpp"
#include "dirltDialogUtil.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dirltSelectInterest::dirltSelectInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
dirltSelectInterest::~dirltSelectInterest()
{
}


//--------------------------------------------------------------------
//	Selected
//--------------------------------------------------------------------
//virtual
void dirltSelectInterest::Selected( const pick3dPickList* i_pSelList, pick3dPickObject* i_pSelObj )
{
	if (dynamic_cast<dirltDirLightObject*>(i_pSelObj) != 0)
	{
		cmmSelectInterest::Selected(i_pSelList,i_pSelObj);
	}
}

//--------------------------------------------------------------------
//	DeSelected
//--------------------------------------------------------------------
//virtual
void dirltSelectInterest::DeSelected( pick3dPickObject* i_pSelObj )
{
	if (dynamic_cast<dirltDirLightObject*>(i_pSelObj) != 0)
	{
		cmmSelectInterest::DeSelected(i_pSelObj);
	}
}
