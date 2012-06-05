/*****************************************************************************
**  snSoundUtilPACDSound.hpp
**
**      snSoundUtilPACDSound contains the windows implementation of the
**	snSoundUtilPAC.
**
**	StudioGPU
**	Copyright(C) 2003-2 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDUTILPACDSOUND_HPP
#error snSoundUtilPACDSound.hpp multiply included
#endif
#define SN_SOUNDUTILPACDSOUND_HPP



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace snSoundUtilPAC
{
//----------------------------------------------------------------------------
//	Conversions
//----------------------------------------------------------------------------

	//========================================================================
	//	ConvertVolume()
	//
	//	Given a percent from 0 to 100, return the correct volume value.  The
	//	percentage is LINEAR, so the coverted volume will not be a percentage
	//	on the curve, but a percentage of the actual volume.
	//========================================================================
	int	ConvertVolume( float i_SoundPercent );

	//========================================================================
	//	ConvertPan()
	//
	//	Given a percent from -1.0 to 1.0, return the correct Pan value.  The
	//	percentage is LINEAR, so the coverted Pan will not be a percentage
	//	on the curve, but a percentage of the actual Pan.
	//========================================================================
	int	ConvertPan( float i_SoundPercent );

	//========================================================================
	//	ConvertFrequency()
	//
	//	Given a multiplier from -10.0 to 10.0, return the correct Frequency 
	//	value.  The ORIGINAL sound frequency is passed in and a conversion
	//	is made.  So if the multiplier if 1.0 the sound is doubled, 2.0 is
	//	tripled, -1.0 is halved, and 0.0 is back to the original.
	//========================================================================
	int	ConvertFrequency( float i_SoundPercent, float i_SoundFrequency );


//----------------------------------------------------------------------------
//	Base functions
//----------------------------------------------------------------------------

	//========================================================================
	//	Don't call Init() and CleanUp() yourself; they are called 
	//	by the package Init and Cleanup.
	//========================================================================
	void Init();
	void CleanUp() throw();
}
