/****************************************************************************\
**	rndrPrefsDataInterest.hpp
**
**		An interest related to preferences.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef RNDRPREFSDATAINTEREST_HPP
#error rndrPrefsDataInterest.hpp multiply included
#endif
#define RNDRPREFSDATAINTEREST_HPP

#ifndef RPRFPREFSDATA_HPP
#include "Support/rprf/rprfPrefsData.hpp"
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
		virtual void PrefsDataUpdated(const rprfPrefsData& i_Data) = 0;
};

