#pragma once
#include "MainApp/stdafx.h"
#include <windows.h>

#ifndef CHNL_TIMEDOCUMENTCHUNK_HPP
#include "Features/Channels/Data/chnlTimeDocumentChunk.hpp"
#endif
#ifndef CHNL_DRIVERCLIP_HPP
#include "Features/Channels/mGUI/chnlDriverClip.hpp"
#endif
#ifndef CHNL_OPERATIONS_HPP
#include "Features/Channels/chnlOperations.hpp"
#endif
#ifndef CHNL_MARKEROPERATIONS_HPP
#include "Features/Channels/Markers/chnlMarkerOperations.hpp"
#endif
#ifndef CHNL_MARKERMGR_HPP
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#endif
#ifndef CHNL_NOTESOPERATIONS_HPP
#include "Features/Channels/Notes/chnlNotesOperations.hpp"
#endif
#ifndef CHNL_NOTESMGR_HPP
#include "Features/Channels/Notes/chnlNotesMgr.hpp"
#endif
#ifndef CHNL_SNAPUTIL_HPP
#include "Features/Channels/chnlSnapUtil.hpp"
#endif
#ifndef CHNL_TIMETICKMGR_HPP
#include "Features/Channels/chnlTimeTickMgr.hpp"
#endif
#ifndef CMM_OBJECTDIALOGUTIL_HPP
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#endif
#ifndef PREFSMGR_HPP
#include "Features/Prefs/prefsMgr.hpp"
#endif
#ifndef TMA_DIALOGTABBEDMGR_HPP
#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"
#endif
#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif
#ifndef TMLN_CHANNELSET_HPP
#include "Support/tmln/tmlnChannelSet.hpp"
#endif
#ifndef TMLN_CREATOR_HPP
#include "Support/tmln/tmlnCreator.hpp"
#endif
#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef TMLN_DRIVERCLIPBOARD_HPP
#include "Support/tmln/tmlnDriverClipboard.hpp"
#endif
#ifndef TMLN_SCRIPTOBJECT_HPP
#include "Support/tmln/tmlnScriptObject.hpp"
#endif
#ifndef TMLN_SELECTIONUTIL_HPP
#include "Support/tmln/tmlnSelectionUtil.hpp"
#endif
#ifndef TMLN_TIMELINE_HPP
#include "Support/tmln/tmlnTimeLine.hpp"
#endif
#ifndef TMLN_TIMELINEMGR_HPP
#include "Support/tmln/tmlnTimeLineMgr.hpp"
#endif
#ifndef TMLN_TIMEUTIL_HPP
#include "Support/tmln/tmlnTimeUtil.hpp"
#endif

#ifndef CMA_COMMANDMGR_HPP
#include "Tool/cma/cmaCommandMgr.hpp"
#endif
#ifndef TMA_MESSAGING_HPP
#include "ToolUIManaged/tma/tmaMessaging.hpp"
#endif
#ifndef DBG_ASSERT_HPP
#include "Core/dbg/dbgAssert.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "Core/dbg/dbgLog.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef G3D_CONSTANTS_HPP
#include "Graphics/g3d/g3dConstants.hpp"
#endif
#ifndef GF_PATHS_HPP
#include "Core/gf/gfPaths.hpp"
#endif
#ifndef MNM_PATHS_HPP
#include "Support/mnm/mnmPaths.hpp"
#endif
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif
#ifndef TMA_MANAGEDCONTROLUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedControlUtil.hpp"
#endif

#ifdef _MANAGED
//#include "Features/Channels/mGUI/chnlDriverSelectForm.h"
#include "Features/Channels/mGUI/chnlMoveDriversForm.h"
#endif

#include <assert.h>

#ifdef _MANAGED

//============================================================================
//============================================================================
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace TimelineControls;
using namespace StudioFramework;
//using namespace TimelineControls;
class tmlnScriptObject;



//============================================================================
//============================================================================
namespace StudioFramework
{
	/// <summary> 
	/// Summary for chnlTraxEditor
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	//====================================================================
	// 
	//====================================================================
	public ref class chnlTraxEditor : public System::Windows::Forms::Form	
	{

	public: static chnlTraxEditor^ FormInstance = nullptr;

	public: static const int c_NamePanelOffsetY	= 0;
	public: static const int c_NamePanelOffsetX	= 0;
	public: static const int c_NameLabelWidth	= 164;		// MUST be less than width of panel_names
	public: static const int c_ChannelHeight	= 32;
	public: static const int c_ChannelOffsetX	= 8;
	public: static const int c_LabelWidth		= c_NameLabelWidth - c_ChannelOffsetX;
	private: static const int c_ScalePanelWidthExtra	= 100;
	private: static const int c_SnapIntervalDefault = 120;
	private: static const float c_TimeTickInc	= 0.01f;
	private: static const int c_ChannelLockedCheckbox_OffsetX = 20;
	private: static const int c_ChannelLockedCheckbox_OffsetY = 6;
	private: static const int c_ChannelControl_Width	= 216;

	public:
		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		chnlTraxEditor(void) 
		:	m_SnapInterval(c_SnapIntervalDefault),
			m_nFilterState(0)
		{
			channelList		= gcnew ArrayList();
			channelLockList = gcnew ArrayList();
			labelList		= gcnew ArrayList();
			labelObjectList = gcnew ArrayList();
			m_ChannelCount	= gcnew ArrayList();
			//m_ChannelClipboard = gcnew ArrayList();
			//m_DriverClipboard = gcnew ArrayList();

			InitializeComponent();

			//	programmer customized control/form set-up
			SetUpComponents();
		}

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
	public: void UpdateMarkersAndNotes()
		{
			this->markerBar1->Clear();

			//	Markers
			for (int i=0; i < chnlMarkerMgr::GetNumMarkers(); ++i)
			{
				System::String^ note;
				const chnlMarkerDataItem& mdata = chnlMarkerMgr::GetMarkerData(i);
				note = gcnew System::String( mdata.m_Note.GetValue().c_str());
				this->markerBar1->AddMarker( mdata.m_Time.GetValue(), 
											 mdata.m_TimeMarkerType.GetValue(),
											 note );
			}

			//	Notes
			for (int i=0; i < chnlNotesMgr::GetNumNotes(); ++i)
			{
				System::String^ note;
				const chnlNoteDataItem& ndata = chnlNotesMgr::GetNoteData(i);
				note = gcnew System::String(ndata.m_Note.GetValue().c_str());
				this->markerBar1->AddNote( ndata.m_Time.GetValue(), 
											 ndata.m_Status.GetValue(),
											 note );
			}
		}

	public: void UpdateCurrentTime(float i_Time)
		{
			//	update the time so it falls on a valid time
			tmlnTimeUtil::AdjustTimeToFrame( i_Time );

			// set the time
			this->timeSlider1->CurTime = (double)i_Time;
		}

	public: void UpdateTimeLabel()
		{
			//	update the time label
			//
			std::string time_string;
			tmlnTimeUtil::GetTimeString(tmlnTimeLine::GetValue(), time_string);
			label_time->Text = gcnew String(time_string.c_str());

			int frames;
			tmlnTimeUtil::GetTimeInFrames(tmlnTimeLine::GetValue(), frames);
			label_time_frames->Text = System::String::Format("[{0:0000}]", frames);

			//	title bar text
			System::String^ tbtext = System::String::Concat(label_time->Text, label_time_frames->Text);
			set_titlebar( tbtext );
		}

		//	based on the length of the panel, calculate the zoom so the whole timeline
		//	is visible on screen.
		//
	public: void CalculateMaxZoom()
		{
			float totaltime = tmlnTimeLine::GetMaximum();
			int label_width = timeLabel1->Width + c_ScalePanelWidthExtra;	// FIX: [rjk] what is this c_ScalePanelWidthExtra for? make it a constant with an explaination
			int panel_width = panel_timeline->Width - (c_ScalePanelWidthExtra / 2);
			float zoom = panel_width / totaltime;

			if (zoom < rangedFloat_Zoom->Minimum)
				zoom = (float)rangedFloat_Zoom->Minimum;
			else if (zoom > rangedFloat_Zoom->Maximum)
				zoom = (float)rangedFloat_Zoom->Maximum;
			this->rangedFloat_Zoom->Value = zoom;
		}

	public: void SetTotalTime(float i_Time)
		{
			this->timeLabel1->TotalTime		= i_Time;
			this->timeSlider1->TotalTime	= i_Time;
			this->markerBar1->TotalTime		= i_Time;
			int width = timeLabel1->Width + c_ScalePanelWidthExtra;
			this->panel_channels->AutoScrollMinSize.Width = width;
			this->panel_timeline->AutoScrollMinSize.Width = width;

			for (int i=0; i<this->channelList->Count; i++)
			{	
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
				channel->TotalTime = i_Time;
			}		

			//	check the markers to see if they are past the end time.
			//
			//for (int k=0; k < this->markerBar1->MarkerCount; ++k)
			//{
			//	float mtime = (float)this->markerBar1->Marker(k)->Time;

			//	// Yoni wants the markers to stay in the position they were set at. -rjk
			//	//
			//	//if ( mtime > i_Time )
			//	//{
			//	//	this->markerBar1->Marker(k)->Time = i_Time;
			//	//}
			//}

			CalculateMaxZoom();
		}

	// TimeScale changes teh zoom left/right
	public: void SetTimeScale(float i_Scale)
		{
			this->timeLabel1->TimeScale		= i_Scale;
			this->timeSlider1->TimeScale	= i_Scale;
			this->markerBar1->TimeScale		= i_Scale;

			//int width = timeLabel1->Width + c_ScalePanelWidthExtra;	// FIX: [rjk] what is this c_ScalePanelWidthExtra for? make it a constant with an explaination
			//this->panel_channels->AutoScrollMinSize.Width = width;
			//this->panel_timeline->AutoScrollMinSize.Width = width;

			//DBG_LOG4( "label width (%d) panel width (%d)  scale (%6.3f) total time(%6.3f)", width, panel_timeline->Width, i_Scale, tmlnTimeLine::GetMaximum());

			for (int i=0; i < channelList->Count; i++)
			{	
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
				channel->TimeScale = i_Scale;
			}

			//	normally this would be called ONLY when the scale changes
			TimeGuideLineMgr::UpdateScale( i_Scale );

			//	adjust the timeline position based on the selected object or current time.
			//
			center_view();
		}

	public: void SetSnapInterval(int i_Time)
		{
			m_SnapInterval = i_Time;
			for (int i=0; i<this->channelList->Count; i++)
			{	
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
				channel->SnapInterval = m_SnapInterval;
			}	
		}

	public: void SetCanAddChannels(bool i_bCanAdd)
		{
			this->buttonAdd->Enabled = i_bCanAdd;
		}

	public: void Update()
		{
			setup_display(tmlnSelectionUtil::GetSelectedScriptObject());

			center_view();
		}

		//--------------------------------------------------------------------
		//  This driver has changed its properties related to the
		//	trax editor display, so update the channels related to it.
		//--------------------------------------------------------------------
	public: void UpdateDriver(tmlnDriver *i_pDriver)
		{
			for (int i=0; i<this->channelList->Count; i++)
			{	
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
				for (int c=0; c<channel->Clips->Count; c++)
				{
					chnlDriverClip^ clip = dynamic_cast<chnlDriverClip^>(channel->Clips[c]);
					if (clip)
					{
						if (&clip->Driver() == i_pDriver)
						{
							clip->Update();

							// invalidate the control in order to force a redraw soon
							channel->Invalidate();

							// Note, driver can occur in more than one channel, 
							// but only once per channel. So, one level "break" 
							// is used here instead of a "return"
							break;
						}
					}
				}
			}
		}

	public: void Select(tmlnScriptObject* i_pObject)
		{
			setup_display(i_pObject);

			center_view();
		}
			
		//------------------------------------------------------------------------
		// Deselect all drivers in the channel interface
		//------------------------------------------------------------------------
	public: void ClearSelection()
		{
			for (int i=0; i<this->channelList->Count; i++)
			{	
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
				channel->ClearSelection();
			}
		}

		//------------------------------------------------------------------------
		// Select driver in the channel interface - this will do an append
		//	to the selection, it will not deselect other drivers.
		//------------------------------------------------------------------------
	public: void AddToSelection( tmlnDriver* i_pDriver )
		{
			for (int i=0; i<this->channelList->Count; i++)
			{	
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
			
				for (int c=0; c<channel->Clips->Count; c++)
				{
					chnlDriverClip^ clip = dynamic_cast<chnlDriverClip^>(channel->Clips[c]);
					if (clip)
					{
						if (&clip->Driver() == i_pDriver)
						{
							const bool append_selection = true;
							channel->SelectClip(c, append_selection);
						}
					}
				}
			}
		}


	public: void UpdateTimeTicks()
		{
			this->timeSlider1->ClearTimeTicks();

			const std::set<float>& time_ticks = chnlTimeTickMgr::GetTimeTicks();
			std::set<float>::const_iterator it;
			for (it = time_ticks.begin(); it != time_ticks.end(); ++it)
			{
				this->timeSlider1->AddTimeTick(*it);
			}
		}

	public: void TimelineZoomIn()
			{
				float newvalue = (float)rangedFloat_Zoom->Value;
				newvalue++;
				if (newvalue > rangedFloat_Zoom->Maximum) 
					newvalue = (float)rangedFloat_Zoom->Maximum;
				rangedFloat_Zoom->Value = newvalue;
				center_view();
			}
	public: void TimelineZoomOut()
			{
				float newvalue = (float)rangedFloat_Zoom->Value;
				newvalue--;
				if (newvalue < rangedFloat_Zoom->Minimum) 
					newvalue = (float)rangedFloat_Zoom->Minimum;
				rangedFloat_Zoom->Value = newvalue;
				center_view();
			}
	public: void GetSelectedDrivers(std::set<tmlnDriver*> &o_Drivers, bool i_bSkipLocked)
		  {
				for (int i=0; i<this->channelList->Count; i++)
				{
					ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
					ChannelClipList ^sel_clips = channel->SelectedClips;
					const int num_clips = sel_clips->Count;

					if (num_clips > 0)
					{
						//	don't allow this interaction if the channel is locked
						if (!i_bSkipLocked || !channel->Locked)
						{
							for (int i=0; i<num_clips; ++i)
							{
								// Cast clip to our driver clip type in order
								// to get the tmlnDriver
								chnlDriverClip^ clip = dynamic_cast<chnlDriverClip^>(sel_clips[i]);
								if (clip)
								{
									//System::Windows::Forms::MessageBox::Show(clip->Name);
									tmlnDriver* driver = &(clip->Driver());
									o_Drivers.insert(driver);
								}
							}
						}
					}
				}
		  }
	private: void CurrentTimeChanged()
			 {
				float time = (float)this->timeSlider1->CurTime;
				float origtime = time;

				tmlnTimeUtil::AdjustTimeToFrame( time );
				//if (time != origtime)

				tmlnTimeLine::SetValue(time);

				//	update the Time GuideLine
				TimeGuideLineMgr::GetGuideLine(m_GuideLineIndexForTime)->Time = time;
				this->panel_channels->Invalidate();

				UpdateTimeLabel();
			 }
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
	protected: ~chnlTraxEditor()
		{
			// clear instance
			if (chnlTraxEditor::FormInstance == this)
				chnlTraxEditor::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}

	protected: virtual bool ProcessCmdKey(Message% msg, Keys keyData) override
	{
		bool bProcessed = tmaMessaging::ProcessCmdKey(msg, keyData);

		return (/*bProcessed ||*/ (__super::ProcessCmdKey(msg,keyData)));
	}


	private:
	//--------------------------------------------------------------------------
		//	create a button image with exception handling
		//--------------------------------------------------------------------------
	private: void create_button_image( System::Windows::Forms::Button^ i_pButton, std::string& i_Path )
		{
			try
			{
				tmaManagedControlUtil::Create_Button_Image( i_pButton, gcnew System::String(i_Path.c_str()) );
			}
			catch (System::IO::FileNotFoundException^ /*ex*/)
			{
				//std::string exfn;
				//tmaManagedStringUtils::ManagedStringToStdString( ex->FileName, exfn );
				DBG_ERROR1("Cannot find file (%s)", i_Path.c_str());
			
				//	Message Box shows up behind the loading dialog
				//guiMessageBox::Show(icon_filename, "File Missing Error");
			
				assert(false);
			}
		}
	private: void SetUpComponents()
		{
			this->buttonDelete->Enabled = false;

			std::string artdir, artfile;
			fsFileUtil::LocatorToANSIFilename( gfPaths::GetPath(mnmPaths::e_ExeArt), artdir );
			artfile = artdir;
			artfile += "\\timeline-tickRev.png";
			create_button_image(buttonTickRev, artfile);
			artfile = artdir;
			artfile += "\\timeline-tickFwd.png";
			create_button_image( buttonTickFwd, artfile );

			this->timeSlider1->TimeChanged += gcnew System::EventHandler(this, &chnlTraxEditor::TimeChanged );
			this->SetTimeScale( (float)this->rangedFloat_Zoom->Value );
			
			//	set the trax editor to the current timeline max
			SetTotalTime(tmlnTimeLine::GetMaximum());

			// Dialog memory remembers size, location, visiblity of dialog 
			m_pMemory = gcnew tmaDialogMemory( this );

			//
			//	Guidelines
			//
			m_GuideLineIndexForTime = TimeGuideLineMgr::CreateGuideLine();
			TimeGuideLine^ pGuide_line;
			pGuide_line = TimeGuideLineMgr::GetGuideLine(m_GuideLineIndexForTime);
			this->panel_channels->Controls->Add( pGuide_line );
			pGuide_line->TimeScale = this->timeSlider1->TimeScale;
			pGuide_line->Color = System::Drawing::Color::Red;
			pGuide_line->Height = this->panel_channels->Height;
			pGuide_line->Anchor = ((System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left))); 
			pGuide_line->BringToFront();

			m_GuideLineIndexForMarker = TimeGuideLineMgr::CreateGuideLine();
			pGuide_line = TimeGuideLineMgr::GetGuideLine(m_GuideLineIndexForMarker);
			this->panel_channels->Controls->Add( pGuide_line );
			pGuide_line->TimeScale = this->timeSlider1->TimeScale;
			pGuide_line->Color = System::Drawing::Color::Green;
			pGuide_line->Height = this->panel_channels->Height;
			pGuide_line->Anchor = ((System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left))); 
			pGuide_line->BringToFront();
			pGuide_line->Visible = false;

			//	Markers
//			markerBar1->MarkersChanged += gcnew System::EventHandler(this, &chnlTraxEditor::markerBar1_MarkersChanged);
			markerBar1->MarkerShowPropertiesRequest += gcnew MarkerDisplayIcon::MarkerEventHandler( this, &chnlTraxEditor::markerBar1_MarkersShowProperties);
			markerBar1->NoteShowPropertiesRequest += gcnew NoteDisplayIcon::NoteEventHandler( this, &chnlTraxEditor::markerBar1_NotesShowProperties);

			// label
			UpdateTimeLabel();
		}

	private: void SystemObjectDisplay()
		{
			//	title bar text
			//System::String^ tbtext = gcnew System::String( "System Objects" );
			//set_titlebar( tbtext );

			//
			this->FreeChannels();

			std::vector<tmlnScriptObject*> objects;

			if (tmlnSelectionUtil::GetSelectedScriptObject() != 0)
			{
				tmlnTimelineMgr::GetObjects( objects, tmlnTimelineMgr::GetObjectCategory(tmlnSelectionUtil::GetSelectedScriptObject()) );
			}

			for (int i=0; i < objects.size(); i++)
			{
				AddChannelsForObject(objects[i]);
			}
			this->labelNoChannels->Visible = (objects.empty());

			chnlTimeTickMgr::GatherTimeTicks(objects);
			this->UpdateTimeTicks();
		}

	private: void AllObjectDisplay()
		{
			//	title bar text
			//System::String^ tbtext = gcnew System::String( "All Objects" );
			//set_titlebar( tbtext );

			//
			this->FreeChannels();

			std::vector<tmlnScriptObject*> objects;
			tmlnTimelineMgr::GetObjects( objects );
			for (int i=0; i<objects.size(); i++)
			{
				AddChannelsForObject(objects[i]);
			}
			this->labelNoChannels->Visible = (objects.empty());

			chnlTimeTickMgr::GatherTimeTicks(objects);
			this->UpdateTimeTicks();
		}

	private: void MultipleSelectionDisplay()
		{
			//	title bar text
			//System::String^ tbtext = gcnew System::String( "Selected Objects" );
			//set_titlebar( tbtext );

			//
			this->FreeChannels();

			std::vector<tmlnScriptObject*> objects;
			tmlnSelectionUtil::GetSelectedScriptObjects(objects);

			for (int i=0; i < objects.size(); i++)
			{
				AddChannelsForObject(objects[i]);
			}
			this->labelNoChannels->Visible = (objects.empty());

			chnlTimeTickMgr::GatherTimeTicks(objects);
			this->UpdateTimeTicks();
		}


	private: void SingleObjectDisplay(tmlnScriptObject* i_pObject)
		{
			//	title bar text
			//
			//if (i_pObject == NULL)
			//{
			//	System::String^ tbtext = gcnew System::String( "Trax Editor" ); 
			//	set_titlebar( tbtext );
			//}
			//else
			//{
			//	System::String^ tbtext = System::String::Format( "Object: {0}", GetObjectName(i_pObject) ); 
			//	set_titlebar( tbtext );
			//}

			//
			this->FreeChannels();

			AddChannelsForObject(i_pObject);

			this->labelNoChannels->Visible = (i_pObject == NULL);

			chnlTimeTickMgr::GatherTimeTicks(i_pObject);
			this->UpdateTimeTicks();
		}
 
	private: System::Windows::Forms::Label ^  labelNoChannels;
	private: System::Windows::Forms::Button ^  buttonAdd;
	private: TimelineControls::TimeLabel ^  timeLabel1;
	private: TimelineControls::TimeSlider ^  timeSlider1;
	private: System::Windows::Forms::Button ^  buttonDelete;
	private: System::Windows::Forms::Button ^  buttonTickFwd;
	private: System::Windows::Forms::Button ^  buttonTickRev;
	private: System::Windows::Forms::CheckBox ^  checkSnap;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_Zoom;
	private: System::Windows::Forms::Panel ^  panel_timeline;
	private: System::Windows::Forms::Panel ^  panel_channels;
	private: System::Windows::Forms::Label ^  label_time;
	private: System::Windows::Forms::Label ^  label_time_frames;
	private: System::Windows::Forms::ComboBox ^  comboBox_filter;
	private: System::Windows::Forms::Button ^  button_paste;
	private: System::Windows::Forms::Button ^  button_copy;
	private: System::Windows::Forms::Panel ^  panel_names;
	private: System::Windows::Forms::Panel ^  panel_strip;
	private: TimelineControls::MarkerDisplayBar ^ markerBar1;
	private: System::Windows::Forms::Button ^  button_split;
	private: System::Collections::ArrayList ^ m_ChannelCount; // number of channels per object
	private: System::Collections::ArrayList ^ channelList;
	private: System::Collections::ArrayList ^ channelLockList;
	private: System::Collections::ArrayList ^ labelList;
	private: System::Collections::ArrayList ^ labelObjectList;
	private: System::Windows::Forms::Button ^  button_zoomtofit;

	private: System::Windows::Forms::Button ^  button_movedrivers;
	private: System::Windows::Forms::Button^  button_select_channel_later;
	private: System::Windows::Forms::CheckBox^  checkBox_conflicts;

	private: System::Windows::Forms::Label^  label_markers;
	private: System::Windows::Forms::Button ^  button_addmarker;
	private: System::Windows::Forms::Button ^  button_delmarker;
	private: System::Windows::Forms::Button^  button_prevmarker;
	private: System::Windows::Forms::Button^  button_nextmarker;

 	private: System::Windows::Forms::Button^  button_nextnote;
	private: System::Windows::Forms::Button^  button_prevnote;
	private: System::Windows::Forms::Label^  label_Notes;
	private: System::Windows::Forms::Button^  button_delnote;
	private: System::Windows::Forms::Button^  button_addnote;

	private: tmaDialogMemory^ m_pMemory;
	private: int m_SnapInterval;
	private: int m_nFilterState;				// 0=multiple selected, 1=selected, 2=system, 3=all
	//private: ArrayList^ m_ChannelClipboard;		// for copy + paste of clips
	//private: ArrayList^ m_DriverClipboard;		// for copy + paste of driver clips
	private: int m_GuideLineIndexForTime;
	private: int m_GuideLineIndexForMarker;
	private: bool m_bDriverSelectStart;
	private: int m_DriverSelectStartX;
	private: int m_DriverSelectStartY;
	private: float m_DriverSelectStartTime;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
	private: void InitializeComponent(void)
		{
			this->panel_channels = (gcnew System::Windows::Forms::Panel());
			this->checkBox_conflicts = (gcnew System::Windows::Forms::CheckBox());
			this->labelNoChannels = (gcnew System::Windows::Forms::Label());
			this->timeLabel1 = (gcnew TimelineControls::TimeLabel());
			this->timeSlider1 = (gcnew TimelineControls::TimeSlider());
			this->panel_timeline = (gcnew System::Windows::Forms::Panel());
			this->markerBar1 = (gcnew TimelineControls::MarkerDisplayBar());
			this->buttonDelete = (gcnew System::Windows::Forms::Button());
			this->buttonAdd = (gcnew System::Windows::Forms::Button());
			this->buttonTickFwd = (gcnew System::Windows::Forms::Button());
			this->buttonTickRev = (gcnew System::Windows::Forms::Button());
			this->checkSnap = (gcnew System::Windows::Forms::CheckBox());
			this->rangedFloat_Zoom = (gcnew TerawattManagedControls::RangedFloat());
			this->label_time = (gcnew System::Windows::Forms::Label());
			this->comboBox_filter = (gcnew System::Windows::Forms::ComboBox());
			this->button_paste = (gcnew System::Windows::Forms::Button());
			this->button_copy = (gcnew System::Windows::Forms::Button());
			this->panel_names = (gcnew System::Windows::Forms::Panel());
			this->button_addmarker = (gcnew System::Windows::Forms::Button());
			this->panel_strip = (gcnew System::Windows::Forms::Panel());
			this->button_delmarker = (gcnew System::Windows::Forms::Button());
			this->button_split = (gcnew System::Windows::Forms::Button());
			this->button_zoomtofit = (gcnew System::Windows::Forms::Button());
			this->label_time_frames = (gcnew System::Windows::Forms::Label());
			this->button_movedrivers = (gcnew System::Windows::Forms::Button());
			this->button_select_channel_later = (gcnew System::Windows::Forms::Button());
			this->label_markers = (gcnew System::Windows::Forms::Label());
			this->button_prevmarker = (gcnew System::Windows::Forms::Button());
			this->button_nextmarker = (gcnew System::Windows::Forms::Button());
			this->button_nextnote = (gcnew System::Windows::Forms::Button());
			this->button_prevnote = (gcnew System::Windows::Forms::Button());
			this->label_Notes = (gcnew System::Windows::Forms::Label());
			this->button_delnote = (gcnew System::Windows::Forms::Button());
			this->button_addnote = (gcnew System::Windows::Forms::Button());
			this->panel_channels->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->timeLabel1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->timeSlider1))->BeginInit();
			this->panel_timeline->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->markerBar1))->BeginInit();
			this->SuspendLayout();
			// 
			// panel_channels
			// 
			this->panel_channels->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel_channels->AutoScroll = true;
			this->panel_channels->AutoScrollMinSize = System::Drawing::Size(600, 0);
			this->panel_channels->BackColor = System::Drawing::SystemColors::ControlDark;
			this->panel_channels->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel_channels->Controls->Add(this->checkBox_conflicts);
			this->panel_channels->Controls->Add(this->labelNoChannels);
			this->panel_channels->Location = System::Drawing::Point(234, 88);
			this->panel_channels->Name = L"panel_channels";
			this->panel_channels->Size = System::Drawing::Size(581, 408);
			this->panel_channels->TabIndex = 0;
			this->panel_channels->UseWaitCursor = true;
			this->panel_channels->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &chnlTraxEditor::panel_channels_Paint);
			this->panel_channels->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &chnlTraxEditor::ChannelPanel_MouseDown);
			this->panel_channels->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &chnlTraxEditor::ChannelPanel_MouseUp);
			this->panel_channels->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &chnlTraxEditor::ChannelPanel_MouseMove);
			// 
			// checkBox_conflicts
			// 
			this->checkBox_conflicts->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox_conflicts->AutoSize = true;
			this->checkBox_conflicts->Location = System::Drawing::Point(3, 3);
			this->checkBox_conflicts->Name = L"checkBox_conflicts";
			this->checkBox_conflicts->Size = System::Drawing::Size(65, 23);
			this->checkBox_conflicts->TabIndex = 21;
			this->checkBox_conflicts->Text = L"0 conflicts";
			this->checkBox_conflicts->UseVisualStyleBackColor = true;
			this->checkBox_conflicts->UseWaitCursor = true;
			this->checkBox_conflicts->Visible = false;
			this->checkBox_conflicts->CheckedChanged += gcnew System::EventHandler(this, &chnlTraxEditor::checkBox_conflicts_CheckedChanged);
			// 
			// labelNoChannels
			// 
			this->labelNoChannels->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->labelNoChannels->Location = System::Drawing::Point(40, 40);
			this->labelNoChannels->Name = L"labelNoChannels";
			this->labelNoChannels->Size = System::Drawing::Size(96, 28);
			this->labelNoChannels->TabIndex = 0;
			this->labelNoChannels->Text = L"No channels";
			this->labelNoChannels->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->labelNoChannels->UseWaitCursor = true;
			// 
			// timeLabel1
			// 
			this->timeLabel1->BackColor = System::Drawing::SystemColors::ControlLight;
			this->timeLabel1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->timeLabel1->Location = System::Drawing::Point(8, 0);
			this->timeLabel1->Name = L"timeLabel1";
			this->timeLabel1->Size = System::Drawing::Size(4832, 32);
			this->timeLabel1->TabIndex = 2;
			this->timeLabel1->TabStop = false;
			this->timeLabel1->TimeScale = 80;
			this->timeLabel1->UseWaitCursor = true;
			// 
			// timeSlider1
			// 
			this->timeSlider1->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->timeSlider1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->timeSlider1->CurTime = 0;
			this->timeSlider1->Location = System::Drawing::Point(8, 38);
			this->timeSlider1->Name = L"timeSlider1";
			this->timeSlider1->Size = System::Drawing::Size(4832, 16);
			this->timeSlider1->TabIndex = 3;
			this->timeSlider1->TabStop = false;
			this->timeSlider1->TimeScale = 80;
			this->timeSlider1->UseWaitCursor = true;
			this->timeSlider1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &chnlTraxEditor::timeSlider1_MouseDown);
			this->timeSlider1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &chnlTraxEditor::timeSlider1_MouseUp);
			// 
			// panel_timeline
			// 
			this->panel_timeline->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel_timeline->AutoScroll = true;
			this->panel_timeline->BackColor = System::Drawing::SystemColors::ControlLight;
			this->panel_timeline->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel_timeline->Controls->Add(this->timeLabel1);
			this->panel_timeline->Controls->Add(this->timeSlider1);
			this->panel_timeline->Controls->Add(this->markerBar1);
			this->panel_timeline->Location = System::Drawing::Point(233, 0);
			this->panel_timeline->Name = L"panel_timeline";
			this->panel_timeline->Size = System::Drawing::Size(582, 100);
			this->panel_timeline->TabIndex = 4;
			this->panel_timeline->UseWaitCursor = true;
			// 
			// markerBar1
			// 
			this->markerBar1->BackColor = System::Drawing::SystemColors::ControlLight;
			this->markerBar1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->markerBar1->CurTime = 0;
			this->markerBar1->Location = System::Drawing::Point(8, 59);
			this->markerBar1->Name = L"markerBar1";
			this->markerBar1->Size = System::Drawing::Size(4832, 16);
			this->markerBar1->TabIndex = 4;
			this->markerBar1->TabStop = false;
			this->markerBar1->TimeScale = 80;
			this->markerBar1->UseWaitCursor = true;
			// 
			// buttonDelete
			// 
			this->buttonDelete->Location = System::Drawing::Point(8, 130);
			this->buttonDelete->Name = L"buttonDelete";
			this->buttonDelete->Size = System::Drawing::Size(48, 21);
			this->buttonDelete->TabIndex = 3;
			this->buttonDelete->TabStop = false;
			this->buttonDelete->Text = L"Delete";
			this->buttonDelete->UseWaitCursor = true;
			this->buttonDelete->Click += gcnew System::EventHandler(this, &chnlTraxEditor::buttonDelete_Click);
			// 
			// buttonAdd
			// 
			this->buttonAdd->Location = System::Drawing::Point(8, 109);
			this->buttonAdd->Name = L"buttonAdd";
			this->buttonAdd->Size = System::Drawing::Size(48, 21);
			this->buttonAdd->TabIndex = 1;
			this->buttonAdd->TabStop = false;
			this->buttonAdd->Text = L"Add";
			this->buttonAdd->UseWaitCursor = true;
			this->buttonAdd->Click += gcnew System::EventHandler(this, &chnlTraxEditor::buttonAdd_Click);
			// 
			// buttonTickFwd
			// 
			this->buttonTickFwd->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->buttonTickFwd->Location = System::Drawing::Point(34, 81);
			this->buttonTickFwd->Name = L"buttonTickFwd";
			this->buttonTickFwd->Size = System::Drawing::Size(20, 21);
			this->buttonTickFwd->TabIndex = 4;
			this->buttonTickFwd->TabStop = false;
			this->buttonTickFwd->UseWaitCursor = true;
			this->buttonTickFwd->Click += gcnew System::EventHandler(this, &chnlTraxEditor::buttonTickFwd_Click);
			// 
			// buttonTickRev
			// 
			this->buttonTickRev->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->buttonTickRev->Location = System::Drawing::Point(10, 81);
			this->buttonTickRev->Name = L"buttonTickRev";
			this->buttonTickRev->Size = System::Drawing::Size(20, 21);
			this->buttonTickRev->TabIndex = 5;
			this->buttonTickRev->TabStop = false;
			this->buttonTickRev->UseWaitCursor = true;
			this->buttonTickRev->Click += gcnew System::EventHandler(this, &chnlTraxEditor::buttonTickRev_Click);
			// 
			// checkSnap
			// 
			this->checkSnap->Location = System::Drawing::Point(10, 56);
			this->checkSnap->Name = L"checkSnap";
			this->checkSnap->Size = System::Drawing::Size(54, 21);
			this->checkSnap->TabIndex = 6;
			this->checkSnap->TabStop = false;
			this->checkSnap->Text = L"Snap";
			this->checkSnap->UseWaitCursor = true;
			this->checkSnap->CheckedChanged += gcnew System::EventHandler(this, &chnlTraxEditor::checkSnap_CheckedChanged);
			// 
			// rangedFloat_Zoom
			// 
			this->rangedFloat_Zoom->Exponent = static_cast<System::Int16>(2);
			this->rangedFloat_Zoom->Location = System::Drawing::Point(0, 3);
			this->rangedFloat_Zoom->Maximum = 500;
			this->rangedFloat_Zoom->Name = L"rangedFloat_Zoom";
			this->rangedFloat_Zoom->NumTicks = static_cast<System::Int16>(100);
			this->rangedFloat_Zoom->Precision = static_cast<System::Int16>(2);
			this->rangedFloat_Zoom->ShowValue = false;
			this->rangedFloat_Zoom->Size = System::Drawing::Size(89, 24);
			this->rangedFloat_Zoom->TabIndex = 8;
			this->rangedFloat_Zoom->TabStop = false;
			this->rangedFloat_Zoom->UseWaitCursor = true;
			this->rangedFloat_Zoom->Value = 80;
			this->rangedFloat_Zoom->ValueChanged += gcnew System::EventHandler(this, &chnlTraxEditor::rangedFloat_Zoom_ValueChanged);
			// 
			// label_time
			// 
			this->label_time->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label_time->Location = System::Drawing::Point(4, 5);
			this->label_time->Name = L"label_time";
			this->label_time->Size = System::Drawing::Size(94, 15);
			this->label_time->TabIndex = 10;
			this->label_time->UseWaitCursor = true;
			this->label_time->Visible = false;
			// 
			// comboBox_filter
			// 
			this->comboBox_filter->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"Multiple Selected", L"Selected", L"System", 
				L"All"});
			this->comboBox_filter->Location = System::Drawing::Point(120, 6);
			this->comboBox_filter->MaxDropDownItems = 4;
			this->comboBox_filter->Name = L"comboBox_filter";
			this->comboBox_filter->Size = System::Drawing::Size(109, 21);
			this->comboBox_filter->TabIndex = 11;
			this->comboBox_filter->TabStop = false;
			this->comboBox_filter->Text = L"Multiple Selected";
			this->comboBox_filter->UseWaitCursor = true;
			this->comboBox_filter->SelectedIndexChanged += gcnew System::EventHandler(this, &chnlTraxEditor::comboBox_filter_SelectedIndexChanged);
			// 
			// button_paste
			// 
			this->button_paste->Enabled = false;
			this->button_paste->Location = System::Drawing::Point(8, 172);
			this->button_paste->Name = L"button_paste";
			this->button_paste->Size = System::Drawing::Size(48, 21);
			this->button_paste->TabIndex = 12;
			this->button_paste->TabStop = false;
			this->button_paste->Text = L"Paste";
			this->button_paste->UseWaitCursor = true;
			this->button_paste->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_paste_Click);
			// 
			// button_copy
			// 
			this->button_copy->Location = System::Drawing::Point(8, 151);
			this->button_copy->Name = L"button_copy";
			this->button_copy->Size = System::Drawing::Size(48, 21);
			this->button_copy->TabIndex = 12;
			this->button_copy->TabStop = false;
			this->button_copy->Text = L"Copy";
			this->button_copy->UseWaitCursor = true;
			this->button_copy->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_copy_Click);
			// 
			// panel_names
			// 
			this->panel_names->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->panel_names->AutoScroll = true;
			this->panel_names->BackColor = System::Drawing::SystemColors::ControlLight;
			this->panel_names->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel_names->Location = System::Drawing::Point(64, 88);
			this->panel_names->Name = L"panel_names";
			this->panel_names->Size = System::Drawing::Size(187, 392);
			this->panel_names->TabIndex = 13;
			this->panel_names->UseWaitCursor = true;
			// 
			// button_addmarker
			// 
			this->button_addmarker->Location = System::Drawing::Point(168, 61);
			this->button_addmarker->Name = L"button_addmarker";
			this->button_addmarker->Size = System::Drawing::Size(16, 16);
			this->button_addmarker->TabIndex = 14;
			this->button_addmarker->TabStop = false;
			this->button_addmarker->Text = L"+";
			this->button_addmarker->UseWaitCursor = true;
			this->button_addmarker->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_addmarker_Click);
			// 
			// panel_strip
			// 
			this->panel_strip->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel_strip->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->panel_strip->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel_strip->Location = System::Drawing::Point(64, 80);
			this->panel_strip->Name = L"panel_strip";
			this->panel_strip->Size = System::Drawing::Size(875, 8);
			this->panel_strip->TabIndex = 15;
			this->panel_strip->UseWaitCursor = true;
			// 
			// button_delmarker
			// 
			this->button_delmarker->Location = System::Drawing::Point(152, 61);
			this->button_delmarker->Name = L"button_delmarker";
			this->button_delmarker->Size = System::Drawing::Size(16, 16);
			this->button_delmarker->TabIndex = 16;
			this->button_delmarker->TabStop = false;
			this->button_delmarker->Text = L"-";
			this->button_delmarker->UseWaitCursor = true;
			this->button_delmarker->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_delmarker_Click);
			// 
			// button_split
			// 
			this->button_split->Location = System::Drawing::Point(8, 193);
			this->button_split->Name = L"button_split";
			this->button_split->Size = System::Drawing::Size(48, 21);
			this->button_split->TabIndex = 17;
			this->button_split->TabStop = false;
			this->button_split->Text = L"Split";
			this->button_split->UseWaitCursor = true;
			this->button_split->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_split_Click);
			// 
			// button_zoomtofit
			// 
			this->button_zoomtofit->Location = System::Drawing::Point(83, 6);
			this->button_zoomtofit->Name = L"button_zoomtofit";
			this->button_zoomtofit->Size = System::Drawing::Size(16, 16);
			this->button_zoomtofit->TabIndex = 18;
			this->button_zoomtofit->TabStop = false;
			this->button_zoomtofit->Text = L"z";
			this->button_zoomtofit->UseWaitCursor = true;
			this->button_zoomtofit->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_zoomtofit_Click);
			// 
			// label_time_frames
			// 
			this->label_time_frames->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label_time_frames->Location = System::Drawing::Point(105, 5);
			this->label_time_frames->Name = L"label_time_frames";
			this->label_time_frames->Size = System::Drawing::Size(51, 16);
			this->label_time_frames->TabIndex = 19;
			this->label_time_frames->UseWaitCursor = true;
			this->label_time_frames->Visible = false;
			// 
			// button_movedrivers
			// 
			this->button_movedrivers->Location = System::Drawing::Point(8, 214);
			this->button_movedrivers->Name = L"button_movedrivers";
			this->button_movedrivers->Size = System::Drawing::Size(48, 21);
			this->button_movedrivers->TabIndex = 20;
			this->button_movedrivers->TabStop = false;
			this->button_movedrivers->Text = L"Move";
			this->button_movedrivers->UseWaitCursor = true;
			this->button_movedrivers->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_movedrivers_Click);
			// 
			// button_select_channel_later
			// 
			this->button_select_channel_later->Location = System::Drawing::Point(8, 235);
			this->button_select_channel_later->Name = L"button_select_channel_later";
			this->button_select_channel_later->Size = System::Drawing::Size(48, 21);
			this->button_select_channel_later->TabIndex = 21;
			this->button_select_channel_later->TabStop = false;
			this->button_select_channel_later->Text = L"Sel->";
			this->button_select_channel_later->UseWaitCursor = true;
			this->button_select_channel_later->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_select_later_drivers_Click);
			// 
			// label_markers
			// 
			this->label_markers->AutoSize = true;
			this->label_markers->Location = System::Drawing::Point(95, 61);
			this->label_markers->Name = L"label_markers";
			this->label_markers->Size = System::Drawing::Size(45, 13);
			this->label_markers->TabIndex = 0;
			this->label_markers->Text = L"Markers";
			this->label_markers->UseWaitCursor = true;
			// 
			// button_prevmarker
			// 
			this->button_prevmarker->Location = System::Drawing::Point(190, 61);
			this->button_prevmarker->Name = L"button_prevmarker";
			this->button_prevmarker->Size = System::Drawing::Size(16, 16);
			this->button_prevmarker->TabIndex = 22;
			this->button_prevmarker->TabStop = false;
			this->button_prevmarker->Text = L"<";
			this->button_prevmarker->UseWaitCursor = true;
			this->button_prevmarker->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_prevmarker_Click);
			// 
			// button_nextmarker
			// 
			this->button_nextmarker->Location = System::Drawing::Point(206, 61);
			this->button_nextmarker->Name = L"button_nextmarker";
			this->button_nextmarker->Size = System::Drawing::Size(16, 16);
			this->button_nextmarker->TabIndex = 23;
			this->button_nextmarker->TabStop = false;
			this->button_nextmarker->Text = L">";
			this->button_nextmarker->UseWaitCursor = true;
			this->button_nextmarker->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_nextmarker_Click);
			// 
			// button_nextnote
			// 
			this->button_nextnote->Location = System::Drawing::Point(206, 43);
			this->button_nextnote->Name = L"button_nextnote";
			this->button_nextnote->Size = System::Drawing::Size(16, 16);
			this->button_nextnote->TabIndex = 28;
			this->button_nextnote->TabStop = false;
			this->button_nextnote->Text = L">";
			this->button_nextnote->UseWaitCursor = true;
			this->button_nextnote->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_nextnote_Click);
			// 
			// button_prevnote
			// 
			this->button_prevnote->Location = System::Drawing::Point(190, 43);
			this->button_prevnote->Name = L"button_prevnote";
			this->button_prevnote->Size = System::Drawing::Size(16, 16);
			this->button_prevnote->TabIndex = 27;
			this->button_prevnote->TabStop = false;
			this->button_prevnote->Text = L"<";
			this->button_prevnote->UseWaitCursor = true;
			this->button_prevnote->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_prevnote_Click);
			// 
			// label_Notes
			// 
			this->label_Notes->AutoSize = true;
			this->label_Notes->Location = System::Drawing::Point(95, 43);
			this->label_Notes->Name = L"label_Notes";
			this->label_Notes->Size = System::Drawing::Size(35, 13);
			this->label_Notes->TabIndex = 24;
			this->label_Notes->Text = L"Notes";
			this->label_Notes->UseWaitCursor = true;
			// 
			// button_delnote
			// 
			this->button_delnote->Location = System::Drawing::Point(152, 43);
			this->button_delnote->Name = L"button_delnote";
			this->button_delnote->Size = System::Drawing::Size(16, 16);
			this->button_delnote->TabIndex = 26;
			this->button_delnote->Text = L"-";
			this->button_delnote->UseWaitCursor = true;
			this->button_delnote->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_delnote_Click);
			// 
			// button_addnote
			// 
			this->button_addnote->Location = System::Drawing::Point(168, 43);
			this->button_addnote->Name = L"button_addnote";
			this->button_addnote->Size = System::Drawing::Size(16, 16);
			this->button_addnote->TabIndex = 25;
			this->button_addnote->TabStop = false;
			this->button_addnote->Text = L"+";
			this->button_addnote->UseWaitCursor = true;
			this->button_addnote->Click += gcnew System::EventHandler(this, &chnlTraxEditor::button_addnote_Click);
			// 
			// chnlTraxEditor
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(827, 494);
			this->Controls->Add(this->button_nextnote);
			this->Controls->Add(this->button_prevnote);
			this->Controls->Add(this->label_Notes);
			this->Controls->Add(this->button_delnote);
			this->Controls->Add(this->button_addnote);
			this->Controls->Add(this->button_nextmarker);
			this->Controls->Add(this->button_prevmarker);
			this->Controls->Add(this->label_markers);
			this->Controls->Add(this->button_select_channel_later);
			this->Controls->Add(this->button_movedrivers);
			this->Controls->Add(this->button_zoomtofit);
			this->Controls->Add(this->button_split);
			this->Controls->Add(this->button_delmarker);
			this->Controls->Add(this->panel_strip);
			this->Controls->Add(this->panel_channels);
			this->Controls->Add(this->panel_names);
			this->Controls->Add(this->panel_timeline);
			this->Controls->Add(this->button_addmarker);
			this->Controls->Add(this->button_paste);
			this->Controls->Add(this->comboBox_filter);
			this->Controls->Add(this->rangedFloat_Zoom);
			this->Controls->Add(this->buttonTickRev);
			this->Controls->Add(this->buttonTickFwd);
			this->Controls->Add(this->buttonAdd);
			this->Controls->Add(this->buttonDelete);
			this->Controls->Add(this->label_time);
			this->Controls->Add(this->button_copy);
			this->Controls->Add(this->checkSnap);
			this->Controls->Add(this->label_time_frames);
			this->KeyPreview = true;
			this->Name = L"chnlTraxEditor";
			this->ShowInTaskbar = false;
			this->Text = L"Channel Editor";
			this->UseWaitCursor = true;
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &chnlTraxEditor::chnlTraxEditor_KeyDown);
			this->panel_channels->ResumeLayout(false);
			this->panel_channels->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->timeLabel1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->timeSlider1))->EndInit();
			this->panel_timeline->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->markerBar1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

			 }		

		//

	private: void AddChannelsForObject(tmlnScriptObject* i_pObject)
		{
			int slot_num = this->channelList->Count + labelObjectList->Count;
			if (i_pObject)
			{
				// Create label for object
				System::Windows::Forms::Label ^nameLabel = gcnew System::Windows::Forms::Label();
				nameLabel->Text = GetObjectName(i_pObject); 
				nameLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
				nameLabel->BackColor = System::Drawing::SystemColors::ActiveCaption;
				nameLabel->ForeColor = System::Drawing::SystemColors::ActiveCaptionText;
				nameLabel->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
				nameLabel->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
				//int offset = this->panel_channels->AutoScrollPosition.Y;
				nameLabel->Location = System::Drawing::Point(c_NamePanelOffsetX /*+ offset*/, c_NamePanelOffsetY + (c_ChannelHeight * slot_num));
				nameLabel->Size = System::Drawing::Size(c_NameLabelWidth, c_ChannelHeight);
				this->panel_names->Controls->Add(nameLabel);
				labelObjectList->Add(nameLabel);
				slot_num++;

				// Add Channels for object that have a driver ONLY
				//
				tmlnChannelSet& chnl_set = i_pObject->ChannelSet();
				int num_channels = chnl_set.GetNumChannels();
				int chnl_count = 0;
				for (int i=0; i<num_channels; i++)
				{
					tmlnChannel& channel = chnl_set.Channel(i);

					//	if drivers for channel, then add it.
					//
					if ( channel.GetNumDrivers() > 0 )
					{
						ChannelControl ^pChannelCtrl = 
							this->CreateChannel(channel.GetName().c_str(), chnl_count+slot_num, channel.IsLocked() );
						pChannelCtrl->MultiChannel = channel.IsMultiChannel();
						
						for (int c=0; c<channel.GetNumDrivers(); c++)
						{
							tmlnDriver& driver = channel.Driver(c);

							chnlDriverClip ^clip = gcnew chnlDriverClip(driver,
																		gcnew System::String(driver.GetName().c_str()),
																		driver.GetBeginTime(), driver.GetEndTime());
							clip->Category = gcnew System::String(driver.GetCategory().c_str());

							maFloatRGBA color;
							color = driver.GetClipFillColor();
							clip->SetClipFillColor( color.GetRed(), color.GetGreen(), color.GetBlue() );

							pChannelCtrl->AddClip( clip );

							chnlOperations::MonitorChanges(driver);
						}

						chnl_count++;
					}
				}
				m_ChannelCount->Add(chnl_count);
			}
		}

	private: System::String^ GetObjectName(tmlnScriptObject* i_pObject)
		{
			return gcnew System::String( i_pObject->GetTmlnName().c_str() );
		}

	private: ChannelControl^ CreateChannel(const char *i_Name, int i_Index, bool i_bLocked)
		{
			int offset = this->panel_channels->AutoScrollPosition.X;

			//	create lock checkbox for each channel
			System::Windows::Forms::CheckBox^ checkBox = gcnew System::Windows::Forms::CheckBox();

			//	location "+ 84" is to put the checkbox at the end of the panel.
			//	location "+ 6" is to drop down the checkbox to line up with the text
			checkBox->Location = System::Drawing::Point(c_ChannelOffsetX + c_LabelWidth - c_ChannelLockedCheckbox_OffsetX, c_ChannelLockedCheckbox_OffsetY + c_NamePanelOffsetY + c_ChannelHeight * i_Index);
			checkBox->Name = System::String::Format("checkBox{0}", i_Index);
			checkBox->Size = System::Drawing::Size(16, 16);
			checkBox->TabIndex = 0;
			checkBox->CheckedChanged += gcnew System::EventHandler(this, &chnlTraxEditor::checkBox_CheckedChanged);
			checkBox->Checked = i_bLocked;
			this->panel_names->Controls->Add(checkBox);
			this->channelLockList->Add(checkBox);

			//	create the channel label
			System::Windows::Forms::Label ^channelLabel = gcnew System::Windows::Forms::Label();
			channelLabel->Name = System::String::Format("channelLabel{0}", i_Index);
			channelLabel->Text = gcnew System::String(i_Name);
			channelLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			channelLabel->Location = System::Drawing::Point(8 , c_NamePanelOffsetY + c_ChannelHeight * i_Index);
			channelLabel->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			channelLabel->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			channelLabel->Size = System::Drawing::Size(c_LabelWidth, c_ChannelHeight);

			this->panel_names->Controls->Add(channelLabel);
			this->labelList->Add(channelLabel);

			// create the channel control
			ChannelControl^ channelControl = gcnew ChannelControl();
			channelControl->Location = System::Drawing::Point(c_ChannelOffsetX + offset, c_NamePanelOffsetY + c_ChannelHeight * i_Index);
			channelControl->Name = System::String::Format("channelControl{0}", i_Index);
			channelControl->Size = System::Drawing::Size(c_ChannelControl_Width, c_ChannelHeight);
			channelControl->TimeScale		= this->timeLabel1->TimeScale;
			channelControl->TotalTime		= this->timeLabel1->TotalTime;
			channelControl->SnapInterval	= this->m_SnapInterval;
			channelControl->Locked			= i_bLocked;

			channelControl->ClipSelected	+= gcnew ClipSelectedEventHandler(this, &chnlTraxEditor::ClipSelected );
			channelControl->ClipMoved		+= gcnew MouseMoveEventHandler(this, &chnlTraxEditor::ClipMoved );
			channelControl->ClipResized		+= gcnew System::EventHandler(this, &chnlTraxEditor::ClipResized );	// make new function later?
			channelControl->ClipMoveFinished	+= gcnew System::EventHandler(this, &chnlTraxEditor::ClipMoveFinished );
			channelControl->SizeChanged		+= gcnew System::EventHandler(this, &chnlTraxEditor::SizeChanged );
			channelControl->MouseDown		+= gcnew System::Windows::Forms::MouseEventHandler(this, &chnlTraxEditor::ChannelPanel_MouseDown );
			channelControl->MouseUp			+= gcnew System::Windows::Forms::MouseEventHandler(this, &chnlTraxEditor::ChannelPanel_MouseUp );

			this->panel_channels->Controls->Add(channelControl);
			this->channelList->Add(channelControl);
			return channelControl;
		}

	private: void FreeChannels()
		{
			this->buttonDelete->Enabled = false;
			chnlOperations::ClearMonitors();
			for (int i=0; i<this->channelList->Count; i++)
			{	
				this->panel_channels->Controls->Remove(dynamic_cast<ChannelControl^>
					(this->channelList[i]));
			}
			this->channelList->Clear();
			for (int i=0; i<this->channelLockList->Count; i++)
			{	
				this->panel_names->Controls->Remove(dynamic_cast<System::Windows::Forms::CheckBox^>
					(this->channelLockList[i]));
			}
			this->channelLockList->Clear();
			for (int i=0; i<this->labelList->Count; i++)
			{	
				this->panel_names->Controls->Remove(dynamic_cast<System::Windows::Forms::Label^>
					(this->labelList[i]));
			}
			this->labelList->Clear();
			for (int i=0; i<this->labelObjectList->Count; i++)
			{	
				this->panel_names->Controls->Remove(dynamic_cast<System::Windows::Forms::Label^>
					(this->labelObjectList[i]));
			}
			this->labelObjectList->Clear();

			m_ChannelCount->Clear();
		}

	private: System::Void TimeChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			CurrentTimeChanged();
		}

	private: System::Void ClipSelected(System::Object ^  sender, TimelineControls::ClipSelectedEventArgs ^ e)
		{
			ChannelControl^ picked = dynamic_cast<ChannelControl^>(sender);
			if (picked == nullptr)
				return;

			// If we are clearing the selection, or not appending to 
			// the selection, then clear selection from other channels
			if (e->SelectedClip == nullptr || e->Appended == false)
			{
				for (int i=0; i<this->channelList->Count; i++)
				{	
					ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
					if (channel != picked)
						channel->ClearSelection();
				}
			}

			// Now, look for all other occurrences of this driver on other channels
			if (e->SelectedClip != nullptr)
			{
				chnlDriverClip^ sel_clip = dynamic_cast<chnlDriverClip^>(e->SelectedClip);
				if (sel_clip)
				{
					tmlnDriver *pSelDriver = &sel_clip->Driver();
					for (int i=0; i<this->channelList->Count; i++)
					{	
						ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
						if (channel != picked)
						{
							for (int c=0; c<channel->Clips->Count; c++)
							{
								chnlDriverClip^ clip = dynamic_cast<chnlDriverClip^>(channel->Clips[c]);
								if (clip)
								{
									if (&clip->Driver() == pSelDriver)
									{
										const bool append_selection = true;
										channel->SelectClip(c, append_selection);
									}
								}
							}
						}
					}
				}
			}

			// Prepare all selected clips in all channels for the interaction
			for (int i=0; i<this->channelList->Count; i++)
			{	
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
				channel->BeginInteraction();
			}

			// Update the properties dialog for the selected driver
			// (Should this merge all driver properties together like with objects?)
			if (picked->SelectedClip >= 0)
			{
				this->buttonDelete->Enabled = true;

				// Got selected clip
				chnlDriverClip^ clip = dynamic_cast<chnlDriverClip^>(picked->Clips[picked->SelectedClip]);
				if (clip)
				{
					tmlnDriver* pDriver = &(clip->Driver());
					if (pDriver != 0)
					{
						if (tmaDialogTabbedMgr::IsVisible("Driver"))
						{
							pDriver->DoEditProperties();
						}
					}
				}
			}
			else
			{			
				this->buttonDelete->Enabled = false;
			}

		}

	private: System::Void ClipMoveFinished(System::Object ^  sender, System::EventArgs ^  e)
		{
			//	turn off the guidelines
			TimeGuideLineMgr::GetGuideLine(m_GuideLineIndexForMarker)->Visible = false;

			// Finish interaction for all selected clips 
			for (int i=0; i<this->channelList->Count; i++)
			{	
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
				channel->FinishInteraction();
			}

			//	driver conflicts - this is here so resizing works
			//
			int num_conflicts = check_channels_for_driver_conflicts();
			set_driver_conflict_text( num_conflicts );
		}

			 // Clip resized
			 //
	private: System::Void ClipResized(System::Object ^  sender, System::EventArgs ^  e)
		{
		}

			 // Clip moved
			 //
	private: System::Void ClipMoved(System::Object ^  sender, TimelineControls::MouseMoveEventArgs ^  e)
		{
			int num_conflicts = 0;

			// Pass move interaction to all selected clips 
			for (int chi=0; chi<this->channelList->Count; chi++)
			{	
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[chi]);
				channel->InteractMoveSelectedClips(e->TimeDelta);

				const int num_selected = channel->SelectedIndices->Count;
				for (int cli=0; cli<num_selected; cli++)
				{	
					ChannelClip^ clip = dynamic_cast<ChannelClip^>(channel->SelectedClips[cli]);
					align_moving_clip(channel, clip, e->TimeDelta);

					//	driver conflicts
					num_conflicts += check_channel_for_driver_conflicts(channel, clip, e->TimeDelta);
				}
			}

			// Report total number of driver conflicts
			set_driver_conflict_text( num_conflicts );
		}

	private: System::Void SizeChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			int offset = this->panel_channels->AutoScrollPosition.Y;
			int height = c_NamePanelOffsetY + offset;
			int ch_cnt = 0, obj_cnt = 0;
			for (int i=0; i<this->channelList->Count; i++)
			{	
				// Update name label when needed
				if (ch_cnt == 0)
				{
					Label^ label = dynamic_cast<Label^>(labelObjectList[obj_cnt]);
					label->Location = System::Drawing::Point(label->Location.X, height );
					height += label->Height;	
					System::Int32 ^int_ptr = safe_cast<System::Int32^>(this->m_ChannelCount[obj_cnt]);
					ch_cnt = (*int_ptr);
					obj_cnt++;
				}
				ch_cnt--;

				// Offset channel control and channel label
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
				channel->Location = System::Drawing::Point(channel->Location.X, height );
				Label^ label = dynamic_cast<Label^>(labelList[i]);
				label->Location = System::Drawing::Point(label->Location.X, height );
				height += channel->Height;
			}
		}

	private: System::Void ChannelPanel_MouseDown(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e)
			 {
				//DBG_LOG0("channel editor general mouse down");

				m_bDriverSelectStart = true;

				m_DriverSelectStartX = e->X;
				m_DriverSelectStartY = e->Y;

				m_DriverSelectStartTime = get_time_from_point( m_DriverSelectStartX, m_DriverSelectStartY );
			 }
	private: System::Void ChannelPanel_MouseMove(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e)
			 {
				 if (m_bDriverSelectStart)
				 {
					 DBG_LOG0("channel editor general mouse up");

					 //select_clips( m_DriverSelectStartX, m_DriverSelectStartY, e->X, e->Y );
					 float curr_time = get_time_from_point( e->X, e->Y );
					 select_clips_within_time( m_DriverSelectStartTime, curr_time );

					 DBG_LOG2("   start time %6.3f  curr time %6.3f", m_DriverSelectStartTime, curr_time);
				 }
			 }
	private: System::Void ChannelPanel_MouseUp(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e)
			 {
				 //DBG_LOG0("channel editor general mouse up");

				m_bDriverSelectStart = false;
			 }
			 /*
				Calculate the time based on the point given
			 */
	private: float get_time_from_point(int i_CurrentX, int i_CurrentY)
			 {
				DBG_LOG2("panel loc   (%d,%d)", panel_channels->Location.X, panel_channels->Location.Y);
				DBG_LOG2("panel aspos (%d,%d)", panel_channels->AutoScrollPosition.X, panel_channels->AutoScrollPosition.Y);
				DBG_LOG2("panel asoff (%d,%d)", panel_channels->AutoScrollOffset.X, panel_channels->AutoScrollOffset.Y);
				DBG_LOG2("mouseclick  (%d,%d)", i_CurrentX, i_CurrentY);

				int x_offset = (i_CurrentX - panel_channels->AutoScrollPosition.X);
				return (float) this->timeSlider1->GetTimeAtPosition( x_offset );
			 }
	private: void select_clips_within_time( float i_StartTime, float i_CurrentTime )
			 {
				float begin_time, end_time;

				if (i_StartTime <= i_CurrentTime)
				{
					begin_time = i_StartTime;
					end_time = i_CurrentTime;
				}
				else
				{
					begin_time = i_CurrentTime;
					end_time = i_StartTime;
				}

				//	loop through the channels and driver clips and select all the ones that fall within the "box"
				//
				for (int i=0; i<this->channelList->Count; i++)
				{	
					ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);

					int count = 0;

					//	loop through the channels
					for (int i=0; i<this->channelList->Count; i++)
					{	
						ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);

						channel->SelectClips( begin_time, end_time );
					}
				}
			 }
//	private: bool clip_within_box( ChannelClip^ i_Clip, int i_StartX, int i_StartY, int i_CurrentX, int i_CurrentY )
//			 {
//				//	How does this work in the window scrolls?  is it screen relative?
//				//
////				if (   ((i_Clip->xxx >= i_StartX) && (i_Clip->xxx <= i_CurrentX))
////					&& ((i_Clip->yyy >= i_StartY) && (i_Clip->yyy <= i_CurrentY)) )
////				{
////					return true;
////				}
//				return false;
//			 }
//	private: void select_clips( int i_StartX, int i_StartY, int i_CurrentX, int i_CurrentY )
//			 {
//				 //	loop through the channels and driver clips and select all the ones that fall within the "box"
//				 //
//				for (int i=0; i<this->channelList->Count; i++)
//				{	
//					ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
//
//					int count = 0;
//
//					//	loop through the channels
//					for (int i=0; i<this->channelList->Count; i++)
//					{	
//						ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
//
//						//	loop through all the clips of the channels
//						int clips_count = channel->Clips->Count; 
//						for (int j=0; j < clips_count; ++j)
//						{
//							ChannelClip^ cclip = dynamic_cast<ChannelClip^>(channel->Clips[j]);
//							if (clip_within_box( cclip, i_StartX, i_StartY, i_CurrentX, i_CurrentY ))
//							{
//								//	select this clip
//								if (!IsClipSelected(cclip))
//								{
//									channel->BeginInteraction();
//
//									cclip->Select();
//									const bool bAppend = true;
//									channel->SelectClip(j,bAppend);
//
//									channel->FinishInteraction();
//									channel->Invalidate();
//								}
//							}
//						}
//					}
//				}
//			 }


	private: ChannelControl^ get_channel_control( const std::string& i_Name )
			 {
				 //DBG_LOG0("-------------------------------------");
				// compare the label with the channel name.  if a match is found, grab the
				// corresponding channel and return it.
				//
				for (int i=0; i<this->labelList->Count; i++)
				{
					Label^ pCL = dynamic_cast<Label^>(this->labelList[i]);

					std::string name;
					tmaManagedStringUtils::ManagedStringToStdString(pCL->Text,name);
					//DBG_LOG3("%d (%s) vs (%s)", i, name.c_str(), i_Name.c_str() );

					if (strcmp(name.c_str(),i_Name.c_str()) == 0)
					{
						ChannelControl^ pCC = dynamic_cast<ChannelControl^>(this->channelList[i]);
						return pCC;
					}
				}
				return nullptr;
			 }

		//	Calculate the midpoint of all selected clips
		//
	private: System::Boolean clips_are_selected()
		 {
			 int count = 0;

			for (int i=0; i<this->channelList->Count; i++)
			{	
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
				if (channel->SelectedClip > -1)
				{
					return true;
				}
			}

			return false;
		 }

	private: System::Drawing::Point calculate_midpoint_selectedclips()
		 {
			 int count = 0;
			 System::Drawing::Point midpoint(0,0);

			 //	go through the clips and sum up the midpoints
			 //
			for (int i=0; i<this->channelList->Count; i++)
			{	
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
				if (channel->SelectedClip > -1)
				{
					// Got selected clip
					TimelineControls::ChannelClip^ clip = dynamic_cast<TimelineControls::ChannelClip^>(channel->Clips[channel->SelectedClip]);
					if (clip)
					{
						int x,y,width,height;
						channel->GetRectangle( clip, x, y, width, height );
						//int cx, cy;
						//channel->Location(&cx,&cy);
						System::Drawing::Point pnt = channel->Location;
						midpoint.X += pnt.X + (x + width) / 2;
						midpoint.Y += pnt.Y + (y + height) / 2;
						count++;
					}
				}
			}

			DBG_ASSERT0(count > 0, "Trying to find the midpoint of no selected clips");

			//	find the average midpoint and return it
			//
			if (count > 0)
			{
				midpoint.X = midpoint.X / count;
				midpoint.Y = midpoint.Y / count;
			}
			return midpoint;
		 }

		//	adjust the timeline position based on the current time.
		//
	private: void center_view_on_currenttime()
		 {
			System::Drawing::Point newpoint;
			int pos = this->timeSlider1->GetCurrentTimePosition();
			int width = this->panel_channels->ClientRectangle.Width;
			int view_x =  this->panel_channels->ClientRectangle.X;
			int maxx =  (this->panel_channels->DisplayRectangle.Width);
			int midx = (width) / 2;
			if (pos + midx > maxx)
			{
				newpoint = this->panel_channels->AutoScrollPosition;
				newpoint.X = (maxx - width);
				this->panel_channels->AutoScrollPosition = newpoint;
			}
			else
			{
				newpoint = this->panel_channels->AutoScrollPosition;
				newpoint.X = pos - midx;
				this->panel_channels->AutoScrollPosition = newpoint;
			}

			this->panel_channels->Invalidate();
		 }

		//	adjust the timeline position based on the selected clip
		//
	private: void center_view_on_selectedclips()
		 {
			 System::Drawing::Point newpoint = calculate_midpoint_selectedclips();

			 // First the horizontal
			int posx = newpoint.X;
			int width = this->panel_channels->ClientRectangle.Width;
			int view_x =  this->panel_channels->ClientRectangle.X;
			int maxx =  (this->panel_channels->DisplayRectangle.Width);
			int midx = (width) / 2;

			int posy = newpoint.Y;
			int height = this->panel_channels->ClientRectangle.Height;
			int view_y =  this->panel_channels->ClientRectangle.Y;
			int maxy =  (this->panel_channels->DisplayRectangle.Height);
			int midy = (height) / 2;

			newpoint = this->panel_channels->AutoScrollPosition;

			if (posx + midx > maxx)
			{
				newpoint.X = (maxx - width);
			}
			else
			{
				newpoint.X = posx - midx;
			}
			if (posy + midy > maxy)
			{
				newpoint.Y = (maxy - height);
			}
			else
			{
				newpoint.Y = posy - midy;
			}

			this->panel_channels->AutoScrollPosition = newpoint;
			this->panel_channels->Invalidate();
		 }

		//	adjust the timeline position based on the selected clip or current time.
		//
	private: void center_view()
		 {
			 //	see if there are any selected clips to center on.
			 if (clips_are_selected())
			 {
				 center_view_on_selectedclips();
			 }
			 else
			 {
				 center_view_on_currenttime();
			 }
		 }

		//	set the trax editor titlebar text.  used for debug mostly
		//
	private: void set_titlebar(System::String^ i_pTitlebar_String)
			{
				this->Text = i_pTitlebar_String;
			}


	private: void setup_display(tmlnScriptObject* i_pObject)
		 {
			switch (m_nFilterState)
			{
				case 0:
					// Show all selected objects
					MultipleSelectionDisplay();
					break;
				case 1:
					// Single selected object display
					SingleObjectDisplay(i_pObject);
					break;
				case 2:
					// Show system objects
					SystemObjectDisplay();
					break;
				case 3:
					// Show all objects
					AllObjectDisplay();
					break;
			}

			//	driver conflicts
			//
			int num_conflicts = check_channels_for_driver_conflicts();
			set_driver_conflict_text( num_conflicts );
		 }

		//
		// Driver Conflict Functions
		//
	private: void set_driver_conflict_text(int i_NumConflicts)
			 {
				 if (i_NumConflicts == 0)
				 {
					 Control^ pParent = this->checkBox_conflicts->Parent;
					 this->checkBox_conflicts->BackColor = pParent->BackColor;// System::Drawing::SystemColors::Control;
					 this->checkBox_conflicts->ForeColor = System::Drawing::SystemColors::ControlText;
					 this->checkBox_conflicts->Text = "";
					 this->checkBox_conflicts->SendToBack();
					 this->checkBox_conflicts->Visible = false;
					 this->checkBox_conflicts->Enabled = false;
				 }
				 else
				 {
					 this->checkBox_conflicts->BackColor = System::Drawing::Color::Red;
					 this->checkBox_conflicts->ForeColor = System::Drawing::Color::White;
					 if (i_NumConflicts == 1)
						this->checkBox_conflicts->Text = System::String::Format("{0} Driver Conflict ", i_NumConflicts);
					 else
						this->checkBox_conflicts->Text = System::String::Format("{0} Driver Conflicts ", i_NumConflicts);
					 this->checkBox_conflicts->BringToFront();
					 this->checkBox_conflicts->Visible = true;
					 this->checkBox_conflicts->Enabled = true;
				 }
			 }
	private: bool check_driver_conflicts(ChannelClip^ i_Clip1, float i_Delta, ChannelClip^ i_Clip2)
			{
				if (i_Clip1 == i_Clip2)
					return false;

				float c1s, c1e;
				c1s = (float)i_Clip1->BeginTime + i_Delta;
				c1e = (float)i_Clip1->EndTime + i_Delta;
				float c2s, c2e;
				c2s = (float)i_Clip2->BeginTime;
				c2e = (float)i_Clip2->EndTime;

				// check times to see if overlap
				//
				if (   ((c2s < c1s) && (c2e > c1s))		// clip 2 start inside
					|| ((c2s < c1e) && (c2e > c1e))		// clip 2 end inside
					|| ((c1s < c2s) && (c1e > c2s))		// clip 1 start inside
					|| ((c1s < c2e) && (c1e > c2e)))	// clip 1 end inside
				{
					return true;
				}
				return false;
			}
	private: int check_channel_for_driver_conflicts(ChannelControl^ i_pChannel, ChannelClip^ i_pClip, float i_Delta )
			 {
				 //	go through the clips for a channel and see if there is a conflict
				 //
				 int clips_count = i_pChannel->Clips->Count; 
				 int num_conflicts = 0;
				for (int j=0; j < clips_count; ++j)
				{
					ChannelClip^ clip2 = dynamic_cast<ChannelClip^>(i_pChannel->Clips[j]);
					if (check_driver_conflicts(i_pClip,i_Delta, clip2))
					{
						++num_conflicts;
					}
				}
				return num_conflicts;
			 }
	private: int check_channel_for_driver_conflicts(ChannelControl^ i_pChannel, float i_Delta )
			 {
				 //	go through the clips for a channel and see if there is a conflict
				 //
				 int clips_count = i_pChannel->Clips->Count; 
				 int num_conflicts = 0;
				for (int i=0; i < clips_count; ++i)
				{
					ChannelClip^ clip1 = dynamic_cast<ChannelClip^>(i_pChannel->Clips[i]);
					for (int j=i+1; j < clips_count; ++j)
					{
						ChannelClip^ clip2 = dynamic_cast<ChannelClip^>(i_pChannel->Clips[j]);
						if (check_driver_conflicts(clip1,i_Delta, clip2))
						{
							++num_conflicts;
						}
					}
				}
				return num_conflicts;
			 }

	private: int check_channels_for_driver_conflicts()
			 {
				 int num_conflicts = 0;

				//	check the clips of each channel and see if there is a match anywhere->
				//
				for (int i=0; i < this->channelList->Count; i++)
				{
					ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
					num_conflicts += check_channel_for_driver_conflicts(channel,0);
				}
				return num_conflicts;
			 }

			 // align_moving_clip - while moving a clip, snap it to guidelines.
			 //
	private: void align_moving_clip(ChannelControl^ i_pChannel, ChannelClip ^i_pClip, double i_TimeDelta)
		{
			PrefsData& prefs_data = PrefsMgr::Data();

			float closest_time = 0;
			int closest_type = -1;
			bool bMatch = false;
			bool bBeginTimeMatch = false;

			//	
			//	Check for time matches for showing guidelines
			//

			//	get the picked clip
			float pickstart = (float)(i_pClip->BeginTime + i_TimeDelta);
			float pickend	= (float)(i_pClip->EndTime + i_TimeDelta);
			//DBG_LOG5("-----------pick start (%6.3f) pick end (%6.3f) delta (%6.3f)   clip start(%6.3f)  clip end (%6.3f)", pickstart, pickend, i_TimeDelta, i_pClip->BeginTime, i_pClip->EndTime);

			//	check the clips of each channel and see if there is a match anywhere->
			//
			for (int i=0; i < this->channelList->Count; i++)
			{
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);

				for (int j=0; j < channel->Clips->Count; ++j)
				{
					ChannelClip^ clip = dynamic_cast<ChannelClip^>(channel->Clips[j]);
					if ( clip != i_pClip )
					{
						float clipstart = (float)clip->BeginTime;
						float clipend	= (float)clip->EndTime;

						if (chnlSnapUtil::TimesMatch(pickstart, clipstart))
						{
							bMatch = true;
							bBeginTimeMatch = true;
							closest_time = clipstart;
							closest_type = 0;
							//DBG_LOG1("    clip match (%6.3f) start-start", closest_time);
						}
						else if (chnlSnapUtil::TimesMatch(pickstart, clipend))
						{
							bMatch = true;
							bBeginTimeMatch = true;
							closest_time = clipend;
							closest_type = 0;
							//DBG_LOG1("    clip match (%6.3f) end-start", closest_time);
						}
						else if (chnlSnapUtil::TimesMatch(pickend, clipstart))
						{
							bMatch = true;
							bBeginTimeMatch = false;
							closest_time = clipstart;
							closest_type = 1;
							//DBG_LOG1("    clip match (%6.3f) start-end", closest_time);
						}
						else if (chnlSnapUtil::TimesMatch(pickend, clipend))
						{
							bMatch = true;
							bBeginTimeMatch = false;
							closest_time = clipend;
							closest_type = 1;
							//DBG_LOG1("    clip match (%6.3f) end-end", closest_time);
						}
					}
				}
			}

			//	check the markers to see if they match the clip start/end
			//
			float mtime;
			if (chnlMarkerMgr::SnapTimeToMarkers(pickstart, mtime))
			{
				bMatch = true;
				bBeginTimeMatch = true;
				closest_time = mtime;
				closest_type = 2;
				//DBG_LOG1("    markers match (%6.3f) start", closest_time);
			}
			if (chnlMarkerMgr::SnapTimeToMarkers(pickend, mtime))
			{
				bMatch = true;
				bBeginTimeMatch = false;
				closest_time = mtime;
				closest_type = 2;
				//DBG_LOG1("    markers match (%6.3f) end", closest_time);
			}

			//	check current time
			//
			float curtime = (float)this->timeSlider1->CurTime;
			if (chnlSnapUtil::TimesMatch(pickstart, curtime))
			{
				bMatch = true;
				bBeginTimeMatch = true;
				closest_time = curtime;
				closest_type = 3;
				//DBG_LOG1("    current time match (%6.3f) start", closest_time);
			}
			else if (chnlSnapUtil::TimesMatch(pickend, curtime))
			{
				bMatch = true;
				bBeginTimeMatch = false;
				closest_time = curtime;
				closest_type = 3;
				//DBG_LOG1("    current time match (%6.3f) end", closest_time);
			}

			//	found a match
			//
			if (closest_type != -1)
			{
				//DBG_LOG1("        MATCH! (%6.3f)", closest_time);
				TimeGuideLineMgr::GetGuideLine(m_GuideLineIndexForMarker)->Time = closest_time;
				if (prefs_data.m_ChannelEditor_SnapActive.GetValue())
				{
					if (bBeginTimeMatch)
					{
						 i_pClip->InteractStart = closest_time;
					}
					else
					{
						i_pClip->InteractEnd = closest_time;
					}
				}

				switch (closest_type)
				{
					default:
					case 0:
					case 1:
						TimeGuideLineMgr::GetGuideLine(m_GuideLineIndexForMarker)->Color = System::Drawing::Color::Green;
						break;
					case 2:
					case 3:
						TimeGuideLineMgr::GetGuideLine(m_GuideLineIndexForMarker)->Color = System::Drawing::Color::DarkGreen;
						break;
				}
			}
			TimeGuideLineMgr::GetGuideLine(m_GuideLineIndexForMarker)->Visible = bMatch;
		}

	private: bool IsClipSelected(ChannelClip^ i_pClip)
			 {
				if (i_pClip != nullptr)
				{
					chnlDriverClip^ sel_clip = dynamic_cast<chnlDriverClip^>(i_pClip);
					if (sel_clip)
					{
						for (int i=0; i<this->channelList->Count; i++)
						{	
							ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
							for (int c=0; c < channel->SelectedClips->Count; c++)
							{
								//chnlDriverClip^ clip = dynamic_cast<chnlDriverClip^>(channel->SelectedClips[c]);
								//if (clip)
								if (channel->SelectedClips[c] == i_pClip)
								{
									return true;
								}
							}
						}
					}
				}
				return false;
			 }

	private: void checkBox_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
			{
				int i;
				for (i = 0; i < channelLockList->Count; ++i)
				{
					//System::Windows::Forms::CheckBox^ checkbox = dynamic_cast<System::Windows::Forms::CheckBox^>(sender);
					System::Windows::Forms::CheckBox^ checkbox = dynamic_cast<System::Windows::Forms::CheckBox^>(channelLockList[i]);

					//	find the checked
					//
					// FIX: [rjk] - each time a single check changes they all get updated.
					//	this is done as a stopgap since on load, the initial state of the checkbox
					//	doesn't get to the tmlnChannel.
					//
					//if (sender == checkbox)
					{
						bool bCheckState = checkbox->Checked;
						//DBG_LOG2("channel (%s) <-- checkbox (%s)", 
						//	(dynamic_cast<ChannelControl^>(channelList[i])->Locked)?"true":"false",
						//	(bCheckState)?"true":"false" );

						dynamic_cast<ChannelControl^>(channelList[i])->Locked = bCheckState;

						// find the matching channel
						tmlnScriptObject* script_obj = tmlnSelectionUtil::GetSelectedScriptObject();
						if (!script_obj)
						{
							MessageBox::Show("Cannot create channels for an object of this type.","Error");
						}
						else
						{
							tmlnChannelSet& chnl_set = script_obj->ChannelSet();
							int num_channels = chnl_set.GetNumChannels();
							for (int j=0; j<num_channels; j++)
							{
								tmlnChannel& channel = chnl_set.Channel(j);

								std::string labelstring;
								Label^ label = dynamic_cast<Label^>(labelList[i]);
								tmaManagedStringUtils::ManagedStringToStdString( label->Text, labelstring);
								//DBG_LOG3("%d) comparing (%s) with (%s)",j, labelstring.c_str(), channel.GetName().c_str() );
								if ( labelstring == channel.GetName() )
								{
									// match -> set the locked state
									channel.SetLocked( bCheckState );
									//return;	// FIX: [rjk] uncomment this when the above is fixed.
								}
							}
						}
					}
				}
			}

	private: void markerBar1_MarkersShowProperties(MarkerDisplayIcon^ sender)
			{
				chnlMarkerOperations::ShowProperties( (float)sender->Time );	
			}
	private: void markerBar1_NotesShowProperties(NoteDisplayIcon^ sender)
			{
				chnlNotesOperations::ShowProperties( (float)sender->Time );	
			}
	private: System::Void buttonAdd_Click(System::Object ^  sender, System::EventArgs ^  e)
			{
				chnlOperations::ShowAvailableDrivers();
			}

	private: System::Void buttonDelete_Click(System::Object ^  sender, System::EventArgs ^  e)
			{
				chnlOperations::DeleteSelectedDrivers();
			}

	private: System::Void buttonTickRev_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 chnlOperations::MoveTimePrevTick();
			 }

	private: System::Void buttonTickFwd_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 chnlOperations::MoveTimeNextTick();
			 }

	private: System::Void checkSnap_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 if (this->checkSnap->Checked)
					 this->SetSnapInterval((int)g3dConstants::c_fDefaultFrameRate);	// fps?
				 else
					 this->SetSnapInterval(120);
			 }

	private: System::Void checkBox_conflicts_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if (checkBox_conflicts->Checked)
			 {
				// find the clips that have conflict and highlight
				for (int chi=0; chi < this->channelList->Count; chi++)
				{	
					ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[chi]);

					bool bInvalidate = false;
					const int num_selected = channel->Clips->Count;
					for (int cli=0; cli<num_selected; cli++)
					{	
						ChannelClip^ clip = dynamic_cast<ChannelClip^>(channel->Clips[cli]);

						//	driver conflicts
						int num_conflicts = check_channel_for_driver_conflicts(channel, clip, 0);
						if (num_conflicts > 0)
						{
							clip->UseHighlightFill = true;
							bInvalidate = true;
						}
					}

					if (bInvalidate)
						channel->Invalidate();
				}
			 }
			 else
			 {
				// clear all highlighted clips
				for (int chi=0; chi < this->channelList->Count; chi++)
				{	
					ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[chi]);

					bool bInvalidate = false;
					const int num_selected = channel->Clips->Count;
					for (int cli=0; cli<num_selected; cli++)
					{	
						ChannelClip^ clip = dynamic_cast<ChannelClip^>(channel->Clips[cli]);
						clip->UseHighlightFill = false;
						bInvalidate = true;
					}

					if (bInvalidate)
						channel->Invalidate();
				}
			 }
		 }

	private: System::Void comboBox_filter_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 if (comboBox_filter->Text->Equals("All"))
				 {
					 m_nFilterState = 3;
				 }
				 else
				 if (comboBox_filter->Text->Equals("System"))
				 {
					 m_nFilterState = 2;
				 }
				 else
				 if (comboBox_filter->Text->Equals("Selected"))
				 {
					 m_nFilterState = 1;
				 }
				 else
				 {
					 //	"Multiple Selected"
					 m_nFilterState = 0;
				 }

				this->Update();
			 }

	private: System::Void rangedFloat_Zoom_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 this->SetTimeScale( (float)this->rangedFloat_Zoom->Value );
			 }

	private: System::Void chnlTraxEditor_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
			 {
				 //	Don't let the arrow keys move the selection on controls
				 //
				 if (   (e->KeyCode == Keys::Up)
					 || (e->KeyCode == Keys::Down)
					 || (e->KeyCode == Keys::Left)
					 || (e->KeyCode == Keys::Right) )
				 {
					e->Handled = true;
				 }
			 }

	private: System::Void panel_channels_Paint(System::Object ^  sender, System::Windows::Forms::PaintEventArgs ^  e)
			 {
				System::Drawing::Point channel_point = panel_channels->AutoScrollPosition;

				//System::String^ tbtext;
				//tbtext = System::String::Format( "autoscroll {0},{1}", channel_point.X, channel_point.Y );
				//set_titlebar( tbtext );

				//	set the vertical (Y) of names panel
				System::Drawing::Point point = this->panel_names->AutoScrollPosition;
				point.Y = -channel_point.Y;
				panel_names->AutoScrollPosition = point;

				//	set the horizontal (X) of timeline panel
				point = this->panel_timeline->AutoScrollPosition;
				point.X = -channel_point.X;
				panel_timeline->AutoScrollPosition = point;
			 }

	private: System::Void timeSlider1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
		 {
				tmlnTimeLine::SetIsScrubbing( true );
		 }
	private: System::Void timeSlider1_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
		 {
				tmlnTimeLine::SetIsScrubbing( false );
		 }
	private: System::Void button_copy_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 chnlOperations::CopyDrivers();

				 if (tmlnDriverClipboard::Count() > 0)
					 button_paste->Enabled = true;
			 }

	private: System::Void button_paste_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 chnlOperations::PasteDrivers();
			 }

	private: System::Void button_zoomtofit_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 this->CalculateMaxZoom();
			 }


	private: System::Void button_movedrivers_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				// NOTE: [rjk] this is a temporary method of moving drivers.  When multiple driver selecting
				// is implemented this will not be needed.
				//

				//	create a form
				//
				chnlMoveDriversForm^ pMoveForm = gcnew chnlMoveDriversForm(channelList, clips_are_selected());
				//System::Drawing::Size sz(button_movedrivers->Location);
				//System::Drawing::Point pt = System::Drawing::Point::Add(this->Location, sz);
				//pMoveForm->Location = System::Drawing::Point(pt.X, pt.Y);

				//	show the form modal
				pMoveForm->ShowDialog();
				delete pMoveForm;
			 }

	private: System::Void button_split_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			chnlOperations::SplitDrivers();
		}

	private: System::Void button_select_later_drivers_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			// First, gather up the drivers
			//
			//std::set<ChannelClip^> clips_unselected;
			for (int i=0; i<this->channelList->Count; i++)
			{
				ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
				ChannelClipList^ clips = channel->Clips;
				ChannelClipList^ sel_clips = channel->SelectedClips;
				ChannelClip^ the_clip;
				const int num_clips = clips->Count;
				const int num_sel_clips = channel->SelectedClips->Count;

				//	don't allow this interaction if the channel is locked
				//
				if ((num_sel_clips > 0) && !channel->Locked)
				{
					channel->BeginInteraction();

					//	go through the clips and find the EARLIEST.
					//
					double begin_time = sel_clips[0]->BeginTime;
					for (int i=1; i<num_sel_clips; ++i)
					{
						the_clip = sel_clips[i];
						if (begin_time > the_clip->BeginTime)
							begin_time = the_clip->BeginTime;
					}

					//	now, select all the drivers with a later time.
					//
					for (int i=0; i<num_clips; ++i)
					{
						the_clip = clips[i];
						if (   (begin_time <= the_clip->BeginTime)
							&& (!IsClipSelected(the_clip)) )
						{
							the_clip->Select();
							const bool bAppend = true;
							channel->SelectClip(i,bAppend);
						}
					}

					channel->FinishInteraction();

					//
					channel->Invalidate();

					//
					//DBG_LOG0("Selected after finish");
					//ChannelClipList ^sclips = channel->SelectedClips;
					//const int num_sclips = sclips->Count;
					//for (int i=0; i<num_sclips; ++i)
					//{
					//	DBG_LOG2("%d %s", i, "selected clip");
					//}
				}
			}
		 }
	private: System::Void button_addmarker_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 chnlMarkerOperations::AddMarker();
			 }
	private: System::Void button_delmarker_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 chnlMarkerOperations::DeleteMarker();
			 }
	private: System::Void button_prevmarker_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 chnlMarkerOperations::MoveToPrevMarker();
			 }
	private: System::Void button_nextmarker_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 chnlMarkerOperations::MoveToNextMarker();
			 }

	private: System::Void button_addnote_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 chnlNotesOperations::AddNote();
			 }
	private: System::Void button_delnote_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 chnlNotesOperations::DeleteNote();
			 }
	private: System::Void button_prevnote_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 chnlNotesOperations::MoveToPrevNote();
			 }
	private: System::Void button_nextnote_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 chnlNotesOperations::MoveToNextNote();
			 }
};
}


#endif // _MANAGED
