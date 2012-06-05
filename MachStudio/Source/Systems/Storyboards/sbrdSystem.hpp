/*****************************************************************************
**  sbrdSystem.hpp
**
**      System for adding dynamic billboards to the world.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SBRD_SYSTEM_HPP
#error sbrdSystem.hpp multiply included
#endif
#define SBRD_SYSTEM_HPP


//============================================================================
//	forward references
//============================================================================
class fsLocator;
class itString;


//============================================================================
//============================================================================
namespace sbrdSystem
{
	//--------------------------------------------------------------------
	// Init -- initialize system with directory to use to look for
	//		geometry files
	//--------------------------------------------------------------------
	void Init(const fsLocator& i_AppDir, const itString& i_BillboardDataDir);

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp();
};
