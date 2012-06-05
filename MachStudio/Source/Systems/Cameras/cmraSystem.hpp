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
#define CMRA_SYSTEM_HPP

class g2dSystem;
class fsLocator;
class itString;

namespace cmraSystem
{
		//--------------------------------------------------------------------
		// Init 
		//--------------------------------------------------------------------
		void Init(const fsLocator& i_AppDir, 
				  const itString& i_CameraDataDir);

		//--------------------------------------------------------------------
		// CleanUp -- cleanup system
		//--------------------------------------------------------------------
		void CleanUp();
};
