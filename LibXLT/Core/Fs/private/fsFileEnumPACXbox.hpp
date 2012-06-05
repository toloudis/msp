/****************************************************************************\
**  fsFileEnumPACWin.hpp
**
**      fsFileEnumPACWin.hpp is the interface to the Xbox pac for the 
**	fsFileEnum component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef FS_FILEENUMPACXBOX_HPP
#error fsFileEnum.hpp multiply included
#endif
#define FS_FILEENUMPACXBOX_HPP

class fsLocator;

#ifndef FS_FILEENUM_HPP
#include "fsFileEnum.hpp"
#endif

namespace fsFileEnumPAC
{

//========================================================================
//	EnumerateFiles lists all of the files in a directory.  If i_SubString
//	is not NULL, only files whose names contain the given substring
//	will be listed. Returns the last return value from fsEnumTarget::Notify
//========================================================================
bool EnumerateFiles(const fsLocator& i_Locator, 
					fsFileEnum::EnumTarget& o_Target,
					const itString* i_SubString = NULL);

//========================================================================
//	EnumerateDirectories lists all of the subdirectories in a directory.
//	Returns the last return value from fsEnumTarget::Notify
//========================================================================
bool EnumerateDirectories(	const fsLocator& i_Locator, 
							fsFileEnum::EnumTarget& o_Target);

//========================================================================
//	GetDrives lists all of the drives listed by windows
//========================================================================
void GetDrives(std::vector<fsLocator>& o_DirList);

}
