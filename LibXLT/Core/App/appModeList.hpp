/*****************************************************************************
**  appModeList.hpp
**
**      appModeList maintains a list of several Mayhem appModes.  This is
**	useful for modes which wish to transfer to other modes (but don't want
**	to create circular dependencies in the rode).  For instance, the Mayhem
**	editor can instantly return to the main menu by pushing the e_MainMenu
**	mode on the appModeMgr.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_MODELIST_HPP
#error appModeList.hpp multiply included
#endif
#define APP_MODELIST_HPP


//============================================================================
//============================================================================
class appMode;


//====================================================================
//====================================================================
namespace appModeList
{
	enum
	{
		e_UserBegin = 0,
	};

	//--------------------------------------------------------------------
	//	Get the given mode.
	//--------------------------------------------------------------------
	appMode *GetMode(int i_ModeIndex);

	//--------------------------------------------------------------------
	//	Get the index for the given mode.
	//--------------------------------------------------------------------
	int GetModeIndex(appMode* i_Mode);

	//--------------------------------------------------------------------
	//	Set the given mode.  Probably only the appApp should use this
	//	function.
	//--------------------------------------------------------------------
	void SetMode(int i_ModeIndex, appMode* i_Mode);

	//------------------------------------------------------------------------
	//	Init must be called before you use the env package.  A good place to
	//	do this is in your main function, before you do anything else.
	//
	//	i_NumModes is the count of modes to be supported by this mode list.
	// you may never index to or set to a mode outside of this initialized max.
	//------------------------------------------------------------------------
	void Init(int i_NumModes);

	//------------------------------------------------------------------------
	//	CleanUp should be called after you are done with the env package.
	//	A good place to do this is in your main function, after you are done
	//	with other deinitialization and cleanup tasks.
	//	The CleanUp function is written with the throw() exception
	//	to suggest that it should not throw any exceptions, since typically
	//	the caller is in the process of de-initializing and won't be able
	//	to do much with them.
	//------------------------------------------------------------------------
	void CleanUp() throw();

}
