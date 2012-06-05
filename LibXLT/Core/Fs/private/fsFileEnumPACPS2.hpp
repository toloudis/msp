/****************************************************************************\
**  fsFileEnumPACPS2.hpp
**
**      fsFileEnumPACPS2.hpp is the interface to the PS2s pac for the 
**	fsFileEnum component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef FS_FILEENUMPACPS2_HPP
#error fsFileEnum.hpp multiply included
#endif
#define FS_FILEENUMPACPS2_HPP

#ifndef FS_FILEENUM_HPP
#include "fsFileEnum.hpp"
#endif


//============================================================================
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
namespace fsFileEnumPAC
{

//------------------------------------------------------------------------
//	EnumerateFiles lists all of the files in a directory.  If i_SubString
//	is not NULL, only files whose names contain the given substring
//	will be listed. Returns the last return value from fsEnumTarget::Notify
//------------------------------------------------------------------------
bool EnumerateFiles(const fsLocator& i_Locator, 
					fsFileEnum::EnumTarget& o_Target,
					const itString* i_SubString = NULL);

//------------------------------------------------------------------------
//	EnumerateDirectories lists all of the subdirectories in a directory.
//	Returns the last return value from fsEnumTarget::Notify
//------------------------------------------------------------------------
bool EnumerateDirectories(	const fsLocator& i_Locator, 
							fsFileEnum::EnumTarget& o_Target);

//------------------------------------------------------------------------
//	GetDrives lists all of the drives listed.  This isn't
//	really useful on PS2, right now it will return cdrom0. 
//------------------------------------------------------------------------
void GetDrives(std::vector<fsLocator>& o_DirList);

}
