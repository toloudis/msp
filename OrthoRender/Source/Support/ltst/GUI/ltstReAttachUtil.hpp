/*****************************************************************************
**	ltstReAttachUtil.hpp
**
**	Utility for finding object by name. 
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef LTST_REATTACHUTIL_HPP
#error ltstReAttachUtil.hpp multiply included
#endif
#define LTST_REATTACHUTIL_HPP


//============================================================================
//============================================================================
class nameObject;
class nameString;


//============================================================================
//============================================================================
namespace ltstReAttachUtil
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
	nameObject* GetObjByName( const nameString& i_String, bool i_IsObject );

}	// end of namespace
