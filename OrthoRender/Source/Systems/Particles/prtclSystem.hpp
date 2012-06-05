/*****************************************************************************
**  prtclSystem.hpp
**
**      System for adding dynamic particles to the world.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PRTCL_SYSTEM_HPP
#error prtclSystem.hpp multiply included
#endif
#define PRTCL_SYSTEM_HPP


//============================================================================
//============================================================================
class fsLocator;
class itString;


//============================================================================
//============================================================================
namespace prtclSystem
{
	//--------------------------------------------------------------------
	// Init -- initialize system with directory to use to look for
	//		geometry files
	//--------------------------------------------------------------------
	void Init(const fsLocator& i_AppDir, const itString& i_ParticleDataDir);

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp();
};
