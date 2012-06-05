/*****************************************************************************
**	prtclActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRTCL_ACTUALOPERATIONS_HPP
#error prtclActualOperations.hpp multiply included
#endif
#define PRTCL_ACTUALOPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class nameString;
class prtclData;
class prtclScriptData;
class maRotation;


//============================================================================
//============================================================================
class prtclActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new Particle to world
	//--------------------------------------------------------------------
	static int  AddObject(const prtclScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete Particle with specific data or a given index
	//--------------------------------------------------------------------
	static void  DeleteObject(const prtclScriptData& i_Data);
	static void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	// Update individual base properties
	//--------------------------------------------------------------------
	static void SetBaseData(int i_Index, const prtclData& i_Data);

	//--------------------------------------------------------------------
	// Update individual Particle properties
	//--------------------------------------------------------------------
	static void SetData(int i_Index, const prtclScriptData& i_Data);

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	static void ChangeDriverData(int i_Index, const prtclScriptData& i_Data);

	//--------------------------------------------------------------------
	// Set Name
	//--------------------------------------------------------------------
	static void SetName(int i_Index, const nameString& i_Name);

	//--------------------------------------------------------------------
	// Set Position
	//--------------------------------------------------------------------
	static void SetPosition(int i_Index, const maPoint3d& i_Position);

	//--------------------------------------------------------------------
	// Set Orientation
	//--------------------------------------------------------------------
	static void SetOrientation(int i_Index, const maRotation& i_Orientation);

};	// end of static class

