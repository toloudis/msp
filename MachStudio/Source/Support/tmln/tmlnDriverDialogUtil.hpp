/*****************************************************************************
**	tmlnDriverDialogUtil.hpp
**
**	Maintains driver properties dialog
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERDIALOGUTIL_HPP
#error tmlnDriverDialogUtil.hpp multiply included
#endif
#define TMLN_DRIVERDIALOGUTIL_HPP

class tmlnDriver;

//============================================================================
//============================================================================
namespace tmlnDriverDialogUtil 
{
	//--------------------------------------------------------------------
	// Clear the driver properties dialog 
	//	(meaning no driver is currently selected)
	//--------------------------------------------------------------------
	void ClearDriverProperties();

	//--------------------------------------------------------------------
	//  The given driver is going to be selected soon, if this
	// is going to change the current driver selection, then
	// clear out old properties.
	//--------------------------------------------------------------------
	void PrepareForSelection(tmlnDriver *i_pDriver);

	//--------------------------------------------------------------------
	//  Show dialog that allows user to edit this driver's properties
	//--------------------------------------------------------------------
	void  EditDriverProperties(tmlnDriver *i_pDriver);
}
