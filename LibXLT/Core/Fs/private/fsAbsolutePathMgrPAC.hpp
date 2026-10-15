/****************************************************************************\
**  fsAbsolutePathMgrPAC.hpp
**
**      fsAbsolutePathMgrPAC.hpp forwards calls from the fsAbsolutePathMgr
**	to the correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef FS_ABSOLUTEPATHMGRPAC_HPP
#error fsAbsolutePathMgrPAC.hpp multiply included
#endif
#define FS_ABSOLUTEPATHMGRPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

// PAC components should support the following interface:
//
/*namespace fsAbsolutePathMgrPAC
//{
	bool FindInStandardDirectories(const fsLocator& i_OrigFilename, fsLocator& o_LocalFilename);
}*/

#if ENV_WINDOWS
	#include "Core/fs/private/fsAbsolutePathMgrPACWin.hpp"
#else
	#error fsAbsolutePathMgrPAC not defined for this platform
#endif
