/*****************************************************************************
**	mtrlSetOperation.hpp
**
**	 Undoable operation for changing material data as a whole. This is
**	used to undo a "paste material" operation, or an import from 
**	a material library.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MTRL_SETOPERATION_HPP
#error mtrlSetOperation.hpp multiply included
#endif
#define MTRL_SETOPERATION_HPP

#ifndef UNDO_UNDOOPERATION_HPP
#include "Core/undo/undoUndoOperation.hpp"
#endif
#ifndef MDL_MATERIALINFO_HPP
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#endif 
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 


class mtrlScriptObject;
class relObjectReference;

//============================================================================
//============================================================================
class mtrlSetOperation : public undoUndoOperation
{
public:

	//--------------------------------------------------------------------
	// constructor takes old to be restored if undone
	//--------------------------------------------------------------------
	mtrlSetOperation( const char* i_DisplayName,
					  mtrlScriptObject *i_pObject,
					  int i_MaterialIndex,
					  const mdlMaterialInfo& i_OldData,
					  bool i_bUpdateProperties );

	//--------------------------------------------------------------------
	//  Get Name for the operation
	//--------------------------------------------------------------------
	virtual std::string GetDisplayName();

	//--------------------------------------------------------------------
	//  Get memory usage for this operation (in KB). This can be
	// accurate or approximate.
	//--------------------------------------------------------------------
	virtual float GetMemoryUsage();

	//--------------------------------------------------------------------
	//  Undo is called on an operation when the user chooses
	// Edit->Undo from the menu.
	//--------------------------------------------------------------------
	virtual void Undo();

	//--------------------------------------------------------------------
	// Redo is called on an operation when the user chooses
	// Edit->Redo from the menu and this operation is the next in
	// line to be redone.
	//--------------------------------------------------------------------
	virtual void Redo();

	//--------------------------------------------------------------------
	//  Commit is called on an operation when it is no longer
	// possible for the user to undo this operation.  The
	// destructor will soon be called.
	//--------------------------------------------------------------------
	virtual void Commit();

	//--------------------------------------------------------------------
	//  Destroy is called on an operation when it has been undone
	// and it can no longer be redone. This may happen after the
	// history gets long enough or a new operation is made when
	// its current state is "undone". The destructor will soon be
	// called.
	//--------------------------------------------------------------------
	virtual void Destroy();

private:
	std::string m_DisplayName;
	shared_ptr<relObjectReference> m_ObjectRef;
	int m_MaterialIndex;
	mdlMaterialInfo m_DataBackup;
	bool m_bUpdateProperties;
};
