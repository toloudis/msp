/*****************************************************************************
**  lodModeList.hpp
**
**      lodModeList maintains a list of several lodModes.  This is
**	useful for modes which wish to transfer to other modes (but don't want
**	to create circular dependencies in the code).
**
**	Extra Large Technology
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/

#ifdef LOD_MODELIST_HPP
#error lodModeList.hpp multiply included
#endif
#define LOD_MODELIST_HPP

class lodMode;

//====================================================================
//====================================================================
namespace lodModeList
{

enum lodModeIndex
{
	e_Handling = 0,
	e_Light,
	e_Placing,
	e_Selecting,
	e_TerrainEditor,
	e_TileEditor,
	e_NumModes
};

//====================================================================
//	Get the given mode.
//====================================================================
lodMode*	GetMode(lodModeIndex i_Index);

//====================================================================
//	Set the given mode.  Probably only the lqApp should use this
//	function.
//====================================================================
void SetMode(lodModeIndex i_Index, lodMode* i_Mode);

//========================================================================
//	Init must be called before you use the env package.  A good place to
//	do this is in your main function, before you do anything else.
//========================================================================
void Init();

//========================================================================
//	CleanUp should be called after you are done with the env package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//	The CleanUp function is written with the throw() exception
//	to suggest that it should not throw any exceptions, since typically
//	the caller is in the process of de-initializing and won't be able
//	to do much with them.
//========================================================================
void CleanUp() throw();

}
