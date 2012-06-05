/*****************************************************************************
**	prtclOperations.hpp
**
**	Utility for doing operations that are undoable in SystemParticles
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRTCL_OPERATIONS_HPP
#error prtclOperations.hpp multiply included
#endif
#define PRTCL_OPERATIONS_HPP

#ifndef PRTCL_SCRIPTDATA_HPP
#include "Systems/Particles/Data/prtclScriptData.hpp"
#endif


//============================================================================
//	forward references
//============================================================================


//============================================================================
//============================================================================
namespace prtclOperations
{
	//------------------------------------------------------------------------
	//  Add new item to prtcl
	//------------------------------------------------------------------------
	void  AddObject(const prtclScriptData &i_Item);

	//------------------------------------------------------------------------
	//  Change data for a specific item
	//------------------------------------------------------------------------
	//void  EditParticle(int i_Index, const prtclScriptData &i_Item);

	//------------------------------------------------------------------------
	//  Removes an item from the prtcl
	//------------------------------------------------------------------------
	void  RemoveParticle(int i_Index);

	//--------------------------------------------------------------------
	//  Make a clone of the Object with given index
	//--------------------------------------------------------------------
	void  DuplicateObject(int i_Index);

	//--------------------------------------------------------------------
	//  Select prtcl with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Keep track of index of prtcl being edited so that calls to
	//  ChangeParticleData affect the right one
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index);

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const prtclScriptData& i_Data);

	//--------------------------------------------------------------------
	// Change the Name
	//--------------------------------------------------------------------
	void ChangeName(const std::string& i_Name);

	//--------------------------------------------------------------------
	// ResetGenerators - clear out particles from generators to
	//	to restart generation
	//--------------------------------------------------------------------
	void ResetGenerators();
}

