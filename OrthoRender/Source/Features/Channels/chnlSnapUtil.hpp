/*****************************************************************************
**	chnlSnapUtil.hpp
**
**	Utility for snapping times
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_SNAPUTIL_HPP
#error chnlSnapUtil.hpp multiply included
#endif
#define CHNL_SNAPUTIL_HPP


//============================================================================
//============================================================================
namespace chnlSnapUtil
{
	//--------------------------------------------------------------------
	// TimesMatch - returns true if the two times are within
	//	the snap threshold.
	//--------------------------------------------------------------------
	bool TimesMatch( float i_Time1, float i_Time2 );

}	// end of namespace
