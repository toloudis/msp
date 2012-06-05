/****************************************************************************\
**	pythThinkInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "Support/pyth/pythThinkInterest.hpp"

#include "Support/pyth/pythEventCallbackMgr.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
pythThinkInterest::pythThinkInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
pythThinkInterest::~pythThinkInterest()
{
}


//--------------------------------------------------------------------
//	Think
//--------------------------------------------------------------------
//virtual 
void pythThinkInterest::Think( )
{
	pythEventCallbackMgr::InvokeEvents();
}
