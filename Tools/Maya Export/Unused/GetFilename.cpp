/*****************************************************************************
**  GetFilename.cpp
**
**     Command to open file dialog to get filename from user        
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <GetFilename.hpp>
#include <SceneFuncs.hpp>

#include <maya/MArgList.h>
#include <maya/MFileIO.h>

#include <windows.h>

namespace
{
OPENFILENAME l_ofn;
}

void *GetFilenameDialog::creator()
{
	// Setup open filename structure
	//
	memset(&l_ofn, 0, sizeof(OPENFILENAME));
	l_ofn.lStructSize	= sizeof(l_ofn);

	l_ofn.lpstrFile		= new char [_MAX_PATH];
	l_ofn.lpstrFile[0]	= 0;
	l_ofn.nMaxFile		= _MAX_PATH;
	
	l_ofn.nFilterIndex	= 0;
	l_ofn.lpstrCustomFilter = NULL;
	l_ofn.nMaxCustFilter	= 0;

	cout << "Current File: " << MFileIO::currentFile() << endl;

	char *init_dir = new char[_MAX_DIR];
	//GetCurrentDirectory(_MAX_DIR, init_dir);
	strcpy(init_dir, MFileIO::currentFile().asChar());
	char *ptr = strrchr(init_dir,'/');
	if (ptr) *ptr = '\0';

	l_ofn.lpstrInitialDir = init_dir;

	l_ofn.Flags |= (OFN_HIDEREADONLY | OFN_PATHMUSTEXIST);

	// Return new instance of command
	//
	return new GetFilenameDialog();
}

MStatus GetFilenameDialog::ParseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	fileFlag				("-f");
	const MString	fileFlagLong			("-File");

	const MString	extFlag				("-e");
	const MString	extFlagLong			("-ext");

	// Parse the arguments.
	for ( size_t i = 0; i < args.length(); i++ ) {
		arg = args.asString( i, &stat );
		if (!stat)              
			continue;
				
		if ( arg == fileFlag || arg == fileFlagLong ) {
			filePath = args.asString(i + 1, &stat);
			i++;
		}
		else if ( arg == extFlag || arg == extFlagLong ) {
			fileExt = args.asString(i + 1, &stat);
			i++;
		}
		else {
			//just ignore extraneous args
		}
	}
	return stat;
}

MStatus GetFilenameDialog::doIt(const MArgList &args)
{
	filePath.clear();

	if(!ParseArgs(args))
		return MS::kFailure; 
/*
	typedef struct tagOFN 
	{ // ofn     
		DWORD         lStructSize; 
	    HWND          hwndOwner;
		HINSTANCE     hInstance; 
	    LPCTSTR       lpstrFilter;
		LPTSTR        lpstrCustomFilter; 
	    DWORD         nMaxCustFilter;
		DWORD         nFilterIndex; 
	    LPTSTR        lpstrFile;
		DWORD         nMaxFile; 
	    LPTSTR        lpstrFileTitle;
		DWORD         nMaxFileTitle; 
	    LPCTSTR       lpstrInitialDir;
		LPCTSTR       lpstrTitle; 
	    DWORD         Flags;
		WORD          nFileOffset; 
	    WORD          nFileExtension;
		LPCTSTR       lpstrDefExt; 
	    DWORD         lCustData;
		LPOFNHOOKPROC lpfnHook; 
	    LPCTSTR       lpTemplateName; 
	} OPENFILENAME; 
*/

	cout << "In GetFilenameDialog" << endl;

	l_ofn.hwndOwner		= NULL;
	l_ofn.lpstrTitle	= "Save File Dialog";
	l_ofn.lpstrDefExt	= (fileExt.asChar() + 1); // don't want period in extension

	// filter should look like this:
	//static char uva_save_filter[] =
	//   "Animation UV Data File (*.uva)\0*.uva\0All Files (*.*)\0*.*\0";

	// Use tabs to represent breaks in format
	const char *Format = "%s\t%s\tAllFiles (*.*)\t*.*\t";
	char *pBuffer = new char[strlen(Format) + 2 * fileExt.length() + 1];
	sprintf(pBuffer, Format, fileExt.asChar(), fileExt.asChar());
	// now convert tabs into "\0" 
	for (char *ptr=pBuffer; *ptr != '\0'; ptr++)
	{
		if (*ptr == '\t') *ptr = '\0';
	}
	l_ofn.lpstrFilter	= pBuffer;
	l_ofn.nFilterIndex	= 1L;

	// If we have a filename, put it into dialog
	if (filePath.length() > 0)
		strcpy(l_ofn.lpstrFile, filePath.asChar());

	BOOL hRet;
	DWORD err;
	if ((hRet = GetSaveFileName(&l_ofn)) == 0)
	{
		err = CommDlgExtendedError();

		cout << "Err return from GetFilenameDialog" << endl;
		delete [] pBuffer;
		setResult(MString(""));
		return MS::kFailure;
	}

	cout << "Got filename: " <<  l_ofn.lpstrFile << endl;
	MString result(l_ofn.lpstrFile);
	setResult(result);
	delete [] pBuffer;
	
	return MS::kSuccess;
}


