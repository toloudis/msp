/*****************************************************************************
**	chnlDialogUtil.cpp
**
**	API for opening dialogs for channel editor
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlDialogUtil.hpp"

#include "Features/Channels/Data/chnlTimeData.hpp"
#include "Features/Channels/mGUI/chnlTraxEditor.h"
#include "Features/Channels/wxGUI/chnlTraxDialog.hpp"

#include "Features/Prefs/PrefsData.hpp"
#include "Support/tmln/tmlnTimeInterest.hpp"

#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#include <algorithm>


//============================================================================
//============================================================================
#ifdef _MANAGED
using namespace StudioFramework;
#endif


//============================================================================
//============================================================================
namespace chnlDialogUtil
{
	namespace
	{
		class chnlTimeInterest : public tmlnTimeInterest
		{
			virtual void TimeChanged(float i_Time)
			{
#ifdef _MANAGED
				if (chnlTraxEditor::FormInstance)
					chnlTraxEditor::FormInstance->UpdateCurrentTime(i_Time);
#endif
#ifdef USE_WXWIDGETS
				if (chnlTraxDialog::FormInstance)
					chnlTraxDialog::FormInstance->UpdateCurrentTime();
#endif
			}
			virtual void TimeRangeChanged(float i_MinTime, float i_MaxTime)
			{
#ifdef USE_WXWIDGETS
				if (chnlTraxDialog::FormInstance)
					chnlTraxDialog::FormInstance->SetTimeRange(i_MinTime, i_MaxTime);
#endif
			}
			virtual void TimeFormatChanged(int i_TimeFormat)
			{
#ifdef _MANAGED
				if (chnlTraxEditor::FormInstance)
				{
					chnlTraxEditor::FormInstance->UpdateTimeLabel();
				}
#endif
#ifdef USE_WXWIDGETS
				if (chnlTraxDialog::FormInstance)
					chnlTraxDialog::FormInstance->UpdateTimeLabel();
#endif
			}
		};

		chnlTimeInterest* l_pInterest = 0;
		tmlnScriptObject* l_pSelObject = 0;

		bool l_bCanAddChannels = false;

		//float l_TotalTime =  60.0f;

		chnlTimeData	l_Data;

		//	
		struct time_match
		{
			time_match(float i_Time) : m_Time(i_Time) {}
			bool operator()(const chnlMarkerDataItem &i_Item) 
				{  return i_Item.m_Time == m_Time; }
			bool operator()(const chnlNoteDataItem &i_Item) 
				{  return i_Item.m_Time == m_Time; }
			float m_Time;
		};

	}	// end of namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
		l_pInterest = new chnlTimeInterest();
		tmlnTimeLine::AddTimeInterest(l_pInterest);

		PrefsData& data = PrefsMgr::Data();
		//l_TotalTime = (float)data.m_ChannelEditor_DefaultTime.GetValue();
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
#ifdef _MANAGED
		chnlTraxEditor::FormInstance = nullptr;
#endif

		if (l_pInterest)
		{
			tmlnTimeLine::RemoveTimeInterest(l_pInterest);
			delete l_pInterest;
			l_pInterest = NULL;
		}
	}

	//--------------------------------------------------------------------
	//  Create trax editor and show it if dialog memory has it open
	//--------------------------------------------------------------------
	void  CreateChannelEditor()
	{
#ifdef _MANAGED
		if (!chnlTraxEditor::FormInstance)
		{
			chnlTraxEditor::FormInstance = gcnew chnlTraxEditor();
			tmaSystem::g_pMainForm->AddOwnedForm(chnlTraxEditor::FormInstance);
		}
#endif
#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance == NULL)
		{
			DBG_ASSERT0(twxSystem::g_pMainForm, "MainForm not yet initialized.");
			chnlTraxDialog::FormInstance = new chnlTraxDialog(twxSystem::g_pMainForm);
		}
#endif
	}

	//--------------------------------------------------------------------
	//  Show dialog to edit channels
	//--------------------------------------------------------------------
	void  ShowChannelEditor()
	{
#ifdef _MANAGED
		// Use class to keep track of instance if already visible
		if (!chnlTraxEditor::FormInstance)
		{
			chnlTraxEditor::FormInstance = gcnew chnlTraxEditor();
			tmaSystem::g_pMainForm->AddOwnedForm(chnlTraxEditor::FormInstance);
		}

		chnlTraxEditor::FormInstance->SetTotalTime(l_TotalTime);
		chnlTraxEditor::FormInstance->Select(l_pSelObject);
		chnlTraxEditor::FormInstance->SetCanAddChannels(l_bCanAddChannels);
		chnlTraxEditor::FormInstance->Show();
		if (chnlTraxEditor::FormInstance->WindowState == FormWindowState::Minimized)
			chnlTraxEditor::FormInstance->WindowState = FormWindowState::Normal;
#endif
#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance == NULL)
		{
			DBG_ASSERT0(twxSystem::g_pMainForm, "MainForm not yet initialized.");
			chnlTraxDialog::FormInstance = new chnlTraxDialog(twxSystem::g_pMainForm);
		}
		//chnlTraxDialog::FormInstance->SetTotalTime(l_TotalTime);
		chnlTraxDialog::FormInstance->Select(l_pSelObject);
		//chnlTraxDialog::FormInstance->SetCanAddChannels(l_bCanAddChannels);
		twxPaneMgr::Show(chnlTraxDialog::FormInstance);
		//if (chnlTraxDialog::FormInstance->WindowState == FormWindowState::Minimized)
		//	chnlTraxDialog::FormInstance->WindowState = FormWindowState::Normal;
#endif

		UpdateMarkersAndNotes();
	}

	//--------------------------------------------------------------------
	//  If you alter driver's properties and want the channel
	//	editor updated, call this function
	//--------------------------------------------------------------------
	void  UpdateChannels()
	{
#ifdef _MANAGED
		if (chnlTraxEditor::FormInstance)
			chnlTraxEditor::FormInstance->Update();
#endif
#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance)
			chnlTraxDialog::FormInstance->UpdateChannels();
#endif
	}

	//--------------------------------------------------------------------
	//  This driver has changed its properties related to the
	//	trax editor display, so update the channels related to it.
	//--------------------------------------------------------------------
	void  UpdateDriver(tmlnDriver *i_pDriver)
	{
#ifdef _MANAGED
		if (chnlTraxEditor::FormInstance)
			chnlTraxEditor::FormInstance->UpdateDriver(i_pDriver);
#endif
#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance)
			chnlTraxDialog::FormInstance->UpdateDriver(i_pDriver);
#endif
	}

	//--------------------------------------------------------------------
	//  Called when a driver's time is altered, it updates the ticks
	//		for the snapping points.
	//--------------------------------------------------------------------
	void  UpdateTimeTicks()
	{
		// Update time ticks in manager
		chnlTimeTickMgr::UpdateTimeTicks();

		// Update display of time ticks
#ifdef _MANAGED
		if (chnlTraxEditor::FormInstance)
			chnlTraxEditor::FormInstance->UpdateTimeTicks();
#endif
#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance)
			chnlTraxDialog::FormInstance->UpdateTimeTicks();
#endif
	}

	//------------------------------------------------------------------------
	//	Update Markers and Notes in trax editor
	//------------------------------------------------------------------------
	void UpdateMarkersAndNotes()
	{
#ifdef _MANAGED
		if (chnlTraxEditor::FormInstance)
			chnlTraxEditor::FormInstance->UpdateMarkersAndNotes();
#endif
#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance)
			chnlTraxDialog::FormInstance->UpdateMarkersAndNotes();
#endif
	}

	//--------------------------------------------------------------------
	// Update channel editor based on selection, which can be NULL
	//--------------------------------------------------------------------
	void ObjectSelected(tmlnScriptObject* i_pObject)
	{
		l_pSelObject = i_pObject;

#ifdef _MANAGED
		if (chnlTraxEditor::FormInstance)
			chnlTraxEditor::FormInstance->Select(l_pSelObject);
#endif
#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance)
			chnlTraxDialog::FormInstance->Select(l_pSelObject);
#endif

	}

	//--------------------------------------------------------------------
	// Call this depending on if an object is selected that can
	//	have a script object created for it.
	//--------------------------------------------------------------------
	void SetCanAddChannels(bool i_bCanAdd)
	{
		l_bCanAddChannels = i_bCanAdd;

#ifdef _MANAGED
		if (chnlTraxEditor::FormInstance)
			chnlTraxEditor::FormInstance->SetCanAddChannels(l_bCanAddChannels);
#endif
	}

	//--------------------------------------------------------------------
	// Set amount of time covered by timeline
	//--------------------------------------------------------------------
	void SetTimeRange(float i_MinTime, float i_MaxTime)
	{
		//l_TotalTime = i_TotalTime;

#ifdef _MANAGED
		if (chnlTraxEditor::FormInstance)
			chnlTraxEditor::FormInstance->SetTotalTime(i_MaxTime);
#endif
#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance)
			chnlTraxDialog::FormInstance->SetTimeRange(i_MinTime, i_MaxTime);
#endif
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//chnlTimeData& Data()
	//{
	//	return l_Data;
	//}

	//------------------------------------------------------------------------
	// Get set of drivers that are selected in the trax editor. If skip locked
	// is true, then only drivers in channels that are unlocked will be returned.
	//------------------------------------------------------------------------
	void GetSelectedDrivers(std::set<tmlnDriver*> &o_Drivers, bool i_bSkipLocked)
	{
#ifdef _MANAGED
		if (chnlTraxEditor::FormInstance)
			chnlTraxEditor::FormInstance->GetSelectedDrivers(o_Drivers, i_bSkipLocked);
#endif
#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance)
			chnlTraxDialog::FormInstance->GetSelectedDrivers(o_Drivers, i_bSkipLocked);
#endif
	}

	//------------------------------------------------------------------------
	// Deselect all drivers in the channel interface
	//------------------------------------------------------------------------
	void ClearSelection()
	{
#ifdef _MANAGED
		if (chnlTraxEditor::FormInstance)
			chnlTraxEditor::FormInstance->ClearSelection();
#endif
#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance)
			chnlTraxDialog::FormInstance->ClearSelection();
#endif
	}

	//------------------------------------------------------------------------
	// Select driver in the channel interface - this will do an append
	//	to the selection, it will not deselect other drivers.
	//------------------------------------------------------------------------
	void AddToSelection( tmlnDriver* i_pDriver )
	{
#ifdef _MANAGED
		if (chnlTraxEditor::FormInstance)
			chnlTraxEditor::FormInstance->AddToSelection(i_pDriver);
#endif
#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance)
			chnlTraxDialog::FormInstance->AddToSelection(i_pDriver);
#endif
	}


}	// end of namespace
