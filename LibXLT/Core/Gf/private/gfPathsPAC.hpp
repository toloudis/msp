/****************************************************************************\
**  gfPathsPAC.hpp
**
**      gfPathsPAC.hpp forwards calls from the gfPaths to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GF_PATHSPAC_HPP
#error gfPathsPAC.hpp multiply included
#endif
#define GF_PATHSPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

// PAC components should support the following interface:
//
/*namespace gfPathsPAC
//{
	void InitPaths(std::vector<fsLocator>& o_Vec, const char* i_CDVolumeName);
}*/

#if ENV_WINDOWS
	#include "Core/gf/private/gfPathsPACWin.hpp"
#else
	#error gfPathsPAC not defined for this platform
#endif
