/****************************************************************************\
**	rndrPrefsDataInterest.hpp
**
**		An interest related to preferences.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef RNDRPREFSDATAINTEREST_HPP
#error rndrPrefsDataInterest.hpp multiply included
#endif
#define RNDRPREFSDATAINTEREST_HPP

#ifndef RNDRPREFSDATA_HPP
#include "Features/RenderPrefs/rndrPrefsData.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class rndrPrefsDataInterest
{
	public:
		//--------------------------------------------------------------------
		//	PrefsDataUpdated - the data was updated
		//--------------------------------------------------------------------
		virtual void PrefsDataUpdated(const rndrPrefsData& i_Data) = 0;
};

