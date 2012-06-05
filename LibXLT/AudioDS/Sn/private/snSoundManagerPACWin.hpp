/****************************************************************************\
**  snSoundManagerPACWin.hpp
**
**      snSoundManagerPACWin.hpp implements the windows portion of the
**	snSoundManagerUtilPAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDMANAGERPACWIN_HPP
#error snSoundManagerPACWin.hpp multiply included
#endif
#define SN_SOUNDMANAGERPACWIN_HPP

namespace snSoundManagerPAC
{
	//========================================================================
	//	Think()
	//
	//	Update for Windows.
	//========================================================================
	void Think( float i_SimulationTime );

	//========================================================================
	//	Don't call Init() and CleanUp() yourself; they are called 
	//	by the package Init and Cleanup.
	//========================================================================
	void	Init();
	void	CleanUp() throw();
};

