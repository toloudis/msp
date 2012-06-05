/****************************************************************************\
**  inTabletPAC.hpp
**
**      inTabletPAC.hpp forwards calls from the inTablet component to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_TABLETPAC_HPP
#error inTabletPAC.hpp multiply included
#endif
#define IN_TABLETPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#if ENV_WINDOWS
	#include "InputTPC/in/inTabletPACWin.hpp"
#endif