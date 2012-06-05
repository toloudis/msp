/********************************************************************
**  envSystemDataPACXbox.hpp
**
**      envSystemDataPACXbox is the declaration for the Xbox
**	version of the system data PAC.  
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*******************************************************************/

#ifdef ENV_SYSTEMDATAPACXBOX_HPP
#error envSystemDataPACXbox.hpp multiply included
#endif
#define ENV_SYSTEMDATAPACXBOX_HPP

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
