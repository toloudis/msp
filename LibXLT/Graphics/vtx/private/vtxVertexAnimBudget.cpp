/*****************************************************************************
**	vtxVertexAnimBudget.cpp
**
**		vtxVertexAnimBudget - animation information for vertex baked
**	animation.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxVertexAnimBudget.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/Env/envSTLHelpers.hpp"

#include <queue>


//============================================================================
//============================================================================
namespace
{
	const bool bDebugBudgeting = false;

	bool l_bBudgetingSuspended = false;
	const vtxVertexAnimBudgetItem::BudgetSize c_Megabyte = 1024*1024;
	vtxVertexAnimBudgetItem::BudgetSize l_VertexAnimBudget = 10 * c_Megabyte; // Reduced to smaller 10MB fixed limit
	vtxVertexAnimBudgetItem::BudgetSize l_BudgetUsed = 0;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool decreasing_order(vtxVertexAnimBudgetItem* i_pItem1, vtxVertexAnimBudgetItem* i_pItem2)
	{
		return (i_pItem1->GetSize() < i_pItem2->GetSize());
	}
	bool increasing_order(vtxVertexAnimBudgetItem* i_pItem1, vtxVertexAnimBudgetItem* i_pItem2)
	{
		return (i_pItem1->GetSize() > i_pItem2->GetSize());
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	//typedef std::priority_queue<vtxVertexAnimBudgetItem*,
	//							std::vector<vtxVertexAnimBudgetItem*>,
	//							decreasing_order> IncreasingQueueType;
	//typedef std::priority_queue<vtxVertexAnimBudgetItem*,
	//							std::vector<vtxVertexAnimBudgetItem*>,
	//							increasing_order> DecreasingQueueType;
	//typedef std::priority_queue<vtxVertexAnimBudgetItem*> MyQueueType;

	//IncreasingQueueType l_LoadedItems;
	//DecreasingQueueType l_SwappingItems;

	// Priority queue didn't allow taking items out of the middle when needed,
	// So I will use a vector and manage the heap myself.
	std::vector<vtxVertexAnimBudgetItem*> l_LoadedItems;
	std::vector<vtxVertexAnimBudgetItem*> l_SwappingItems;

} // end of namespace


//--------------------------------------------------------------------
// Set new limit for memory allowed to be allocated to
//	vertex animation. Within this function, ReallocateBudget()
//	will be called.
//--------------------------------------------------------------------
void vtxVertexAnimBudget::SetBudgetLimitInMBs(int i_BudgetLimitMBs)
{
	SetBudgetLimit(i_BudgetLimitMBs * c_Megabyte);
}
void vtxVertexAnimBudget::SetBudgetLimit(vtxVertexAnimBudgetItem::BudgetSize i_BudgetLimit)
{
	l_VertexAnimBudget = i_BudgetLimit;
	ReallocateBudget();
}

//--------------------------------------------------------------------
// Add new item to budget management. Returns true if the
// full item can be loaded.
//--------------------------------------------------------------------
bool vtxVertexAnimBudget::AddItem(vtxVertexAnimBudgetItem *i_pBufferItem)
{
	vtxVertexAnimBudgetItem::BudgetSize item_size = i_pBufferItem->GetSize();
	//vtxVertexAnimBudgetItem::BudgetSize item_size = i_Size;
	if (l_BudgetUsed + item_size > l_VertexAnimBudget)
	{
		// Too much memory is being used. Add this item to the queue
		// that is requesting more memory. When the next call to
		// ReallocateBudget comes, we can decide who is more worthy then.
		l_SwappingItems.push_back(i_pBufferItem);
		return false;
	}
	else
	{
		// We haven't reached the budget yet, so go ahead and load the 
		// full data for this item. At the next call to ReallocateBudget()
		// we might change our mind.
		l_BudgetUsed += item_size;
		l_LoadedItems.push_back(i_pBufferItem);
		return true;
	}

	// Adding the items as above (instead of push_heap) has broken the heap sorting. 
	// The heap will be recreated in ReallocateBudget().
}

//--------------------------------------------------------------------
// Remove item from management
//--------------------------------------------------------------------
void vtxVertexAnimBudget::RemoveItem(vtxVertexAnimBudgetItem *i_pBufferItem)
{
	// Remove the item from the queues
	if (envSTLHelpers::RemoveOneValue(l_LoadedItems, i_pBufferItem))
		l_BudgetUsed -= i_pBufferItem->GetSize();
	else
		envSTLHelpers::RemoveOneValue(l_SwappingItems, i_pBufferItem);

	// Doing the removal above has broken the heap sorting.  
	// The heap will be recreated in ReallocateBudget().

	// Note that I am not calling ReallocateBudget here because we
	// might be in a situation where the whole scene is begin freed
	// and I don't want to start loading animations into memory
	// just before they get freed also.
}

//--------------------------------------------------------------------
// Reallocate the memory budget between the vertex sets that
//	have been added to the system. It would be too expensive 
//  to reallocate the memory on every add and remove item,
//	so it is necessary for this function to be called after
//	files are loaded or scenes are freed.
//--------------------------------------------------------------------
void vtxVertexAnimBudget::ReallocateBudget()
{
	// If the budgeting is suspended, then return for now and wait
	// for the next call.
	if (l_bBudgetingSuspended) return;

	// The goal allocation here is to make the smallest items fully loaded
	// up until the budget is filled. Each time a vertex set is fully loaded,
	// that is one fewer file locations to track and makes it easier on the
	// load. I am trying to create something with fewer files open and larger 
	// reads at each time versus many small reads from more file locations.

	// Sort the lists into heaps so that we can process them in order
	std::make_heap(l_LoadedItems.begin(), l_LoadedItems.end(), decreasing_order);
	std::make_heap(l_SwappingItems.begin(), l_SwappingItems.end(), increasing_order);

	// When the allocation budget has changed, it could be that we are overbudget and
	// have to free items from the l_LoadedItems list first.
	while (l_BudgetUsed > l_VertexAnimBudget)
	{
		// Free items from the loaded list until we get below the limit.
		// Freeing the largest items first.
		vtxVertexAnimBudgetItem *pLoadedItem = l_LoadedItems[0];
		vtxVertexAnimBudgetItem::BudgetSize loaded_size = pLoadedItem->GetSize();
			
		// Free the memory in the loaded_item and move it to the other heap
		if (bDebugBudgeting)
			DBG_LOG("Overbudget, freeing loaded set: " << loaded_size);
		l_BudgetUsed -= loaded_size;
		pLoadedItem->FreeDataAndStartSwapping();

		// Pop item off of loaded heap
		std::pop_heap(l_LoadedItems.begin(), l_LoadedItems.end(), decreasing_order);
		l_LoadedItems.pop_back();

		// Push item onto swapping heap
		l_SwappingItems.push_back(pLoadedItem);
		std::push_heap(l_SwappingItems.begin(), l_SwappingItems.end(), increasing_order);
	}

	// Consider the smallest item in the l_SwappingItems list and see if it
	// could fit in memory, or if it is smaller than the largest items in 
	// the l_LoadedList. If so, tell it to load itself.
	while (!l_SwappingItems.empty())
	{
		// Index 0 is the top() item after make_heap
		vtxVertexAnimBudgetItem *pItem = l_SwappingItems[0];
		vtxVertexAnimBudgetItem::BudgetSize item_size = pItem->GetSize();
		if (bDebugBudgeting)
			DBG_LOG("Smallest Swapping set is size: " << item_size);

		bool bLoadItem = false; // This will be set to true if we should load the item.
		if (l_BudgetUsed + item_size <= l_VertexAnimBudget)
		{	
			// Load the item now, without having to remove any other item from memory
			bLoadItem = true;
		}
		else if (!l_LoadedItems.empty())
		{
			// Get the largest loaded item and decide if it is better to
			// free this item in order to get the smaller item into memory
			vtxVertexAnimBudgetItem *pLoadedItem = l_LoadedItems[0];
			vtxVertexAnimBudgetItem::BudgetSize loaded_size = pLoadedItem->GetSize();
			if (bDebugBudgeting)
				DBG_LOG("Largest Loaded set is size: " << loaded_size);

			if (item_size < loaded_size)
			{
				bLoadItem = true;
				
				// Free the memory in the loaded_item and move it to the other heap
				if (bDebugBudgeting)
					DBG_WARNING("Freeing loaded set: " << loaded_size);
				l_BudgetUsed -= loaded_size;
				pLoadedItem->FreeDataAndStartSwapping();

				// Pop item off of loaded heap
				std::pop_heap(l_LoadedItems.begin(), l_LoadedItems.end(), decreasing_order);
				l_LoadedItems.pop_back();

				// Push item onto swapping heap
				l_SwappingItems.push_back(pLoadedItem);
				std::push_heap(l_SwappingItems.begin(), l_SwappingItems.end(), increasing_order);
			}
		}

		if (bLoadItem)
		{
			if (bDebugBudgeting)
				DBG_LOG("Loading previously swapping set: " << item_size);
			l_BudgetUsed += item_size;
			pItem->FullyLoadData();

			// Pop item off of swapping heap
			std::pop_heap(l_SwappingItems.begin(), l_SwappingItems.end(), increasing_order);
			l_SwappingItems.pop_back();

			// Push item onto loaded heap
			l_LoadedItems.push_back(pItem);
			std::push_heap(l_LoadedItems.begin(), l_LoadedItems.end(), decreasing_order);
		}
		else break; // If we didn't change memory, quit the loop
	}

	if (bDebugBudgeting)
	{
		if ((l_BudgetUsed > 0) || (!l_SwappingItems.empty()))
		{
			// Debug results of re-allocation
			DBG_WARNING("Budget reallocated, using memory: " << l_BudgetUsed /c_Megabyte << " MBs");
			DBG_WARNING(" Fully loaded sets:");
			for (int i=0; i<l_LoadedItems.size(); ++i)
				DBG_WARNING("   Size: " << l_LoadedItems[i]->GetSize()/1024 << " KBs");
			DBG_WARNING(" Swapping sets:");
			for (int i=0; i<l_SwappingItems.size(); ++i)
				DBG_WARNING("   Size: " << l_SwappingItems[i]->GetSize()/1024 << " KBs");
		}
	}
}

//--------------------------------------------------------------------
// If the application knows that multiple files will be loaded
// then it would be better to suspend the budgeting while
// the files are loaded and then assign the budget after the
// full set has been loaded.
//--------------------------------------------------------------------
void vtxVertexAnimBudget::SuspendBudgeting()
{
	l_bBudgetingSuspended = true;
}
void vtxVertexAnimBudget::ResumeBudgeting()
{
	l_bBudgetingSuspended = false;
	ReallocateBudget();
}

