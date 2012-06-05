/*****************************************************************************
**  demMode.cpp
**
**      demMode is an abstract interface representing a program mode.
**	A program can only be using one mode at a time.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demMode.hpp"

//====================================================================
//====================================================================
demMode::demMode()
{
}

//====================================================================
//====================================================================
demMode::~demMode()
{
}


//====================================================================
//	GetTerminateCondition returns whichever terminate condition is
//	requested by the mode.
//====================================================================
demMode::TerminateCondition demMode::GetTerminateCondition() const
{
	return m_Condition;
}

//====================================================================
//	SetTerminateCondition should be called when a mode is ready to 
//	end (it's not neccesary to call it to set "e_Continue", as this
//	is the default).  The mode will 
//====================================================================
void demMode::SetTerminateCondition(TerminateCondition i_Condition)
{
	m_Condition = i_Condition;
}
