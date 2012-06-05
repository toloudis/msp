/*****************************************************************************\
**	mexpMgr.hpp
**
**		Provides method for looking up a Exportd object throughout all systems
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MEXP_MGR_HPP
#error mexpMgr.hpp multiply included
#endif
#define MEXP_MGR_HPP


//============================================================================
//	Forward References
//============================================================================
struct mexpExportData;
class mexpExportInterest;
class fsLocator;


//============================================================================
//============================================================================
namespace mexpMgr
{
	//--------------------------------------------------------------------
	// Gather up the list of the potential things to export and
	// return in data structure
	//--------------------------------------------------------------------
	mexpExportData GetPotentialExportData();

	//--------------------------------------------------------------------
	// Export Maya Ascii file by calling Export functions on each
	//	registered interest. Uses the data structure to control
	//	which things should be exported.
	//--------------------------------------------------------------------
	void DoExport( fsLocator &i_Locator, const mexpExportData &i_Data );

	//--------------------------------------------------------------------
	//	RegisterExportInterest() - add a Export interest to the system
	//--------------------------------------------------------------------
	void RegisterExportInterest( mexpExportInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterExportInterest() - remove a Export interest from the system.
	//
	//	Note: this will NOT delete the Export interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterExportInterest( mexpExportInterest* i_pInterest );

};
