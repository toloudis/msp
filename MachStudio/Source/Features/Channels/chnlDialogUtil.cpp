/*****************************************************************************
**	chnlDialogUtil.cpp
**
**	API for opening dialogs for channel editor
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlDialogUtil.hpp"

#include "Features/Channels/chnlOperations.hpp"
#include "Features/Channels/chnlSnapUtil.hpp"
#include "Features/Channels/chnlTimeTickMgr.hpp"
#include "Features/Channels/Data/chnlTimeData.hpp"
#include "Features/Channels/Data/chnlTimeDocumentChunk.hpp"
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Features/Channels/Markers/chnlMarkerOperations.hpp"
#include "Features/Channels/Notes/chnlNotesMgr.hpp"
#include "Features/Channels/Notes/chnlNotesOperations.hpp"
#include "Features/Channels/wxGUI/chnlTraxDialog.hpp"

#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/prefsMgr.hpp"
#include "Support/tmln/tmlnChannel.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnDriverClipboard.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeLineMgr.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/name/nameObject.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"

#include <algorithm>


//============================================================================
//============================================================================
namespace chnlDialogUtil
{
	namespace
	{
		class chnlTimeInterest : public tmlnTimeInterest
		{
			virtual void TimeChanged(const maTime& i_Time)
			{
#ifdef USE_WXWIDGETS
				if (chnlTraxDialog::FormInstance)
					chnlTraxDialog::FormInstance->UpdateCurrentTime();
#endif
			}
			virtual void TimeRangeChanged(const maTime& i_MinTime, const maTime& i_MaxTime)
			{
#ifdef USE_WXWIDGETS
				if (chnlTraxDialog::FormInstance)
					chnlTraxDialog::FormInstance->SetTimeRange(i_MinTime, i_MaxTime);
#endif
			}
			virtual void TimeFormatChanged(int i_TimeFormat)
			{
#ifdef USE_WXWIDGETS
				if (chnlTraxDialog::FormInstance)
				{
					chnlTraxDialog::FormInstance->UpdateTimeLabel();
				}
#endif
			}

			virtual void FrameRateChanged( float i_FrameRate )
			{
#ifdef USE_WXWIDGETS
				//if (chnlTraxDialog::FormInstance)
				//	chnlTraxDialog::FormInstance->UpdateTimeLabel();
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
			time_match(maTime i_Time) : m_Time(i_Time) {}
			bool operator()(const chnlMarkerDataItem &i_Item) 
				{  return i_Item.m_Time == m_Time; }
			bool operator()(const chnlNoteDataItem &i_Item) 
				{  return i_Item.m_Time == m_Time; }
			maTime m_Time;
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

#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance == NULL)
		{
			DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
			chnlTraxDialog::FormInstance = new chnlTraxDialog(twxSystem::g_pMainForm);
		}
#endif
	}

	//--------------------------------------------------------------------
	//  Show dialog to edit channels
	//--------------------------------------------------------------------
	void  ShowChannelEditor()
	{

#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance == NULL)
		{
			DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
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
		UpdateZoom();
	}

	//--------------------------------------------------------------------
	//  If you alter driver's properties and want the channel
	//	editor updated, call this function
	//--------------------------------------------------------------------
	void  UpdateChannels()
	{

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

#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance)
			chnlTraxDialog::FormInstance->UpdateDriver(i_pDriver);
#endif
	}
	//------------------------------------------------------------------------
	// Make sure that the timeline editor has max zoom on the length
	// of time in the scene.
	//------------------------------------------------------------------------
	void  UpdateZoom()
	{
#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance)
			chnlTraxDialog::FormInstance->CalculateMaxZoom();
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

	}

	//--------------------------------------------------------------------
	// Set amount of time covered by timeline
	//--------------------------------------------------------------------
	void SetTimeRange(const maTime& i_MinTime, const maTime& i_MaxTime)
	{
		//l_TotalTime = i_TotalTime;


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

#ifdef USE_WXWIDGETS
		if (chnlTraxDialog::FormInstance)
			chnlTraxDialog::FormInstance->AddToSelection(i_pDriver);
#endif
	}


}	// end of namespace
