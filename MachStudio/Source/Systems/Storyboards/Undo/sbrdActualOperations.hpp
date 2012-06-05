/*****************************************************************************
**	sbrdActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SBRD_ACTUALOPERATIONS_HPP
#error sbrdActualOperations.hpp multiply included
#endif
#define SBRD_ACTUALOPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

//============================================================================
//============================================================================
class sbrdListData;
class sbrdScriptData;
class nameString;
class maRotation;
class itString;


//============================================================================
//============================================================================
class sbrdActualOperations
{
public:
	//--------------------------------------------------------------------
	//	Insert
	//--------------------------------------------------------------------
	static void InsertStoryboard(int i_Index, const itString& i_Filename);

	//--------------------------------------------------------------------
	//  Add new storyboard to world
	//--------------------------------------------------------------------
	static int  AddStoryboard(const itString& i_Filename, bool i_bAddTo3DWorld = true);

	//--------------------------------------------------------------------
	//  Delete Billboard with specific data or a given index
	//--------------------------------------------------------------------
	//static void  DeleteStoryboard(const sbrdListData& i_Data);
	static void  DeleteStoryboard(int i_Index);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	static void SwapStoryboards(int i_Index1, int i_Index2);

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	//Bstatic void ChangeDriverData(int i_Index, const sbrdScriptData& i_Data);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static int AddObject(const sbrdScriptData& i_Data);
	static void DeleteObject(int i_Index) ;

};	// end of static class
