/********************************************************************
**  envSystemDataPACWin.hpp
**
**      envSystemDataPACPS2 is the declaration for the ps2
**	version of the system data PAC.  
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*******************************************************************/

#ifdef ENV_SYSTEMDATAPACPS2_HPP
#error envSystemDataPACPS2.hpp multiply included
#endif
#define ENV_SYSTEMDATAPACPS2_HPP

namespace envSystemDataPAC
{
	//========================================================================
	//	Don't call Init() yourself; it is called by the package Init().
	//========================================================================
	void Init();

	//========================================================================
	//	Don't call CleanUp() yourself; it is called by the package CleanUp().
	//========================================================================
	void CleanUp() throw();
}
