/*****************************************************************************
**	vtxVertexAnimBudget.hpp
**
**		vtxVertexAnimBudget - manages the amount of vertex animation
**	memory required versus the budget allowed. Decides when to swap
**	data in and out of memory.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_VERTEXANIMBUDGET_HPP
#error vtxVertexAnimBudget.hpp multiply included
#endif
#define VTX_VERTEXANIMBUDGET_HPP
 
#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
#endif 


//============================================================================
//============================================================================
class vtxVertexAnimBudgetItem
{
public:
	typedef envType::UInt64 BudgetSize;
	virtual BudgetSize GetSize() const = 0;
	//virtual BudgetSize GetMinimumSize() const = 0;

	virtual void FullyLoadData() = 0;
	virtual void FreeDataAndStartSwapping() = 0;
};


//============================================================================
//============================================================================
namespace vtxVertexAnimBudget
{
	//--------------------------------------------------------------------
	// Set new limit for memory allowed to be allocated to
	//	vertex animation. Within this function, ReallocateBudget()
	//	will be called.
	// The "inMBs" variation sets the limit in terms of megabytes.
	//--------------------------------------------------------------------
	void SetBudgetLimitInMBs(int i_BudgetLimitMBs);
	void SetBudgetLimit(vtxVertexAnimBudgetItem::BudgetSize i_BudgetLimit);

	//--------------------------------------------------------------------
	// Add new item to budget management. Returns true if the
	// full item can be loaded.
	//--------------------------------------------------------------------
	bool AddItem(vtxVertexAnimBudgetItem *i_pBufferItem);

	//--------------------------------------------------------------------
	// Remove item from management
	//--------------------------------------------------------------------
	void RemoveItem(vtxVertexAnimBudgetItem *i_pBufferItem);

	//--------------------------------------------------------------------
	// Reallocate the memory budget between the vertex sets that
	//	have been added to the system. It would be too expensive 
	//  to reallocate the memory on every add and remove item,
	//	so it is necessary for this function to be called after
	//	files are loaded or scenes are freed.
	//--------------------------------------------------------------------
	void ReallocateBudget();

	//--------------------------------------------------------------------
	// If the application knows that multiple files will be loaded
	// then it would be better to suspend the budgeting while
	// the files are loaded and then assign the budget after the
	// full set has been loaded.
	// ResumeBudgeting will call ReallocateBudget().
	//--------------------------------------------------------------------
	void SuspendBudgeting();
	void ResumeBudgeting();
}
