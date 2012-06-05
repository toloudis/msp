/****************************************************************************\
**  itLocaleUtilPACPS2.hpp
**
**      itLocaleUtilPACPS2.hpp defines the locale util PAC for the PS2.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IT_LOCALEUTILPACPS2_HPP
#error itLocaleUtilPACPS2.hpp multiply included
#endif
#define IT_LOCALEUTILPACPSW_HPP

#ifndef IT_LOCALES_HPP
#include "itLocales.hpp"
#endif

namespace itLocaleUtilPAC
{
	itLocales::Locale GetSystemLocale();
}
