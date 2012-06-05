/*****************************************************************************\
**	visMgr.hpp
**
**		the selelection system manager.
**	This controls all the selection interests for the system.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef VIS_MGR_HPP
#error visMgr.hpp multiply included
#endif
#define VIS_MGR_HPP

#include <string>


//============================================================================
//	Forward References
//============================================================================
class visVisibleInterest;


//============================================================================
//============================================================================
namespace visMgr
{
	//--------------------------------------------------------------------
	//	ShowIcons - show or hide icons that are not part of real scene.
	//--------------------------------------------------------------------
	void ShowIcons( bool i_bVisible );

	//--------------------------------------------------------------------
	// Return if icons are current visible
	//--------------------------------------------------------------------
	bool IsShowIcons();

	//--------------------------------------------------------------------
	// Make sure that all geometry is visible for rendering
	//--------------------------------------------------------------------
	void ConfirmGeometryVisible();

	//--------------------------------------------------------------------
	// Set object active state in the render layer
	//--------------------------------------------------------------------
	void SetActiveInRenderLayer(std::string& i_ObjectName, bool i_bVisible);

	//--------------------------------------------------------------------
	// Set object visible state while editting
	//--------------------------------------------------------------------
	void SetVisibleInEditor(std::string& i_ObjectName, bool i_bVisible);

	//--------------------------------------------------------------------
	// Get object visible state while editting
	//--------------------------------------------------------------------
	bool GetVisibleInEditor(std::string& i_ObjectName);

	//--------------------------------------------------------------------
	//	RegisterVisibleInterest() - add a Visible interest to the system
	//--------------------------------------------------------------------
	void RegisterVisibleInterest( visVisibleInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterVisibleInterest() - remove a Visible interest from the system.
	//
	//	Note: this will NOT delete the Visible interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterVisibleInterest( visVisibleInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	Clear() - clear the interest list
	//--------------------------------------------------------------------
	void Clear();

};
