/*****************************************************************************
**	chnlSnapUtil.cpp
**
**	Utility for snapping times
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlSnapUtil.hpp"

#include "Features/Prefs/PrefsMgr.hpp"
#include "Core/Ma/maTime.hpp"

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
	bool TimesMatch( const maTime& i_Time1, const maTime& i_Time2 )
	{
		PrefsData& prefs_data = PrefsMgr::Data();

		maTime c_TIMEDIFF = maTime::FromFrame(8, 1000);
		if (prefs_data.m_ChannelEditor_SnapActive.GetValue())
			c_TIMEDIFF = maTime::FromSeconds(prefs_data.m_ChannelEditor_SnapAmount.GetValue());

		return (   (i_Time1 >= (i_Time2 - c_TIMEDIFF))
				&& (i_Time1 <= (i_Time2 + c_TIMEDIFF)));
	}

}	// end of namespace
