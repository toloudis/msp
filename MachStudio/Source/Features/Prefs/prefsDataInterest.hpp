/****************************************************************************\
**	prefsDataInterest.hpp
**
**		An interest related to preferences.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef PREFS_DATAINTEREST_HPP
#error prefsDataInterest.hpp multiply included
#endif
#define PREFS_DATAINTEREST_HPP

#ifndef PREFSDATA_HPP
#include "Features/Prefs/PrefsData.hpp"
#endif



//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class prefsDataInterest
{
	public:
		//--------------------------------------------------------------------
		//	PrefsDataUpdated - the data was updated
		//--------------------------------------------------------------------
		virtual void PrefsDataUpdated(const PrefsData& i_Data) = 0;
};

