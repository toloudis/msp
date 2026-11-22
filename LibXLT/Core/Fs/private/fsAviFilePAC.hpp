/****************************************************************************\
**  fsAviFilePAC.hpp
**
**      fsAviFilePAC.hpp forwards calls from the fsAviFile to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#ifdef FS_AVIFILEPAC_HPP
#error fsAviFilePAC.hpp multiply included
#endif
#define FS_AVIFILEPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

// PAC components should support the same interface as fsAviFile
// (see Core/fs/fsAviFile.hpp), in the namespace fsAviFilePAC.

#if ENV_WINDOWS
	#include "Core/fs/private/fsAviFilePACWin.hpp"
#else
	#error fsAviFilePAC not defined for this platform
#endif
