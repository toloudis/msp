/*****************************************************************************
**	tmlnDriverClipboard.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnDriverClipboard.hpp"

#include "Support/tmln/tmlnChannel.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/Common/GUI/cmmAddDriverOperation.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/undo/undoUndoMgr.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace tmlnDriverClipboard 
{
	namespace
	{
		std::vector<tmlnDriverInfo*> l_DriverInfos;
	}

	//------------------------------------------------------------------------
	//	Copy - Add a driver to the clipboard
	//------------------------------------------------------------------------
	void Copy(tmlnDriver *i_pDriver)
	{
		tmlnDriverInfo* pDI = i_pDriver->GetDriverInfo();
		if (pDI != NULL)
		{
			//DBG_LOG("   Created driver info for copy");

			l_DriverInfos.push_back( pDI );
		}
	}

	//------------------------------------------------------------------------
	//	Paste - Create drivers and add to the object
	//------------------------------------------------------------------------
	void Paste(tmlnScriptObject *i_pObject)
	{
		DBG_ASSERT( i_pObject != NULL, "Cannot paste to a NULL object");

		//	find the earliest driver from the copy list
		//
		float earliest_time = -1.0f;
		
		for (int j = 0; j < l_DriverInfos.size(); ++j)
		{
			tmlnDriverInfo* pDI = l_DriverInfos[j];
			if (pDI != NULL)
			{
				//	grab the driver based on the driver ID and see if it is the "earliest"
				//
				if ((earliest_time < 0.0f) || (earliest_time > pDI->m_BeginTime))
				{
					earliest_time = pDI->m_BeginTime;
				}
			}
		}


		//	Create the new drivers
		//

		for (int j = 0; j < l_DriverInfos.size(); ++j)
		{
			tmlnDriverInfo* pDI = l_DriverInfos[j];
			if (pDI != NULL)
			{ 
				// set the driver ID to zero since we are copying drivers and do not want the "old" id in there
				pDI->m_DriverId = 0;
				float endTime, beginTime;
				endTime = pDI->m_EndTime;
				beginTime = pDI->m_BeginTime;
				float duration = endTime - beginTime;
				//TIME - driver info is still in seconds, this should change when that changes
				float timeline_time = tmlnTimeLine::GetValue().AsSeconds();

				if (earliest_time < 0.0f)
				{
						pDI->m_BeginTime =   timeline_time;
						pDI->m_EndTime = pDI->m_BeginTime + duration;
				}
				else
				{
					pDI->m_BeginTime =  timeline_time + (pDI->m_BeginTime - earliest_time) ;
					pDI->m_EndTime = pDI->m_BeginTime + duration;			
				}

				
				tmlnDriver* pDriver = tmlnCreator::CreateDriverFromInfo(i_pObject, pDI);
								
				if (pDriver != NULL)
				{
					//DBG_LOG2("Paste driver %d %s", j, pDriver->GetName().c_str());

					//	set the driver information
					//
					//	set the driver times to they match time layout of the copied drivers.
					//
					
					
					i_pObject->NotifyDriverChanged();

					// Should this undo operation go here since it uses undo and cmm?
					//
					// Create an undo operation for this driver
					undoUndoMgr::AddOperation( new cmmAddDriverOperation(i_pObject->CreateReferenceToSelf(), pDriver) );
				}
			}
		}
	}

	//------------------------------------------------------------------------
	//	Clear - Clear out the entire clipboard
	//------------------------------------------------------------------------
	void Clear()
	{
		envSTLHelpers::DeleteContainer( l_DriverInfos );
	}
	void Clear(tmlnDriver *i_pDriver)
	{
		// find it by driverID
		for (int i=0; i < l_DriverInfos.size(); ++i)
		{
			if (i_pDriver->GetDriverId() == l_DriverInfos[i]->m_DriverId)
			{
				// delete it
				envSTLHelpers::DeleteOneValue( l_DriverInfos, l_DriverInfos[i] );
				break;
			}
		}
	}

	//------------------------------------------------------------------------
	//	Count - Get the number of items in the clipboard
	//------------------------------------------------------------------------
	int Count()
	{
		return l_DriverInfos.size();
	}

	//------------------------------------------------------------------------
	//	GetDriver - Get a specific driver
	//------------------------------------------------------------------------
	tmlnDriverInfo* GetDriverInfo(const int i_DriverIndex)
	{
		DBG_ASSERT( (i_DriverIndex < 0 || i_DriverIndex >= l_DriverInfos.size()), "Invalid index " << i_DriverIndex << " < " << l_DriverInfos.size() );

		return l_DriverInfos[i_DriverIndex];
	}
	
}