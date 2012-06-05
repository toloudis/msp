/*****************************************************************************
**  GetFilename.hpp
**
**     Command to open file dialog to get filename from user   
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GETFILENAME_HPP
#error GetFilename.hpp multiply included
#endif
#define GETFILENAME_HPP

#include <maya/MPxCommand.h>

class GetFilenameDialog: public MPxCommand
{
public:
	GetFilenameDialog() 
	{
		fileExt = "*.*";
	};
	virtual	~GetFilenameDialog() {}
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );
private:
	virtual MStatus	ParseArgs( const MArgList& );
	MString	filePath;
	MString	fileExt;
};
