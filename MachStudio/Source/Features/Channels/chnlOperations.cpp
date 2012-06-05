/*****************************************************************************
**	chnlOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlOperations.hpp"

#include "Features/Channels/chnlDeleteDriverOperation.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/chnlTimeTickMgr.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnDriverClipboard.hpp"
#include "Support/tmln/tmlnDriverDialogUtil.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"


//============================================================================
//============================================================================
namespace chnlOperations
{
	namespace
	{
		class DriverMonitor : public prtyPropertyCallback, public envAuditor
		{
		public:
			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			DriverMonitor(tmlnDriver &i_Driver)
				: m_Driver(i_Driver) 
			{
				m_Driver.PropertyBeginTime().AddCallback(create_callback());
				m_Driver.PropertyEndTime().AddCallback(create_callback());
				m_Driver.PropertyBlendType().AddCallback(create_callback());
				m_Driver.PropertyBlendTime().AddCallback(create_callback());
				m_Driver.PropertyRestoreOriginal().AddCallback(create_callback());
				m_Driver.AddAuditor(this);
			}

			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			~DriverMonitor()
			{
				// If we still have our callbacks, then the tmlnDriver is still
				// around, but the channel editor view is being updated. So,
				// remove our callbacks from the driver, taking back ownership,
				// and then delete them.
				if (!m_Callbacks.empty())
				{
					m_Driver.RemoveAuditor(this);
					m_Driver.PropertyBeginTime().RemoveCallback(m_Callbacks[0]);
					m_Driver.PropertyEndTime().RemoveCallback(m_Callbacks[1]);
					m_Driver.PropertyBlendType().RemoveCallback(m_Callbacks[2]);
					m_Driver.PropertyBlendTime().RemoveCallback(m_Callbacks[3]);
					m_Driver.PropertyRestoreOriginal().RemoveCallback(m_Callbacks[4]);
				}
			}

		private:
			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			shared_ptr<prtyPropertyCallback> create_callback()
			{
				shared_ptr<prtyPropertyCallback> pCallback(new prtyCallbackWrapper<DriverMonitor>(this, &DriverMonitor::PropertyChanged));
				m_Callbacks.push_back(pCallback);
				return pCallback;
			}

			//--------------------------------------------------------------------
			// From envAuditor, this callback is called when the reference 
			//	is deleted, so that we can break the connection.
			//--------------------------------------------------------------------
			virtual void AuditorNotify(envAuditable* i_pReference)
			{
				// In this case, the driver is being deleted before this monitor.
				// So we need to remove our references to this driver and its callbacks.
				//
				// Just clear the list of callbacks, ownership is shared
				m_Callbacks.clear();
			}


			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			virtual void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
			{
				chnlDialogUtil::UpdateDriver(&m_Driver);
			}

			tmlnDriver &m_Driver;
			std::vector< shared_ptr<prtyPropertyCallback> > m_Callbacks;
		};
	
		std::vector<DriverMonitor*> l_Monitors;
		int copyCount = 0;
		
		//--------------------------------------------------------------------
		// find which of the given objects contains the given driver
		//--------------------------------------------------------------------
		tmlnScriptObject* find_driver_owner(tmlnDriver *i_pDriver,
											std::vector<tmlnScriptObject*>& i_Objects)
		{
			for (int i=0; i<i_Objects.size(); i++)
			{
				if (i_Objects[i]->HasDriver(i_pDriver))
					return i_Objects[i];
			}
			return NULL;
		}									
	}

	//--------------------------------------------------------------------
	// Show available drivers for selected object
	//--------------------------------------------------------------------
	void ShowAvailableDrivers()
	{
		cmmObjectDialogUtil::ShowDrivers();
	}
	
	//--------------------------------------------------------------------
	// Delete selected drivers from selected script objects
	//--------------------------------------------------------------------
	void DeleteSelectedDrivers()
	{
		//	Clear out the driver dialog
		//	NOTE: this goes first to avoid a crash
		tmlnDriverDialogUtil::ClearDriverProperties();

		// First, gather up the drivers that need to be deleted
		std::set<tmlnDriver*> drivers_to_delete;
		const bool skip_locked = true;
		chnlDialogUtil::GetSelectedDrivers(drivers_to_delete, skip_locked);

		//	Now delete the drivers in a single undo operation
		chnlOperations::DeleteDrivers(drivers_to_delete);

		//	Reset our display
		chnlDialogUtil::UpdateChannels();
	}

	//--------------------------------------------------------------------
	// Delete given drivers from selected script objects
	//--------------------------------------------------------------------
	void DeleteDrivers(std::set<tmlnDriver*>& i_Drivers)
	{
		std::vector<tmlnScriptObject*> objects;
		tmlnSelectionUtil::GetSelectedScriptObjects(objects);
		const int num_objs = objects.size();

		if (i_Drivers.size() > 1)
			undoUndoMgr::BeginMultipleOperationBlock("Delete Drivers");

		std::set<tmlnDriver*>::iterator it, end = i_Drivers.end();
		for (it = i_Drivers.begin(); it != end; ++it)
		{
			tmlnDriver *pDriver = (*it);
			for (int i=0; i<num_objs; ++i)
			{
				tmlnScriptObject *script_obj = objects[i];
				int driver_index = 0;
				if (script_obj->RemoveDriver(pDriver, driver_index))
				{
					// Backup the driver info in an undo operation
					tmlnDriverInfo* pInfo = pDriver->GetDriverInfo();
					undoUndoMgr::AddOperation( new chnlDeleteDriverOperation(script_obj->CreateReferenceToSelf(), pInfo, driver_index) );

					script_obj->RemoveDriver(pDriver);
					delete pDriver;
					script_obj->NotifyDriverChanged();
					break;
				}
			}
		}

		if (i_Drivers.size() > 1)
			undoUndoMgr::EndMultipleOperationBlock();
	}

	//--------------------------------------------------------------------
	// Set blend type for all selected drivers at once
	//--------------------------------------------------------------------
	void SetSelectedDriversBlend(tmlnDriver::BlendType i_BlendType)
	{
		std::set<tmlnDriver*> drivers_selected;
		const bool skip_locked = true;
		chnlDialogUtil::GetSelectedDrivers(drivers_selected, skip_locked);

		if (!drivers_selected.empty())
			undoUndoMgr::BeginMultipleOperationBlock("Set Driver Blend");

		std::set<tmlnDriver*>::iterator cur_it;
		for(cur_it = drivers_selected.begin(); cur_it != drivers_selected.end() ; ++cur_it )
		{
			tmlnDriver* pDriver = *cur_it;
			if (pDriver != NULL)
			{
				pDriver->CreateUndoForProperty(pDriver->PropertyBlendType());
				pDriver->SetBlendType(i_BlendType);
			}
		}

		if (!drivers_selected.empty())
			undoUndoMgr::EndMultipleOperationBlock();

	}

	//--------------------------------------------------------------------
	//Cut driver and store it in the clipboard
	//--------------------------------------------------------------------
	void CutDrivers()
	{
		//	clean-up the clipboard
		//
		//clean_channel_clipboard();
		tmlnDriverClipboard::Clear();

		//	First, gather up the selected drivers
		//
		std::set<tmlnDriver*> drivers_selected;
		const bool skip_locked = true;
		chnlDialogUtil::GetSelectedDrivers(drivers_selected, skip_locked);

		//	add each one to the driver clipboard
		//
		std::set<tmlnDriver*>::iterator cur_it;
		for(cur_it = drivers_selected.begin(); cur_it != drivers_selected.end() ; ++cur_it )
		{
			tmlnDriver* pDriver = *cur_it;
			if (pDriver != NULL)
			{
				tmlnDriverClipboard::Copy( pDriver );
				//DBG_LOG2("Copied %d! %s", tmlnDriverClipboard::Count(), pDriver->GetName().c_str());
			}
		}

		DeleteSelectedDrivers();
	}
	//--------------------------------------------------------------------
	// Copy information about selected drivers to clipboard
	//--------------------------------------------------------------------
	
	void CopyDrivers()
	{
		//	clean-up the clipboard
		//
		//clean_channel_clipboard();
		tmlnDriverClipboard::Clear();
		std::vector<tmlnScriptObject*> objects;
		tmlnSelectionUtil::GetSelectedScriptObjects(objects);

		//	First, gather up the selected drivers
		//
		std::set<tmlnDriver*> drivers_selected;
		std::list<tmlnScriptObject*> selection;
		const bool skip_locked = true;
		chnlDialogUtil::GetSelectedDrivers(drivers_selected, skip_locked);

		//	add each one to the driver clipboard
		//
		std::set<tmlnDriver*>::iterator cur_it;
		for(cur_it = drivers_selected.begin(); cur_it != drivers_selected.end() ; ++cur_it )
		{

			tmlnDriver* pDriver = *cur_it;
			//DBG_LOG("No of objects sel :"<<selection.size());
			
			if (pDriver != NULL)
			{
				tmlnScriptObject* pScriptObj = find_driver_owner(pDriver, objects);
				selection.push_front(pScriptObj);
				//envSTLHelpers::RemoveOneValue(selection,pScriptObj);
				selection.unique();
				tmlnDriverClipboard::Copy( pDriver );
				//DBG_LOG2("Copied %d! %s", tmlnDriverClipboard::Count(), pDriver->GetName().c_str());
			}
		}
		copyCount = selection.size();
	}

	//--------------------------------------------------------------------
	// Create new drivers based on information in clipboard
	//--------------------------------------------------------------------
	void PasteDrivers()
	{
		//	Note: paste had problems with multiple clips across multiple objects
		//
		int io_count;
		io_count = tmlnSelectionUtil::GetSelectedScriptObjectsCount();
		DBG_TRACE("No.Selected objects : "<< io_count);
		DBG_TRACE("No.objects copied : "<<copyCount);
		tmlnScriptObject* script_obj = tmlnSelectionUtil::GetSelectedScriptObject();
		std::vector<tmlnScriptObject*> o_Objects;
		
		tmlnSelectionUtil::GetSelectedScriptObjects(o_Objects);
		
		if( io_count >= copyCount)
		{
			std::vector<tmlnScriptObject*>::const_iterator it, end = o_Objects.end();
			for(it = o_Objects.begin(); it!=end; ++it)
			{
				if(*it!= 0)
				{
				tmlnDriverClipboard::Paste(*it);
				chnlDialogUtil::ObjectSelected(*it);
				}
			}
		}

		else if ( io_count < copyCount)
		{
			if (script_obj != 0)
			{
				tmlnDriverClipboard::Paste(script_obj);
				chnlDialogUtil::ObjectSelected(script_obj);
			}
		}
		/*if (script_obj != 0)
		{
			tmlnDriverClipboard::Paste(script_obj);

			chnlDialogUtil::ObjectSelected(script_obj);
		}*/
	}

	//--------------------------------------------------------------------
	// Split selected drivers at current time
	//--------------------------------------------------------------------
	void SplitDrivers()
	{
		maTime split_time = tmlnTimeLine::GetValue();
		
		// Get all the selected script objects
		std::vector<tmlnScriptObject*> objects;
		tmlnSelectionUtil::GetSelectedScriptObjects(objects);

		//	First, gather up the selected drivers
		//
		std::set<tmlnDriver*> drivers_selected;
		const bool skip_locked = true;
		chnlDialogUtil::GetSelectedDrivers(drivers_selected, skip_locked);

		std::set<tmlnDriver*>::iterator cur_it;
		for(cur_it = drivers_selected.begin(); cur_it != drivers_selected.end() ; ++cur_it )
		{
			tmlnDriver* pDriver = *cur_it;
			if (pDriver != NULL)
			{
				//	if the time isn't within the selected driver, jump out.
				if (   split_time > pDriver->GetBeginTime()
					&& split_time < pDriver->GetEndTime() )
				{
					// find the driver owner from the selected objects list
					tmlnScriptObject* pScriptObj = find_driver_owner(pDriver, objects);
					if (pScriptObj)
					{
						//	split the driver
						//
						//DBG_LOG("      cloned");
						tmlnDriver* pDriver2 = tmlnCreator::CloneDriver(pScriptObj, pDriver);
						pDriver->Split( pDriver2, split_time );
					}
				}
			}
		}

		//	Reset our display
		chnlDialogUtil::UpdateChannels();
	}

	//--------------------------------------------------------------------
	// Move time forward or backwards to next time tick
	//--------------------------------------------------------------------
	void MoveTimeNextTick()
	{
		maTime mtime = chnlTimeTickMgr::GetNextTickTime(tmlnTimeLine::GetValue());
		if (mtime < maTime::c_ZeroTime)
			tmlnTimeLine::SetValue(tmlnTimeLine::GetMaximum());
		else
			tmlnTimeLine::SetValue(mtime);
		chnlDialogUtil::UpdateChannels();
	}
	void MoveTimePrevTick()
	{
		maTime mtime = chnlTimeTickMgr::GetPrevTickTime(tmlnTimeLine::GetValue());
		if (mtime < maTime::c_ZeroTime)
			tmlnTimeLine::SetValue(tmlnTimeLine::GetMinimum());
		else
			tmlnTimeLine::SetValue(mtime);
		chnlDialogUtil::UpdateChannels();

	}

	//--------------------------------------------------------------------
	//  When a driver's properties have changed, need to notify the
	//		script object that there were changes so the corresponding
	//		data can be updated and dirty bits set.
	//--------------------------------------------------------------------
	void  NotifyScriptObject()
	{
		tmlnScriptObject *script_obj = tmlnSelectionUtil::GetSelectedScriptObject();
		if (script_obj)
			script_obj->NotifyDriverChanged();
		
	}

	//--------------------------------------------------------------------
	// Set up callbacks for this driver to update trax editor display
	//--------------------------------------------------------------------
	void MonitorChanges(tmlnDriver &i_Driver)
	{
		l_Monitors.push_back(new DriverMonitor(i_Driver));
	}

	//--------------------------------------------------------------------
	// Clear out all the monitoring of drivers
	//--------------------------------------------------------------------
	void ClearMonitors()
	{
		envSTLHelpers::DeleteContainer(l_Monitors);
	}

}	// end of namespace

