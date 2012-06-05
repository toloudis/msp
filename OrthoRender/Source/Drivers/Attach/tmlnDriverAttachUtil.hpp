/*****************************************************************************
**	tmlnDriverAttachUtil.hpp
**
**	Utility for finding object by name. 
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_DRIVERATTACHUTIL_HPP
#error tmlnDriverAttachUtil.hpp multiply included
#endif
#define TMLN_DRIVERATTACHUTIL_HPP


//============================================================================
//============================================================================
class nameObject;
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
	//	Returns object that matches nameString. May display a dialog
	//	if object is not found. May return NULL.
	//--------------------------------------------------------------------
	nameObject* GetObjectByName( const nameString& i_String );

}	// end of namespace
