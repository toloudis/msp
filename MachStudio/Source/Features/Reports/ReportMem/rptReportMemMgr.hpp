/*****************************************************************************\
**	rptReportMemMgr.hpp
**
**		Provides method for all objects reporting memory usage throughout all systems
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef RPT_REPORTMEMMGR_HPP
#error rptReportMemMgr.hpp multiply included
#endif
#define RPT_REPORTMEMMGR_HPP


//============================================================================
//	Forward References
//============================================================================
class gfFileTxt;
class rptReportMemInterest;


//============================================================================
//============================================================================
namespace rptReportMemMgr
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DoReportMem( gfFileTxt& i_File );

	//--------------------------------------------------------------------
	//	RegisterExportInterest() - add a Export interest to the system
	//--------------------------------------------------------------------
	void RegisterReportMemInterest( rptReportMemInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterExportInterest() - remove a Export interest from the system.
	//
	//	Note: this will NOT delete the Export interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterReportMemInterest( rptReportMemInterest* i_pInterest );

};
