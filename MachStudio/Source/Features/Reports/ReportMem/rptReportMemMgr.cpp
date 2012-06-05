/****************************************************************************\
**	rptReportMemMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/Reports/ReportMem/rptReportMemMgr.hpp"

#include "Features/Reports/ReportMem/rptReportMemInterest.hpp"

//	library
#include "Core/env/envSTLHelpers.hpp"
#include "Core/gf/gfFileTxt.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace
{
	std::vector<rptReportMemInterest*>	l_ReportMemInterestList;
}

//--------------------------------------------------------------------
// Export Maya Ascii file by calling Export functions on each
//	registered interest.
//--------------------------------------------------------------------
void rptReportMemMgr::DoReportMem( gfFileTxt& i_File )
{

	// Gather exporting data from interests
	const int num_interests = l_ReportMemInterestList.size();
	for ( int i=0; i < num_interests; i++ )
	{
		l_ReportMemInterestList[i]->Report( i_File );
	}

}

//--------------------------------------------------------------------
//	RegisterExportInterest() - add a Export interest to the system
//--------------------------------------------------------------------
void rptReportMemMgr::RegisterReportMemInterest( rptReportMemInterest* i_pInterest )
{
	DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Export Interest" );
	l_ReportMemInterestList.push_back( i_pInterest );
}

//--------------------------------------------------------------------
//	UnRegisterExportInterest() - remove a Export interest from the system.
//
//	Note: this will NOT delete the Export interest.  It is up to the
//	registerer.
//--------------------------------------------------------------------
void rptReportMemMgr::UnRegisterReportMemInterest( rptReportMemInterest* i_pInterest )
{
	envSTLHelpers::RemoveOneValue( l_ReportMemInterestList, i_pInterest );
}
