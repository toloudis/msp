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
	itString GetCreationString(const fsLocator& i_Locator);
	itString GetLastAccessString(const fsLocator& i_Locator);
	itString GetLastModifiedString(const fsLocator& i_Locator);
	itString GetCreationStringWithTime(const fsLocator& i_Locator);
	itString GetLastAccessStringWithTime(const fsLocator& i_Locator);
	itString GetLastModifiedStringWithTime(const fsLocator& i_Locator);
	void Init();
	void CleanUp() throw();
}*/

#if ENV_WINDOWS
	#include "Core/fs/private/fsFileTimeUtilPACWin.hpp"
#else
	#if ENV_OS == ENV_PS2OS
		#include "Core/fs/private/fsFileTimeUtilPACPS2.hpp"
	#else
		#if ENV_OS == ENV_XBOXOS
			#include "Core/fs/private/fsFileTimeUtilPACXbox.hpp"
		#else
			#error fsFileTimeUtilPAC not defined for this platform
		#endif
	#endif
#endif
