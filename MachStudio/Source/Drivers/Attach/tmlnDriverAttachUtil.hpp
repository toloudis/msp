/*****************************************************************************
**	tmlnDriverAttachUtil.hpp
**
**	Utility for finding object by name. 
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_DRIVERATTACHUTIL_HPP
#error tmlnDriverAttachUtil.hpp multiply included
#endif
#define TMLN_DRIVERATTACHUTIL_HPP

#include <vector>

//============================================================================
//============================================================================
class nameObject;
class nameString;
class nameString;


//============================================================================
//============================================================================
namespace tmlnDriverAttachUtil
{
	//--------------------------------------------------------------------
	// In batch mode, call this function to disable the dialogs.
	//--------------------------------------------------------------------
	void SetAllowDialogs(bool i_bAllow);

	//--------------------------------------------------------------------
	// Clear out names that have been cached from previous dialogs.
	// Should be cleared when reading a new file.
	//--------------------------------------------------------------------
	void ClearCache();

	//--------------------------------------------------------------------
	// Get list of names that can be attached to.
	//--------------------------------------------------------------------
	void GetNameList(std::vector<nameString>& o_NameList);

	//--------------------------------------------------------------------
	//	Returns object that matches nameString. May display a dialog
	//	if object is not found. May return NULL.
	//--------------------------------------------------------------------
	nameObject* GetObjectByName( const nameString& i_String, bool i_bPromptIfMissing = true );

}	// end of namespace
