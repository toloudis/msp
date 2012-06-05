/*****************************************************************************
**	captRenderProgressMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/capt/captRenderProgressMgr.hpp"
#include "Support/capt/captRenderProgressInterest.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include <string>
#include <vector>
//============================================================================
//============================================================================
namespace
{
	captRenderProgressData m_Data;
	std::vector<captRenderProgressInterest*>	l_ProgressInterestList;
}
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
captRenderProgressData& captRenderProgressMgr::Data()
{
	return m_Data;
}

//------------------------------------------------------------------------
//	RegisterInterest() - add an interest
//------------------------------------------------------------------------
void captRenderProgressMgr::RegisterInterest( captRenderProgressInterest* i_pInterest )
{
	//DBG_ASSERT( i_pInterest != 0, "NULL interest" );
	l_ProgressInterestList.push_back( i_pInterest );
}

//------------------------------------------------------------------------
//	UnRegisterInterest() - remove an interest
//
//	Note: this will NOT delete the  interest.  It is up to the
//	registerer.
//------------------------------------------------------------------------
void captRenderProgressMgr::UnRegisterInterest( captRenderProgressInterest* i_pInterest )
{
//DBG_ASSERT( i_pInterest != 0, "NULL interest" );
envSTLHelpers::RemoveOneValue( l_ProgressInterestList, i_pInterest );
}

//------------------------------------------------------------------------
//	Clear() - clear the interest list
//------------------------------------------------------------------------
void captRenderProgressMgr::ClearInterests()
{
	l_ProgressInterestList.clear();
}

//------------------------------------------------------------------------
//	This gets called to let the interests know that the data has changed
//------------------------------------------------------------------------
void captRenderProgressMgr::FrameComplete()
{
	for (int i = 0; i < l_ProgressInterestList.size(); ++i)
	{
		l_ProgressInterestList[i]->FrameComplete(m_Data);
	}
}

