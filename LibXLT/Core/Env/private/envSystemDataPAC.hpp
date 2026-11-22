/**********************************************************
**  envSystemDataPAC.hpp
**
**      envSystemDataPAC forwards the envSystemData calls 
**	to the PAC components
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************/

#ifdef ENV_SYSTEMDATAPAC_HPP
#error envSystemDataPAC.hpp multiply included
#endif
#define ENV_SYSTEMDATAPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

//	Here is the interface that must be supported by the PAC component:
//

//namespace envSystemDataPAC
//{
//	std::wstring GetExecutablePath();
//	void Init();
//	void CleanUp();
//}

#if ENV_WINDOWS
	#include "Core/env/private/envSystemDataPACWin.hpp"
#else
	#error envSystemDataPAC not defined for this platform
#endif

