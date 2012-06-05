/****************************************************************************\
**	prefsQuickDataInterest.hpp
**
**		An interest related to preferences.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PREFS_QUICKDATAINTEREST_HPP
#error prefsQuickDataInterest.hpp multiply included
#endif
#define PREFS_QUICKDATAINTEREST_HPP

#ifndef PREFS_QUICKDATA_HPP
#include "Features/Prefs/prefsQuickData.hpp"
#endif


//============================================================================
//============================================================================
class prefsQuickDataInterest
{
	public:
		//--------------------------------------------------------------------
		//	prefsQuickDataUpdated - the data was updated
		//--------------------------------------------------------------------
		virtual void prefsQuickDataUpdated(const prefsQuickData& i_Data) = 0;
};

