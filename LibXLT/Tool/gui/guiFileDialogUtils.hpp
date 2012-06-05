/********************************************************************************************\
**	guiFileDialogUtils.hpp
**
**		Utilities for opening/saving files using fsLocators
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#ifdef	GUI_FILEDIALOGUTILS_HPP
#error	guiFileDialogUtils.hpp included recursively.
#endif
#define	GUI_FILEDIALOGUTILS_HPP

#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
// forward declaration
//============================================================================
class fsLocator;
class guiFileDialogUtilsImpl;


//============================================================================
// static functions define API
//============================================================================
class guiFileDialogUtils : public envAbstraction<guiFileDialogUtilsImpl>
{
public:
	//----------------------------------------------------------------------------
	//	GetOpenFileName() - returns result in o_ChosenLocator.
	//		returns true if a file was chosen, false if cancelled
	//		was pressed.
	//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
	//----------------------------------------------------------------------------
	static bool GetOpenFileName(const std::string &i_Filter,
						 const fsLocator &i_InitialDir,
						 fsLocator &o_ChosenLocator );

	//----------------------------------------------------------------------------
	// This variation of GetOpenFileName uses the directory category to set
	// the initial directory and updates the directory category when a file 
	// is chosen.
	//----------------------------------------------------------------------------
	static bool GetOpenFileName(const std::string &i_Filter,
						 const std::string &i_DirectoryCategory,
						 fsLocator &o_ChosenLocator );

	//----------------------------------------------------------------------------
	//	GetOpenFileNames() - returns multiple locators in o_ChosenLocators.
	//		returns true if a file was chosen, false if cancelled
	//		was pressed.
	//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
	//----------------------------------------------------------------------------
	static bool GetOpenFileNames(const std::string &i_Filter,
								 const fsLocator &i_InitialDir,
								 std::vector<fsLocator> &o_ChosenLocators );

	//----------------------------------------------------------------------------
	// This variation of GetOpenFileName uses the directory category to set
	// the initial directory and updates the directory category when a file 
	// is chosen.
	//----------------------------------------------------------------------------
	static bool GetOpenFileNames(const std::string &i_Filter,
						 const std::string &i_DirectoryCategory,
						 std::vector<fsLocator> &o_ChosenLocators );

	//----------------------------------------------------------------------------
	//	GetSaveFileName() - returns result in o_ChosenLocator.
	//		returns true if a file was chosen, false if cancelled
	//		was pressed.
	//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
	//----------------------------------------------------------------------------
	static bool GetSaveFileName(const std::string &i_Filter, fsLocator &io_ChosenLocator);
};

//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiFileDialogUtilsImpl
{
public:
	//----------------------------------------------------------------------------
	//	GetOpenFileName() - returns result in o_ChosenLocator.
	//		returns true if a file was chosen, false if cancelled
	//		was pressed.
	//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
	//----------------------------------------------------------------------------
	virtual bool GetOpenFileName(const std::string &i_Filter,
								 const fsLocator &i_InitialDir,
								 fsLocator &o_ChosenLocator ) = 0;

	//----------------------------------------------------------------------------
	//	GetOpenFileNames() - returns multiple locators in o_ChosenLocators.
	//		returns true if a file was chosen, false if cancelled
	//		was pressed.
	//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
	//----------------------------------------------------------------------------
	virtual bool GetOpenFileNames(const std::string &i_Filter,
								  const fsLocator &i_InitialDir,
								  std::vector<fsLocator> &o_ChosenLocators ) = 0;

	//----------------------------------------------------------------------------
	//	GetSaveFileName() - returns result in o_ChosenLocator.
	//		returns true if a file was chosen, false if cancelled
	//		was pressed.
	//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
	//----------------------------------------------------------------------------
	virtual bool GetSaveFileName(const std::string &i_Filter, 
								 fsLocator &io_ChosenLocator) = 0;
};
