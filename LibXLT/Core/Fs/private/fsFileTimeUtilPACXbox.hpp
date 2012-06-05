/****************************************************************************\
**  fsFileTimeUtilPACXbox.hpp
**
**      fsFileTimeUtilPACXbox.hpp defines the file util PAC for Xbox.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef FS_FILETIMEUTILPACXBOX_HPP
#error fsFileTimeUtilPACXbox.hpp multiply included
#endif
#define FS_FILETIMEUTILPACXBOX_HPP

#include <string>

class itString;
class fsLocator;

namespace fsFileTimeUtilPAC
{
	//========================================================================
	//	GetCreationTime returns a double in UTC (Coordinated Universal Time)
	//	i.e. the number of seconds since January 1, 1970, 00:00:00 GMT which
	//	represents the file's creation time
	//========================================================================
	double GetCreationTime(const fsLocator& i_Locator);

	//========================================================================
	//	GetLastAccessTime returns a double in UTC (Coordinated Universal Time)
	//	i.e. the number of seconds since January 1, 1970, 00:00:00 GMT which
	//	represents the file's last access time
	//========================================================================
	double GetLastAccessTime(const fsLocator& i_Locator);

	//========================================================================
	//	GetLastModifiedTime returns a double in UTC (Coordinated Universal Time)
	//	i.e. the number of seconds since January 1, 1970, 00:00:00 GMT which
	//	represents the file's last modified time
	//========================================================================
	double GetLastModifiedTime(const fsLocator& i_Locator);

	//========================================================================
	//	GetCreationString returns the in an itstring of the form 
	//	mm/dd/yy hh:mm 
	//========================================================================
	itString GetCreationString(const fsLocator& i_Locator);

	//========================================================================
	//	GetLastAccessString returns the in an itstring of the form 
	//	mm/dd/yy hh:mm 
	//========================================================================
	itString GetLastAccessString(const fsLocator& i_Locator);

	//========================================================================
	//	GetLastModifiedString returns the in an itstring of the form 
	//	mm/dd/yy hh:mm 
	//========================================================================
	itString GetLastModifiedString(const fsLocator& i_Locator);

	//========================================================================
	//	GetCreationString returns the in an itstring of the form 
	//	mm/dd/yy hh:mm 
	//========================================================================
	itString GetCreationStringWithTime(const fsLocator& i_Locator);

	//========================================================================
	//	GetLastAccessString returns the in an itstring of the form 
	//	mm/dd/yy hh:mm 
	//========================================================================
	itString GetLastAccessStringWithTime(const fsLocator& i_Locator);

	//========================================================================
	//	GetLastModifiedString returns the in an itstring of the form 
	//	mm/dd/yy hh:mm 
	//========================================================================
	itString GetLastModifiedStringWithTime(const fsLocator& i_Locator);

	//========================================================================
	//	Don't call Init() and CleanUp() yourself; they are called 
	//	by the package Init and Cleanup.
	//========================================================================
	void Init();
	void CleanUp() throw();
}

