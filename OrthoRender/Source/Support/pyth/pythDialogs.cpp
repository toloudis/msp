/****************************************************************************\
**	pythDialogs.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythDialogs.hpp"

#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"

#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsLocator.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMessageBox.hpp"


// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)

namespace
{
	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Display message in dialog
	//--------------------------------------------------------------------
	PyObject *
	message_box(PyObject *self, PyObject *args)
	{
		const char *message, *title;
		if (!PyArg_ParseTuple(args, "ss", &message, &title))
			return NULL;

		guiMessageBox::Show(message, title);

		return pythFunctionUtil::ReturnNone();
	}

	//--------------------------------------------------------------------
	// Display yes/no dialog and return true if yes is pressed
	//--------------------------------------------------------------------
	PyObject *
	yes_no_dialog(PyObject *self, PyObject *args)
	{
		const char *message, *title;
		if (!PyArg_ParseTuple(args, "ss", &message, &title))
			return NULL;

		if (guiMessageBox::Show(message, title, guiMessageBox::e_YesNo) == guiMessageBox::e_Yes)
		{
			return pythFunctionUtil::ConvertBoolean(true);
		}

		return pythFunctionUtil::ConvertBoolean(false);
	}

	//--------------------------------------------------------------------
	// Returns filename chosen through open file selection dialog.
	//--------------------------------------------------------------------
	PyObject *
	open_file_dialog(PyObject *self, PyObject *args)
	{
		const char *filter, *initDir;
		if (!PyArg_ParseTuple(args, "ss", &filter, &initDir))
			return NULL;

		fsLocator init_loc;
		fsFileUtil::ANSIFilenameToLocator(initDir, init_loc);

		fsLocator file_loc;
		if (guiFileDialogUtils::GetOpenFileName(filter, init_loc, file_loc))
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(file_loc, filename);
			return pythFunctionUtil::ConvertString(filename);
		}

		return pythFunctionUtil::ConvertString("");
	}

	//--------------------------------------------------------------------
	// Returns filenames chosen through open file selection dialog.
	//--------------------------------------------------------------------
	PyObject *
	open_files_dialog(PyObject *self, PyObject *args)
	{
		const char *filter, *initDir;
		if (!PyArg_ParseTuple(args, "ss", &filter, &initDir))
			return NULL;

		fsLocator init_loc;
		fsFileUtil::ANSIFilenameToLocator(initDir, init_loc);

		std::vector<fsLocator> file_locs;
		guiFileDialogUtils::GetOpenFileNames(filter, init_loc, file_locs);
		
		const int num_files = file_locs.size();
		PyObject* FileList = PyList_New(num_files);
		if (FileList != NULL)
		{
			for (int i=0; i<num_files; ++i)
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(file_locs[i], filename);
				PyList_SetItem(FileList, i, PyString_FromString(filename.c_str()));
			}
		}
		
		return FileList;
	}

	//--------------------------------------------------------------------
	// Returns filename chosen through save file selection dialog.
	//--------------------------------------------------------------------
	PyObject *
	save_file_dialog(PyObject *self, PyObject *args)
	{
		const char *filter, *defaultFilename;
		if (!PyArg_ParseTuple(args, "ss", &filter, &defaultFilename))
			return NULL;

		fsLocator file_loc;
		fsFileUtil::ANSIFilenameToLocator(defaultFilename, file_loc);
		if (guiFileDialogUtils::GetSaveFileName(filter, file_loc))
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(file_loc, filename);
			return pythFunctionUtil::ConvertString(filename);
		}

		return pythFunctionUtil::ConvertString("");
	}


}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void pythDialogs::AddCommands(const std::string &i_ModuleName)
{
	pythModules::AddCommand(i_ModuleName, "messageBox", 
		"Displays message in dialog."
		"	messageBox(message, title)", 
		message_box);
	pythModules::AddCommand(i_ModuleName, "yesNoDialog", 
		"Asks yes/no question of user, returns true if Yes is pressed."
		"	yesNoDialog(question, title) returns True or False", 
		yes_no_dialog);

	pythModules::AddCommand(i_ModuleName, "openFileDialog", 
		"Opens dialog to select a file to open."
		"	openFileDialog(filter, initialDirectory) returns filename or \"\"", 
		open_file_dialog);
	pythModules::AddCommand(i_ModuleName, "openFilesDialog", 
		"Opens dialog to select multiple files."
		"	openFilesDialog(filter, initialDirectory) returns list of filenames possibly empty", 
		open_files_dialog);
	pythModules::AddCommand(i_ModuleName, "saveFileDialog", 
		"Opens dialog to select a file to save."
		"	saveFileDialog(filter, initialFilename) returns filename or \"\"", 
		save_file_dialog);

}

#endif
