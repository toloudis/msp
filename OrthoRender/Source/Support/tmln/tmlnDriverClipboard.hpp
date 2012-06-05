/*****************************************************************************
**	tmlnDriverClipboard.hpp
**
**	This namespace manages the storing of drivers used in copy and paste
**	operations.
**
**	The clipboard takes in a driver (which it doesn't keep, but will create 
**	a driver info structure that it owns.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERCLIPBOARD_HPP
#error tmlnDriverClipboard.hpp multiply included
#endif
#define TMLN_DRIVERCLIPBOARD_HPP


//============================================================================
//	forward references
//============================================================================
class tmlnDriver;
class tmlnDriverInfo;
class tmlnScriptObject;


//============================================================================
//============================================================================
namespace tmlnDriverClipboard
{
	//------------------------------------------------------------------------
	//	Copy - copy a driver to the clipboard
	//------------------------------------------------------------------------
	void Copy(tmlnDriver *i_pDriver);

	//------------------------------------------------------------------------
	//	Paste - Create drivers and add to the object
	//------------------------------------------------------------------------
	void Paste(tmlnScriptObject *i_pObject);

	//------------------------------------------------------------------------
	//	Clear - Clear out the entire clipboard
	//------------------------------------------------------------------------
	void Clear();
	void Clear(tmlnDriver *i_pDriver);

	//------------------------------------------------------------------------
	//	Count - Get the number of items in the clipboard
	//------------------------------------------------------------------------
	int Count();

	//------------------------------------------------------------------------
	//	GetDriver - Get a specific driver
	//------------------------------------------------------------------------
	tmlnDriverInfo* GetDriverInfo(const int i_DriverIndex);
};
