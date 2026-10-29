/****************************************************************************\
**  fsFileTimeUtilPAC.hpp
**
**      fsFileTimeUtilPAC.hpp forwards calls from the fsFileTimeUtil to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef FS_FILETIMEUTILPAC_HPP
#error fsFileTimeUtilPAC.hpp multiply included
#endif
#define FS_FILETIMEUTILPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

// PAC components should support the following interface:
//
/*namespace fsFileTimeUtilPAC
//{
	double GetCreationTime(const fsLocator& i_Locator);
	double GetLastAccessTime(const fsLocator& i_Locator);
	double GetLastModifiedTime(const fsLocator& i_Locator);
	time_t GetLastModifiedTimeT(const fsLocator& i_Locator);
	std::string GetCreationString(const fsLocator& i_Locator);
	std::string GetLastAccessString(const fsLocator& i_Locator);
	std::string GetLastModifiedString(const fsLocator& i_Locator);
	std::string GetCreationStringWithTime(const fsLocator& i_Locator);
	std::string GetLastAccessStringWithTime(const fsLocator& i_Locator);
	std::string GetLastModifiedStringWithTime(const fsLocator& i_Locator);
	void Init();
	void CleanUp() throw();
}*/

#if ENV_WINDOWS
	#include "Core/fs/private/fsFileTimeUtilPACWin.hpp"
#else
	#error fsFileTimeUtilPAC not defined for this platform
#endif
