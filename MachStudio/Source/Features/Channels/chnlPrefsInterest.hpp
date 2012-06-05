/*****************************************************************************
**  chnlPrefsInterest.hpp
**
**      the Prefs interest for the channel editor package.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef CHNL_PREFSINTEREST_HPP
#error chnlPrefsInterest.hpp multiply included
#endif
#define CHNL_PREFSINTEREST_HPP

#include "Features/Prefs/prefsDataInterest.hpp"


//============================================================================
//============================================================================
class chnlPrefsInterest : public prefsDataInterest
{
	public:
		//--------------------------------------------------------------------
		//	PrefsDataUpdated - the data was updated
		//--------------------------------------------------------------------
		virtual void PrefsDataUpdated(const PrefsData& i_Data);
};
