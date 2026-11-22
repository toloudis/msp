/*****************************************************************************
**  cmraSystem.hpp
**
**      System Cmra
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CMRA_SYSTEM_HPP
#error cmraSystem.hpp multiply included
#endif
#include <string>
#define CMRA_SYSTEM_HPP

class g2dSystem;
class fsLocator;

namespace cmraSystem
{
		//--------------------------------------------------------------------
		// Init 
		//--------------------------------------------------------------------
		void Init(const fsLocator& i_AppDir, 
				  const std::string& i_CameraDataDir);

		//--------------------------------------------------------------------
		// CleanUp -- cleanup system
		//--------------------------------------------------------------------
		void CleanUp();
};
