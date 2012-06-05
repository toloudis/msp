/*****************************************************************************
**	cmmAddOperationTemplate.hpp
**
**	 Represents the add object operation, which can be undone
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_ADDOPERATIONTEMPLATE_HPP
#error cmmAddOperationTemplate.hpp multiply included
#endif
#define CMM_ADDOPERATIONTEMPLATE_HPP

#ifndef UNDO_UNDOOPERATION_HPP
#include "Core/undo/undoUndoOperation.hpp"
#endif

//============================================================================
//============================================================================
template<class xxxScriptData, class xxxActualOperations>
class cmmAddOperationTemplate : public undoUndoOperation
{
public:

	//--------------------------------------------------------------------
	// constructor takes old to be restored if undone
	//--------------------------------------------------------------------
	cmmAddOperationTemplate( int i_ItemIndex, 
							 const xxxScriptData& i_OldData,
							 const char* i_DisplayName)
	:	m_ItemIndex(i_ItemIndex), 
		m_DataBackup( i_OldData ),
		m_DisplayName( i_DisplayName )
	{
	}

	//--------------------------------------------------------------------
	//  Get Name for the operation
	//--------------------------------------------------------------------
	virtual std::string GetDisplayName()
	{
		return m_DisplayName.c_str();
	}

	//--------------------------------------------------------------------
	//  Get memory usage for this operation (in KB). This can be
	// accurate or approximate.
	//--------------------------------------------------------------------
	virtual float GetMemoryUsage()
	{
		return (sizeof(xxxScriptData) / 1000.0f); // convert to KB
	}

	//--------------------------------------------------------------------
	//  Undo is called on an operation when the user chooses
	// Edit->Undo from the menu.
	//--------------------------------------------------------------------
	virtual void Undo()
	{
		xxxActualOperations::DeleteObject( m_ItemIndex );
	}

	//--------------------------------------------------------------------
	// Redo is called on an operation when the user chooses
	// Edit->Redo from the menu and this operation is the next in
	// line to be redone.
	//--------------------------------------------------------------------
	virtual void Redo()
	{
		xxxActualOperations::AddObject( m_DataBackup );
	}

	//--------------------------------------------------------------------
	//  Commit is called on an operation when it is no longer
	// possible for the user to undo this operation.  The
	// destructor will soon be called.
	//--------------------------------------------------------------------
	virtual void Commit()
	{
		// nothing needed
	}

	//--------------------------------------------------------------------
	//  Destroy is called on an operation when it has been undone
	// and it can no longer be redone. This may happen after the
	// history gets long enough or a new operation is made when
	// its current state is "undone". The destructor will soon be
	// called.
	//--------------------------------------------------------------------
	virtual void Destroy()
	{
		// nothing needed
	}

private:
	int m_ItemIndex;
	xxxScriptData m_DataBackup;
	std::string m_DisplayName;
};
