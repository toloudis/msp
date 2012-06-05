/****************************************************************************\
**  fsFileTimeUtilPACPS2.cpp
**
**      fsFileTimeUtilPACPS2.cpp defines the file time util PAC for 
**	the PS2.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "fsFileTimeUtilPACPS2.hpp"

#include "fsFileUtilPACPS2.hpp"

//#include "dbgAssert.hpp"
#include "dbgLog.hpp"
#include "fsFileX.hpp"

#include <sifdev.h>

namespace fsFileTimeUtilPAC
{

namespace
{

void handle_file_error(int i_ErrorNumber, const fsLocator& i_Locator)
{
	switch( i_ErrorNumber )
	{	
		case -SCE_ENXIO:		// No such device or address
			DBG_LOG0("SCE_ENXIO");
		break;
		case -SCE_EBADF:		// Bad file number 
			DBG_LOG0("SCE_EBADF");
		break;
		case -SCE_ENODEV:		// No such device 
			DBG_LOG0("SCE_ENODEV");
		break;
		case -SCE_EINVAL:		// Invalid argument 
			DBG_LOG0("SCE_EINVAL");
		break;
		case -SCE_EMFILE:		// Too many open files 
			DBG_LOG0("SCE_EMFILE");
		break;
		default:
			throw fsUnknownX(i_Locator);
		break;
	}
	
	throw fsUnknownX(i_Locator);
}

}

//========================================================================
//	GetCreationTime returns a double in UTC (Coordinated Universal Time)
//	i.e. the number of seconds since January 1, 1970, 00:00:00 GMT which
//	represents the file's creation time
//========================================================================
double GetCreationTime(const fsLocator& i_Locator)
{
	DBG_ASSERT0(false, "Implement me! (if necessary)");
	return -1.0f;
}

//========================================================================
//	GetLastAccessTime returns a double in UTC (Coordinated Universal Time)
//	i.e. the number of seconds since January 1, 1970, 00:00:00 GMT which
//	represents the file's last access time
//========================================================================
double GetLastAccessTime(const fsLocator& i_Locator)
{
	DBG_ASSERT0(false, "Implement me! (if necessary)");
	return -1.0f;
}

//========================================================================
//	GetLastModifiedTime returns a double in UTC (Coordinated Universal Time)
//	i.e. the number of seconds since January 1, 1970, 00:00:00 GMT which
//	represents the file's last modified time
//========================================================================
double GetLastModifiedTime(const fsLocator& i_Locator)
{
	DBG_ASSERT0(false, "Implement me! (if necessary)");
	return -1.0f;
}

//========================================================================
//	GetCreationString returns the in an itstring of the form 
//	mm/dd/yy hh:mm 
//========================================================================
itString GetCreationString(const fsLocator& i_Locator)
{
	DBG_ASSERT0(false, "Implement me! (if necessary)");
	return itString("Not implemented");
}

//========================================================================
//	GetLastAccessString returns the in an itstring of the form 
//	mm/dd/yy hh:mm 
//========================================================================
itString GetLastAccessString(const fsLocator& i_Locator)
{
	DBG_ASSERT0(false, "Implement me! (if necessary)");
	return itString("Not implemented");
}

//========================================================================
//	GetLastModifiedString returns the in an itstring of the form 
//	mm/dd/yy hh:mm 
//========================================================================
itString GetLastModifiedString(const fsLocator& i_Locator)
{
	DBG_ASSERT0(false, "Implement me! (if necessary)");
	return itString("Not implemented");
}

//========================================================================
//	GetCreationString returns the in an itstring of the form 
//	mm/dd/yy hh:mm 
//========================================================================
itString GetCreationStringWithTime(const fsLocator& i_Locator)
{
	DBG_ASSERT0(false, "Implement me! (if necessary)");
	return itString("Not implemented");
}

//========================================================================
//	GetLastAccessString returns the in an itstring of the form 
//	mm/dd/yy hh:mm 
//========================================================================
itString GetLastAccessStringWithTime(const fsLocator& i_Locator)
{
	DBG_ASSERT0(false, "Implement me! (if necessary)");
	return itString("Not implemented");
}

//========================================================================
//	GetLastModifiedString returns the in an itstring of the form 
//	mm/dd/yy hh:mm 
//========================================================================
itString GetLastModifiedStringWithTime(const fsLocator& i_Locator)
{
	DBG_ASSERT0(false, "Implement me! (if necessary)");
	return itString("Not implemented");
}

//========================================================================
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//========================================================================
void Init()
{
}

void CleanUp() throw()
{
}

}
