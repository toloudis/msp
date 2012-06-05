/*****************************************************************************\
**	fbxMgr.hpp
**
**		Provides method for looking up a Exportd object throughout all systems
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef FBX_MGR_HPP
#error fbxMgr.hpp multiply included
#endif
#define FBX_MGR_HPP

//============================================================================
//	Forward References
//============================================================================
struct fbxExportData;
class fbxExportInterest;
class fsLocator;

#include <string>
#include <map>
#include <vector>

//============================================================================
//============================================================================
namespace fbxMgr
{
	//--------------------------------------------------------------------
	// GetPotentialExportData()
	//--------------------------------------------------------------------
	fbxExportData GetPotentialExportData();

	//--------------------------------------------------------------------
	// DoExport()
	//--------------------------------------------------------------------
	void DoExport( fsLocator &i_Locator, const fbxExportData &i_Data );

	//--------------------------------------------------------------------
	//	RegisterExportInterest() - add a Export interest to the system
	//--------------------------------------------------------------------
	void RegisterExportInterest( fbxExportInterest* i_pInterest );

	//--------------------------------------------------------------------
	// UnRegisterExportInterest()
	//--------------------------------------------------------------------
	void UnRegisterExportInterest( fbxExportInterest* i_pInterest );

	//--------------------------------------------------------------------
	// Reset()
	//--------------------------------------------------------------------
	void Reset();

	//--------------------------------------------------------------------
	// SetExportSelected()
	//--------------------------------------------------------------------
	void SetExportSelected( bool i_Val );

	//--------------------------------------------------------------------
	// GetExportSelected()
	//--------------------------------------------------------------------
	bool GetExportSelected();
};
