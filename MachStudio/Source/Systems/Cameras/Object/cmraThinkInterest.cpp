/****************************************************************************\
**	cmraThinkInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Object/cmraThinkInterest.hpp"

#include "Support/cams/camsFollowUtil.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraThinkInterest::cmraThinkInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
cmraThinkInterest::~cmraThinkInterest()
{
}


//--------------------------------------------------------------------
//	Think
//--------------------------------------------------------------------
//virtual 
void cmraThinkInterest::Think( )
{
// This is handled by the individual render panes now
//	camsFollowUtil::Think();
}
