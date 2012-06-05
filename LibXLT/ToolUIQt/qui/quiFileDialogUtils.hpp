/********************************************************************************************\
**  quiFileDialogUtils.hpp
**
**      Utilities for opening/saving files using fsLocators
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#ifdef	QUI_FILEDIALOGUTILS_HPP
#error	quiFileDialogUtils.hpp included recursively.
#endif
#define	QUI_FILEDIALOGUTILS_HPP


#ifndef GUI_FILEDIALOGUTILS_HPP
#include "Tool/gui/guiFileDialogUtils.hpp"
#endif

#include <vector>

//============================================================================
//	quiFileDialogUtils Functions
//============================================================================
class quiFileDialogUtils : public guiFileDialogUtilsImpl
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
								 fsLocator &o_ChosenLocator );

	//----------------------------------------------------------------------------
	//	GetOpenFileNames() - returns multiple locators in o_ChosenLocators.
	//		returns true if a file was chosen, false if cancelled
	//		was pressed.
	//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
	//----------------------------------------------------------------------------
	virtual bool GetOpenFileNames(const std::string &i_Filter,
								  const fsLocator &i_InitialDir,
								  std::vector<fsLocator> &o_ChosenLocators );

	//----------------------------------------------------------------------------
	//	GetSaveFileName() - returns result in o_ChosenLocator.
	//		returns true if a file was chosen, false if cancelled
	//		was pressed.
	//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
	//----------------------------------------------------------------------------
	virtual bool GetSaveFileName(const std::string &i_Filter, 
								 fsLocator &io_ChosenLocator);

};
