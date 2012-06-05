/*****************************************************************************
**  setsSystem.hpp
**
**      System for adding static object with no transformation into level
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SETS_SYSTEM_HPP
#error setsSystem.hpp multiply included
#endif
#define SETS_SYSTEM_HPP


//============================================================================
//============================================================================
class fsLocator;
class itString;


//============================================================================
//============================================================================
namespace setsSystem
{
	//--------------------------------------------------------------------
	// Init -- initialize system with directory to use to look for
	//		geometry files
	//--------------------------------------------------------------------
	void Init(const fsLocator& i_AppDir, const itString& i_Dir);

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp();
};
