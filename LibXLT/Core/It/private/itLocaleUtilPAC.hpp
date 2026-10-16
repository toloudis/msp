/****************************************************************************\
**  itLocaleUtilPAC.hpp
**
**      itLocaleUtilPAC.hpp forwards calls from the itLocaleUtil to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IT_LOCALEUTILPAC_HPP
#error itLocaleUtilPAC.hpp multiply included
#endif
#define IT_LOCALEUTILPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

// PAC components should support the following interface:
//
//namespace itLocaleUtilPAC
//{
//	itLocales::Locale GetSystemLocale();
//}

#if ENV_WINDOWS
#include "Core/it/private/itLocaleUtilPACWin.hpp"
#else
	#error itLocaleUtilPACWin not defined for this platform
#endif
