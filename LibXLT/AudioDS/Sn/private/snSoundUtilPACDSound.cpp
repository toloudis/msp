/*****************************************************************************
**  snSoundUtilPACWin.cpp
**
**      snSoundUtilPACWin contains the windows implementation of the
**	snSoundUtilPAC.
**
**	StudioGPU
**	Copyright(C) 2003-2 - All Rights Reserved
\****************************************************************************/

#include "AudioDS/sn/private/snSoundUtilPACDSound.hpp"

#include "Core/dbg/dbgMsg.hpp"

#include <dsound.h>
#include <iomanip>
#include <math.h>			// for pow()



//============================================================================
//============================================================================
namespace snSoundUtilPAC
{

namespace
{
	//	reverse LUT for percentage to 100ths of dBs conversion
	//
	int		VolumeLUT[ 101 ];

	const float	VOLUME_RANGE	= DSBVOLUME_MAX - DSBVOLUME_MIN;
}
 

//========================================================================
//	ConvertVolume()
//
//	Given a percent from 0 to 100, return the correct volume value.  The
//	percentage is LINEAR, so the coverted volume will not be a percentage
//	on the curve, but a percentage of the actual volume.
//========================================================================
int	ConvertVolume( float i_SoundPercent )
{
	DBG_ASSERT( ((i_SoundPercent >= 0.0f) && (i_SoundPercent <= 1.0f)), "Percent out of range - " << std::setw(6) << std::setprecision(2) << i_SoundPercent );

	return VolumeLUT[ (unsigned int)(i_SoundPercent * 100.0f) ];
}


//========================================================================
//	ConvertPan()
//
//	Given a percent from 0 to 1.0, return the correct Pan value.  The
//	percentage is LINEAR, so the coverted Pan will not be a percentage
//	on the curve, but a percentage of the actual Pan.
//========================================================================
int	ConvertPan( float i_SoundPercent )
{
	DBG_ASSERT( ((i_SoundPercent >= -1.0f) && (i_SoundPercent <= 1.0f)), "Percent out of range - " << std::setw(6) << std::setprecision(2) << i_SoundPercent );

	//	DSBPAN_LEFT  = -10000
	//	DSBPAN_RIGHT =  10000
	return static_cast<int>(i_SoundPercent * DSBPAN_RIGHT);
}


//========================================================================
//	ConvertFrequency()
//
//	The sound system provides a way to manipulate frequencies based on a
//	percentage rather than dealing with the frequency values themselves.
//	Given a percent, where 1.0 is 100% (normal playback), 0.5 is 50% 
//	(half speed), and 2.0 is 200% (double speed) we can calculate the 
//	frequency.  the valid percent is 0.0 to 10.0.
//
//	DSBFREQUENCY_MAX = 100,000
//	DSBFREQUENCY_MIN = 100
//	DSBFREQUENCY_ORIGINAL = 0 (resets to original frequency)
//========================================================================
int	ConvertFrequency( float i_SoundPercent, float i_SoundFrequency )
{
	DBG_ASSERT( ((i_SoundPercent >= 0.0f) && (i_SoundPercent <= 10.0f)), "Percent out of range - " << std::setw(6) << std::setprecision(2) << i_SoundPercent );
	DBG_ASSERT( ((i_SoundFrequency >= 100.0f) && (i_SoundFrequency <= 100000.0f)), "Frequency out of range - " << std::setw(6) << std::setprecision(2) << i_SoundFrequency );

	if ( i_SoundPercent == 1.0f )
	{
		return DSBFREQUENCY_ORIGINAL;
	}
	else
	{
		float newfrequency;
		newfrequency = i_SoundFrequency + ((i_SoundPercent-1.0f) * i_SoundFrequency);
		if ( newfrequency > DSBFREQUENCY_MAX )
		{
			newfrequency	= DSBFREQUENCY_MAX;
		}
		if ( newfrequency < DSBFREQUENCY_MIN )
		{
			newfrequency	= DSBFREQUENCY_MIN;
		}

		return static_cast<int>(newfrequency);
	}
}


//========================================================================
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//========================================================================
void Init()
{
	// Also decided to scale up all volumes such that the max volume is the true max, and also set the
	// minimum to the absolute directx minimum.
	//
	VolumeLUT[ 100 ] = DSBVOLUME_MIN + (int)( VOLUME_RANGE * ( 1.0f - powf( 1.3f, -10.0f) ) );
	for ( unsigned int i = 0; i <= 100; i++ )
	{
		VolumeLUT[ i ] = DSBVOLUME_MIN + (int)( VOLUME_RANGE * ( 1.0f - powf( 1.3f, -( i / 10.0f ) ) ) );
		VolumeLUT[ i ] = VolumeLUT[ i ] - VolumeLUT[ 100 ];
	}
	VolumeLUT[ 0 ] = DSBVOLUME_MIN;
}

void CleanUp() throw()
{
}

}
