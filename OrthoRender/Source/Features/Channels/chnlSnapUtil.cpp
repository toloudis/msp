/*****************************************************************************
**	chnlSnapUtil.cpp
**
**	Utility for snapping times
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlSnapUtil.hpp"

#include "Features/Prefs/PrefsMgr.hpp"

//============================================================================
//============================================================================
namespace chnlSnapUtil
{
	//--------------------------------------------------------------------
	// TimesMatch - returns true if the two times are within
	//	the snap threshold.
	//--------------------------------------------------------------------
	bool TimesMatch( float i_Time1, float i_Time2 )
	{
		PrefsData& prefs_data = PrefsMgr::Data();

		float c_TIMEDIFF = 0.008f;
		if (prefs_data.m_ChannelEditor_SnapActive.GetValue())
			c_TIMEDIFF = prefs_data.m_ChannelEditor_SnapAmount.GetValue();

		return (   (i_Time1 >= (i_Time2 - c_TIMEDIFF))
				&& (i_Time1 <= (i_Time2 + c_TIMEDIFF)));
	}

}	// end of namespace
