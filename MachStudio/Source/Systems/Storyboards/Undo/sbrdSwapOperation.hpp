/*****************************************************************************
**	sbrdSwapOperation.hpp
**
**	 Represents the swap object operation, which can be undone
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SBRD_SWAPOPERATION_HPP
#error sbrdSwapOperation.hpp multiply included
#endif
#define SBRD_SWAPOPERATION_HPP

#ifndef UNDO_UNDOOPERATION_HPP
#include "Core/undo/undoUndoOperation.hpp"
#endif

//============================================================================
//============================================================================
class sbrdActualOperations;


//============================================================================
//============================================================================
class sbrdSwapOperation : public undoUndoOperation
{
public:
	//--------------------------------------------------------------------
	// constructor takes old to be restored if undone
	//--------------------------------------------------------------------
	sbrdSwapOperation(  int i_ItemIndex1, 
						int i_ItemIndex2, 
						const sbrdListData& i_OldData,
						const char* i_DisplayName)
	:	m_ItemIndex1(i_ItemIndex1), 
		m_ItemIndex2(i_ItemIndex2), 
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
		return (sizeof(sbrdScriptData) / 1000.0f); // convert to KB
	}

	//--------------------------------------------------------------------
	//  Undo is called on an operation when the user chooses
	// Edit->Undo from the menu.
	//--------------------------------------------------------------------
	virtual void Undo()
	{
		sbrdActualOperations::SwapStoryboards( m_ItemIndex2, m_ItemIndex1 );
	}

	//--------------------------------------------------------------------
	// Redo is called on an operation when the user chooses
	// Edit->Redo from the menu and this operation is the next in
	// line to be redone.
	//--------------------------------------------------------------------
	virtual void Redo()
	{
		sbrdActualOperations::SwapStoryboards( m_ItemIndex1, m_ItemIndex2 );
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
	int m_ItemIndex1;
	int m_ItemIndex2;
	sbrdListData m_DataBackup;
	std::string m_DisplayName;
};
