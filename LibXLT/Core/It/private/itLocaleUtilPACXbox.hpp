/****************************************************************************\
**  itLocaleUtilPACXbox.hpp
**
**      itLocaleUtilPACXbox.hpp defines the locale util PAC for Xbox.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IT_LOCALEUTILPACXBOX_HPP
#error itLocaleUtilPACXbox.hpp multiply included
#endif
#define IT_LOCALEUTILPACXBOX_HPP

#ifndef IT_LOCALES_HPP
#include "itLocales.hpp"
#endif


//============================================================================
//============================================================================
namespace itLocaleUtilPAC
{
	itLocales::Locale GetSystemLocale();
}
