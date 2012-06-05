/*****************************************************************************
**	fcuiFormMgr.hpp
**
**		manages the main UI elements for Fusion Cinema
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcui/fcuiFormMgr.hpp"

#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcui/fcuiTimelineMgr.hpp"
#include "FCSupport/fcui/qtGUI/fcuiTimelineItem.hpp"
#include "FCSupport/fcui/qtGUI/qtfc3d.h"
#include "FCSupport/fcui/qtGUI/fcuiButtonLayout.hpp"
#include "FCSupport/fcui/qtGUI/fcuiPanel.hpp"
#include "FCSupport/fcui/qtGUI/fcuiLabel.hpp"
#include "FCSupport/fcui/qtGUI/fcuiListViewOperations.hpp"
#include "FCSupport/fcui/qtGUI/fcuiTextEditOperations.hpp"
#include "FCSupport/fcui/qtGUI/fcuiTimeline.hpp"

#include "FCSupport/path/pathDirectoryParser.hpp"

#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/app/appApplication.hpp"
#include "Core/it/itStringUtil.hpp"

#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

//============================================================================
//============================================================================
namespace fcuiFormMgr
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	namespace
	{
		QtFC3D* l_pMainWindow = NULL;
		fcuiButtonLayout* l_pCameraPicker = NULL;
		fcuiPanel* l_pNewMoviePanel = NULL;
		fcuiPanel* l_pModePanel = NULL;
		fcuiPanel* l_pSubjectPanel = NULL;
		fcuiPanel* l_pCategoryPanel = NULL;
		fcuiPanel* l_pElementPanel = NULL;
		fcuiPanel* l_pCustomIconPanel = NULL;
		const int l_cLABELS_MAX = 10;
		fcuiLabel* l_pSubjectLabels[l_cLABELS_MAX];
		fcuiLabel* l_pCategoryLabels[l_cLABELS_MAX];
		fcuiLabel* l_pModeLabels[l_cLABELS_MAX];
		fcuiTextEditOperations* l_pTextOps = NULL;
		fcuiTimeline* l_pActionTimeline = NULL;
		fcuiListViewOperations* l_pListOps = NULL;
		fcuiListViewOperations* l_pMovieBG = NULL;
		QRect l_WatchPanelGeometry;
		QRect l_MovieBGGeometry;

		bool l_bPacksVisible = false;
		bool l_bWatchPanelVisible = false;
		bool l_bAlreadyAdjustedTime = false;

		// dimensions for the panels
		const int l_NewMovieColumns = 7;
		const int l_NewMovieRows = 1;
		const int l_ModeColumns = 5;
		const int l_ModeRows = 1;
		const int l_SubjectColumns = 5;
		const int l_SubjectRows = 1;
		const int l_CategoryColumns = 5;
		const int l_CategoryRows = 1;
		const int l_ElementColumns = 2;
		const int l_ElementRows = 3;
		const int l_CustomIconColumns = 2;
		const int l_CustomIconsRows = 1;

		//mode strings
		const itString l_CastString(fcuiConstants::c_MODE_CAST);
		const itString l_LocationString(fcuiConstants::c_MODE_LOCATION);
		const itString l_ActionString(fcuiConstants::c_MODE_ACTION);
		const itString l_MakeString(fcuiConstants::c_MODE_MAKE);
		const itString l_TheaterString(fcuiConstants::c_MODE_THEATER);

		//action subject strings
		const itString l_AudioString(fcuiConstants::c_ACTION_STATE_AUDIO);
		const itString l_SceneString(fcuiConstants::c_ACTION_STATE_SCENE);
		const itString l_LightsString(fcuiConstants::c_ACTION_STATE_LIGHTS);
		const itString l_PostFXString(fcuiConstants::c_ACTION_STATE_POSTFX);
		const itString l_TitleCardString(fcuiConstants::c_ACTION_STATE_TITLECARD);
	}

	//------------------------------------------------------------------------
	// Do any form initializations here
	//------------------------------------------------------------------------
	void Init()
	{
		if (l_pMainWindow == NULL)
			return;

		//set the watch/edit panel start geometry
		l_WatchPanelGeometry = l_pMainWindow->ui.Home_ListMovieWidget->geometry();
		l_MovieBGGeometry = l_pMainWindow->ui.Home_MovieClipBGWidget->geometry();
		l_pMainWindow->ui.Home_ListMovieWidget->setVisible(false);
		l_pMainWindow->ui.Home_MovieClipBGWidget->setVisible(false);

		l_bWatchPanelVisible = false;
		l_pListOps = new fcuiListViewOperations();
		l_pMovieBG = new fcuiListViewOperations();
		l_pTextOps = new fcuiTextEditOperations();
		QWidget* panel ;
		//QWidget* scrollpanel;

		itString title_str(mnmConstants::c_PRODUCT_FOR_DISPLAY);
		l_pMainWindow->setObjectName( itStringUtil::GetStdString(title_str).c_str() );

		//create the New Movie panel
		panel = l_pMainWindow->ui.Home_MovieClipWidget;
		l_pNewMoviePanel = new fcuiPanel(panel, l_NewMovieColumns, l_NewMovieRows, fcmdModeMgr::e_HomeScreen);
		l_pNewMoviePanel->SetPanelScrollButtons(l_pMainWindow->ui.Home_ScrollLeftButton, l_pMainWindow->ui.Home_ScrollRightButton);
		l_pNewMoviePanel->ShowScrollButtons(false);
		l_pNewMoviePanel->show();

		//create the Mode panel
		panel = l_pMainWindow->ui.ModePanel;
		l_pModePanel = new fcuiPanel(panel, l_ModeColumns, l_ModeRows, fcmdModeMgr::e_Mode);
		l_pModePanel->show();

		//create the subject panel
		panel = l_pMainWindow->ui.SubjectPanel;
		l_pSubjectPanel = new fcuiPanel(panel, l_SubjectColumns, l_SubjectRows, fcmdModeMgr::e_Subject);
		l_pSubjectPanel->SetPanelScrollButtons(l_pMainWindow->ui.Edit_SubjectScroll_Home, l_pMainWindow->ui.Edit_SubjectScroll_End);
		l_pSubjectPanel->ShowScrollButtons(false);
		l_pSubjectPanel->show();

 
		//create the category panel
		panel = l_pMainWindow->ui.CategoriesPanel;
		l_pCategoryPanel = new fcuiPanel(panel, l_CategoryColumns, l_CategoryRows, fcmdModeMgr::e_Category);
		l_pCategoryPanel->SetPanelScrollButtons(l_pMainWindow->ui.Edit_CategoryScroll_Home, l_pMainWindow->ui.Edit_CategoryScroll_End);
		l_pCategoryPanel->ShowScrollButtons(false);
		l_pCategoryPanel->show();

		//create the element panel
		panel = l_pMainWindow->ui.ElementPanel;
		l_pElementPanel = new fcuiPanel(panel, l_ElementColumns, l_ElementRows, fcmdModeMgr::e_Element, true, true);
		l_pElementPanel ->SetPanelScrollBar(l_pMainWindow->ui.Edit_ElementScrollBar);
		l_pElementPanel->show();
		
		//create the custom icons panel
		panel = l_pMainWindow->ui.Edit_CustomNoneWidget;
		l_pCustomIconPanel = new fcuiPanel(panel, l_CustomIconColumns, l_CustomIconsRows, fcmdModeMgr::e_CustomIcons);
		l_pCustomIconPanel->show();

		//create the action timeline list view
		panel = l_pMainWindow->ui.Action_TimelineWidget;
		l_pActionTimeline = new fcuiTimeline(panel);
		l_pActionTimeline->SetTimelineScrollButtons(l_pMainWindow->ui.Edit_ActionScrollLeftButton, l_pMainWindow->ui.Edit_ActionScrollRightButton);
		l_pActionTimeline->ShowScrollButtons(false);
		l_pActionTimeline->hide();

		//hide the camera selection frame for now
		panel = l_pMainWindow->ui.zAction_CameraPickerWidget;
		l_pCameraPicker = new fcuiButtonLayout(panel);
		l_pCameraPicker->HideLayout();
		
		UpdateCurrentMovieTime();
		UpdateMovieEndTime();
	}

	//------------------------------------------------------------------------
	// Do any cleanup here
	//------------------------------------------------------------------------
	void CleanUp()
	{
		if (l_pMainWindow != NULL)
		{
			delete l_pMainWindow;
			l_pMainWindow = NULL;
			l_pNewMoviePanel = NULL;
			l_pModePanel = NULL;
			l_pSubjectPanel = NULL;
			l_pCategoryPanel = NULL;
			l_pElementPanel = NULL;
			l_pCustomIconPanel = NULL;
			l_pActionTimeline = NULL;
		}
		delete l_pListOps;
		l_pListOps = NULL;

		delete l_pTextOps;
		l_pTextOps = NULL;

		delete l_pMovieBG;
		l_pMovieBG = NULL;
	}

	//------------------------------------------------------------------------
	/// Perform any form think operations
	//------------------------------------------------------------------------
	void Think()
	{
		if( l_pMainWindow == NULL )
			return;

		if( l_pActionTimeline != NULL && l_pActionTimeline->isVisible() )
			l_pActionTimeline->Think();
	}

	//------------------------------------------------------------------------
	// create the main window
	//------------------------------------------------------------------------
	void CreateMainWindow()
	{
		if (l_pMainWindow == NULL)
			l_pMainWindow = new QtFC3D();
	}

	//------------------------------------------------------------------------
	// Return our main window
	//------------------------------------------------------------------------
	QtFC3D* GetMainWindow()
	{
		return l_pMainWindow;
	}

	//------------------------------------------------------------------------
	/// Returns whether or not the main window has focus
	//------------------------------------------------------------------------
	bool MainHasFocus()
	{
		if(l_pMainWindow == NULL)
			return false;

		return appApplication::IsSuspended();
	}

	//------------------------------------------------------------------------
	/// Populate the watch/edit list with the appropriate contents
	//------------------------------------------------------------------------
	void PopulateEditList(const fsLocator& i_DirectoryName)
	{
		if(l_pMainWindow != NULL)
			l_pMainWindow->EditList(i_DirectoryName);
	}

	//------------------------------------------------------------------------
	/// Load the movie requested by the mode manager
	//------------------------------------------------------------------------
	void LoadMovie(const itString& i_MovieName)
	{
		if( l_pMainWindow != NULL )
			l_pMainWindow->LoadMoviePack(i_MovieName);
	}

	//------------------------------------------------------------------------
	/// Hide the movie pack list and play it's animation
	//------------------------------------------------------------------------
	void HideMoviePacks()
	{
		l_pMainWindow->ui.Home_ScrollRightWidget->setVisible(false);
		l_pMainWindow->ui.Home_ScrollLeftWidget->setVisible(false);
		l_pMainWindow->ui.Home_MovieClipBGWidget->setVisible(false);

		if( (l_pNewMoviePanel != NULL) && l_bPacksVisible )
		{
			HideMoviePackBG();
			l_pNewMoviePanel->ShowScrollButtons(false);
			l_pNewMoviePanel->DoHideAnimation();
			l_bPacksVisible = false;
		}
	}

	//------------------------------------------------------------------------
	// update the icons in the panel with the images files of a directory
	//------------------------------------------------------------------------
	void UpdatePanelIcons(int i_ModeID, const std::vector<fsLocator>& i_IconList)
	{
		if (l_pMainWindow == NULL)
			return; 

		//depending on the mode, decide which panel to clear and repopulate
		switch( (fcmdModeMgr::Mode)i_ModeID )
		{
		case fcmdModeMgr::e_Mode:
			l_pModePanel->ClearItems();
			UpdateModePanel(i_IconList);
			break;

		case fcmdModeMgr::e_Subject:
			l_pSubjectPanel->ReLoadItems(i_IconList);
			break;

		case fcmdModeMgr::e_Category:
			l_pCategoryPanel->ReLoadItems(i_IconList);
			break;

		case fcmdModeMgr::e_Element:
			l_pElementPanel->ReLoadItems(i_IconList);
			break;

		case fcmdModeMgr::e_CustomIcons:
			l_pCustomIconPanel->ReLoadItems(i_IconList);
			break;

		case fcmdModeMgr::e_HomeScreen:
			if(!l_bPacksVisible)
			{
				l_bPacksVisible = true;
				ShowMoviePackBG();
				l_pNewMoviePanel->ReLoadItems(i_IconList);
			}
			break;

		default:
			break;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateModePanel(const std::vector<fsLocator>& i_IconList)
	{
		int index;
		itString mode_string;
		for ( int i = 0; i < i_IconList.size(); ++i)
		{
			index = -1;
			mode_string = i_IconList[i].GetLastName();

			DBG_TRACE("[" << mode_string << "](" << l_TheaterString << ")(" << l_ActionString << ")(" << l_LocationString << ")(" << l_CastString << ")(" << l_MakeString << ")");
			if ( mode_string == l_CastString)
				index = 0;
			else if ( mode_string == l_LocationString )
				index = 1;
			else if ( mode_string == l_ActionString )
				index = 2;
			else if ( mode_string == l_MakeString )
				index = 3;
			else if ( mode_string == l_TheaterString )
				index = 4;
		
			//if the mode is found, load the icon in its appropriate place on the mode panel
			//also set the mode ID and root directory for the mode
			if ( index > -1 )
			{
				l_pModePanel->LoadItemAtIndex(i_IconList[i], index);
				l_pModePanel->SetItemID(index, index);
				l_pModePanel->SetItemLocator(index, fcmdModeMgr::GetModeLocator((fcmdModeMgr::Mode)index));
			}
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReOrderActionPanel(const std::vector<fsLocator>& i_IconList)
	{
		int index;
		itString subject_string;
		for ( int i = 0; i < i_IconList.size(); ++i)
		{
			index = -1;
			subject_string = i_IconList[i].GetLastName();

			if ( subject_string == l_SceneString)
				index = 0;
			else if ( subject_string == l_AudioString )
				index = 1;
			else if ( subject_string == l_LightsString )
				index = 2;
			else if ( subject_string == l_TitleCardString )
				index = 3;
			else if ( subject_string == l_PostFXString )
				index = 4;
		
			//if the mode is found, load the icon in its appropriate place on the mode panel
			//also set the mode ID and root directory for the mode
			if ( index > -1 )
			{
				l_pSubjectPanel->LoadItemAtIndex(i_IconList[i], index);
				l_pSubjectPanel->SetItemLocator(index, i_IconList[i]);
			}
		}
	}

	//------------------------------------------------------------------------
	/// Highlight the panel item at the given index
	//------------------------------------------------------------------------
	void HighlightPanelItemAtIndex(int i_ModeID, int i_Index, bool i_bHighlightAfterClear)
	{
		if (l_pMainWindow == NULL)
			return; 

		//depending on the mode, decide which panel to clear and repopulate
		switch( (fcmdModeMgr::Mode)i_ModeID )
		{
		case fcmdModeMgr::e_Mode:
			l_pModePanel->HighlightItemAtIndex(i_Index, i_bHighlightAfterClear);
			break;

		case fcmdModeMgr::e_Subject:
			l_pSubjectPanel->HighlightItemAtIndex(i_Index, i_bHighlightAfterClear);
			break;

		case fcmdModeMgr::e_Category:
			l_pCategoryPanel->HighlightItemAtIndex(i_Index, i_bHighlightAfterClear);
			break;

		case fcmdModeMgr::e_Element:
			l_pElementPanel->HighlightItemAtIndex(i_Index, i_bHighlightAfterClear);
			break;

		case fcmdModeMgr::e_CustomIcons:
			l_pCustomIconPanel->HighlightItemAtIndex(i_Index, i_bHighlightAfterClear);
			break;

		case fcmdModeMgr::e_HomeScreen:
			if(!l_bPacksVisible)
			{
				l_pNewMoviePanel->HighlightItemAtIndex(i_Index, i_bHighlightAfterClear);
			}
			break;

		default:
			break;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateElementName(const itString& i_ElementName)
	{
		if (l_pMainWindow == NULL)
			return;

		QString elem_name( itStringUtil::GetStdString(i_ElementName).c_str() );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetActionTimelineEnabled(bool i_bEnabled)
	{
		if(l_pActionTimeline)
		{
			if(i_bEnabled)
				l_pActionTimeline->AnimateDropIn();
			else
				l_pActionTimeline->AnimateDropOut();
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CreateLabels(QWidget* i_pParent, fsLocator i_Directory, int i_ModeID)
	{
		std::vector<fsLocator> filenames;
		pathDirectoryParser pdp;
		if( i_Directory.GetNumNames() > 0 )
			pdp.GetClippedSubDirectories(i_Directory, filenames);

		//	if this case happens it means the user accidentally deleted a folder
		//	they shouldn't have (or we are testing new movie packs and haven't
		//	created the proper number of folders, so resize the filenames list
		//	here.
		//
		//	TODO - let the user know of this error!
		//
		if (filenames.size() < 4 && i_ModeID == 0 )	// the required number of dirs
		{
			//	This is bad
			DBG_ERROR("Folders are missing under " << i_Directory);
			return;
		}

		switch( (fcmdModeMgr::Mode)i_ModeID )
		{
			case fcmdModeMgr::e_Mode:
			{
				//	if the label list is too small, report it
				//
				if (filenames.size() > l_cLABELS_MAX)
				{
					DBG_ASSERT(false, "Too many filenames for the maximum number of labels.  " << filenames.size());
				}

				//	Clear out the strings first
				for (int j=0;j< l_cLABELS_MAX;++j)
				{
					if (l_pModeLabels[j] != NULL)
					{
						l_pModeLabels[j]->Clear();
					}
				}

				//	Loop through and either update an existing label or create one.
				//
				int labelNumber = 0;
				fsLocator temp = filenames[0];
				filenames[0] = filenames[1];
				filenames[1] = filenames[2];
				filenames[2] = temp;
				for (int i = 0 ; i < filenames.size(); i++)
				{
					if (l_pModeLabels[i] == NULL)
					{
						l_pModeLabels[i] = new fcuiLabel(i_pParent, filenames[i], labelNumber++, 64);
					}
					else
					{
						l_pModeLabels[i]->SetUp(i_pParent, filenames[i], labelNumber++, 64);
					}
					l_pModeLabels[i]->show();
				}
			}
			break;
			case fcmdModeMgr::e_Subject:
			{
				//	if the label list is too small, resize it.
				//
				if (filenames.size() > l_cLABELS_MAX)
				{
					DBG_ASSERT(false, "Too many filenames for the maximum number of labels.  " << filenames.size());
				}

				//	Clear out the strings first
				for (int j=0;j< l_cLABELS_MAX;++j)
				{
					if (l_pSubjectLabels[j] != NULL)
					{
						l_pSubjectLabels[j]->Clear();
					}
				}

				int labelNumber = 0;
				for (int i = 0 ; i < filenames.size(); i++)
				{
					if (l_pSubjectLabels[i] == NULL)
					{
						l_pSubjectLabels[i] = new fcuiLabel(i_pParent, filenames[i], labelNumber++, 64);
					}
					else
					{
						l_pSubjectLabels[i]->SetUp(i_pParent, filenames[i], labelNumber++, 64);
					}
					l_pSubjectLabels[i]->show();
				}
			}
			break;
			case fcmdModeMgr::e_Category:
			{
				//	if the label list is too small, resize it.
				//
				if (filenames.size() > l_cLABELS_MAX)
				{
					DBG_ASSERT(false, "Too many filenames for the maximum number of labels.  " << filenames.size());
				}

				//	Clear out the strings first
				for (int j=0;j< l_cLABELS_MAX;++j)
				{
					if (l_pCategoryLabels[j] != NULL)
					{
						l_pCategoryLabels[j]->Clear();
					}
				}

				//	Loop through and either update an existing label or create one.
				//
				int labelNumber = 0;
				for (int i = 0 ; i < filenames.size(); i++)
				{
					if (l_pCategoryLabels[i] == NULL)
					{
						l_pCategoryLabels[i] = new fcuiLabel(i_pParent, filenames[i], labelNumber++, 47);
					}
					else
					{
						l_pCategoryLabels[i]->SetUp(i_pParent, filenames[i], labelNumber++, 47);
					}
					l_pCategoryLabels[i]->show();
				}
			}
			break;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ShowCameraPicker(fcuiTimelineItemData& io_ItemData, 
						  const std::vector<fsLocator>& i_ButtonDirectories, 
						  void (*i_CallbackFunction)(fcuiTimelineItemData& io_ItemData, const fsLocator& i_Locator ))
	{
		if (l_pMainWindow == NULL)
			return;

		l_pCameraPicker->SetupCameraLayout(i_ButtonDirectories.size());
		l_pCameraPicker->PopulateCameraButtons(io_ItemData, i_ButtonDirectories, i_CallbackFunction);
		l_pCameraPicker->ShowLayout();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void HideCameraPicker()
	{
		if (l_pMainWindow == NULL)
			return;

		l_pCameraPicker->HideLayout();
		l_pMainWindow->update();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetTimelineEditMode(bool i_bEditing)
	{
		if (l_pMainWindow == NULL)
			return;

		l_pActionTimeline->SetIsEditing(i_bEditing);
	}

	//------------------------------------------------------------------------
	/// Update the selection of the timeline based on the current time of the 
	/// playback
	//------------------------------------------------------------------------
	void UpdateTimelineSelection(const maTime& i_CurrentTime)
	{
		if (l_pMainWindow == NULL)
			return;

		l_pActionTimeline->SelectItemAtTime(i_CurrentTime);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateTimelineItemData(const fcuiTimelineItemData& i_OldData, const fcuiTimelineItemData& i_NewData)
	{
		if (l_pMainWindow == NULL)
			return;

		l_pActionTimeline->UpdateItemData(i_OldData, i_NewData);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ResetTimelineEdit()
	{
		if (l_pMainWindow == NULL)
			return;
		l_pTextOps->CancelEdit();
		l_pCameraPicker->HideLayout();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ClearTimeline()
	{
		l_pActionTimeline->clear();
	}

	//------------------------------------------------------------------------
	/// Animate the watch/edit panel into view
	//------------------------------------------------------------------------
	void ShowWatchPanel(const fsLocator& i_DirectoryName)
	{
		QWidget* panel = l_pMainWindow->ui.Home_ListMovieWidget;
		
		panel->setVisible(true);
		
		QRect start = l_WatchPanelGeometry;
		QRect end = start;

		//start the widget behind the home screen frame and slide into position
		start.setX( (start.x() - 200) );
		if(l_bWatchPanelVisible)
		{
			l_bWatchPanelVisible = true;
			l_pListOps->ReLoadListView(i_DirectoryName, panel, end, start, 500);
		}
		else
		{
			l_bWatchPanelVisible = true;
			l_pListOps->LoadListView(i_DirectoryName, panel, start, end, 500);
		}
	}

	//------------------------------------------------------------------------
	/// Animate the watch/edit panel out of view
	//------------------------------------------------------------------------
	void HideWatchPanel()
	{
		QWidget* panel = l_pMainWindow->ui.Home_ListMovieWidget;
		if(l_bWatchPanelVisible)
		{
			QRect start = l_WatchPanelGeometry;
			QRect end = start;
			//end the widget behind the home screen frame
			end.setX( (start.x() - 200) );

			l_bWatchPanelVisible = false;
			l_pListOps->RemoveListView(panel, start, end, 500);
		}
		
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ShowMoviePackBG()
	{
		QWidget* panel = l_pMainWindow->ui.Home_MovieClipBGWidget;
		
		panel->setVisible(true);
		
		QRect start = l_MovieBGGeometry;
		QRect end = start;

		//start the widget behind the home screen frame and slide into position
		start.setY( (start.y() - 200) );
		l_pMovieBG->ShowListView(panel, start, end, 200);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void HideMoviePackBG()
	{
		QWidget* panel = l_pMainWindow->ui.Home_MovieClipBGWidget;
		
		QRect start = l_MovieBGGeometry;
		QRect end = start;
		//end the widget behind the home screen frame
		end.setY( (start.y() - 200) );

		l_pMovieBG->HideListView(panel, start, end, 500, false);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fcuiTextEditOperations* GetTextEdits()
	{
		return l_pTextOps;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateCurrentMovieTime(bool i_bUpdateSlider)
	{
		if (l_pMainWindow == NULL)
			return;
		std::string time_string;
		tmlnTimeUtil::GetTimeStringInMSM(tmlnTimeLine::GetValue(), time_string);
		l_pMainWindow->ui.Action_CurrentTimeLabel->setText(QString(time_string.c_str()));

		if( fcuiTimelineMgr::Instance != NULL && i_bUpdateSlider)
		{
			float percent = tmlnTimeLine::GetValue() / fcuiTimelineMgr::Instance->GetEndTime();
			int sliderVal = percent * 100;
			l_bAlreadyAdjustedTime = true;
			AdjustTimelineProgress(sliderVal);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateMovieEndTime()
	{
		if (l_pMainWindow == NULL)
			return;
		
		std::string time_string = "00:00:00";
		if( fcuiTimelineMgr::Instance != NULL )
		{
			tmlnTimeUtil::GetTimeStringInMSM(fcuiTimelineMgr::Instance->GetEndTime(), time_string);
		}
		
		l_pMainWindow->ui.Action_EndTimeLabel->setText(QString(time_string.c_str()));
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AdjustTimelineProgress(int i_ProgressValue)
	{
		if (l_pMainWindow == NULL)
			return;
		QSlider* slider = l_pMainWindow->ui.Edit_TimelineSlider;
		slider->setValue(i_ProgressValue);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void TimeSliderMoved(int i_SliderValue)
	{
		if (l_pMainWindow == NULL)
			return;

		if( l_bAlreadyAdjustedTime )
		{
			l_bAlreadyAdjustedTime = false;
			return;
		}
		float percent = (float)i_SliderValue / 100.0f;
		float time = 0.0f;
		if( fcuiTimelineMgr::Instance != NULL )
		{
			time = percent * fcuiTimelineMgr::Instance->GetEndTime().AsSeconds();
		}
		maTime new_time = maTime::FromSeconds(time);
		l_pActionTimeline->SelectItemAtTime(new_time);
		tmlnTimeLine::SetValue(new_time);
		UpdateCurrentMovieTime(false);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void EnableTimelineSlider(bool i_bEnable)
	{
		if (l_pMainWindow == NULL)
			return;
		QSlider* slider = l_pMainWindow->ui.Edit_TimelineSlider;
		slider->setEnabled(i_bEnable);
	}

	//------------------------------------------------------------------------
	/// Reset the checkboxes on the element panel and check the appropriate box
	/// for the current mode
	//------------------------------------------------------------------------
	void ResetCheckboxes(const fsLocator& i_Locator)
	{
		if (l_pMainWindow == NULL)
			return;

		//show the checkboxes
		l_pElementPanel->PrepareResetCheck(i_Locator);
	}

	//------------------------------------------------------------------------
	/// Show the checkboxes on the element panel
	//------------------------------------------------------------------------
	void ShowCheckboxes()
	{
		if (l_pMainWindow == NULL)
			return;

		//show the checkboxes
		l_pElementPanel->PrepareShowCheckboxItems();
	}

	//------------------------------------------------------------------------
	/// Hide the checkboxes on the element panel
	//------------------------------------------------------------------------
	void HideCheckboxes()
	{
		if (l_pMainWindow == NULL)
			return;

		//hide the checkboxes
		l_pElementPanel->HideItemCheckboxes();
	}

	//------------------------------------------------------------------------
	/// Update the selected element pairing for the panel, used only by 
	/// element panel
	//------------------------------------------------------------------------
	void UpdateSelectedElementPair(const fsLocator& i_Category, const fsLocator& i_SelectedElement)
	{
		if (l_pMainWindow == NULL)
			return;

		//hide the checkboxes
		l_pElementPanel->UpdateSelectedItemPair(i_Category, i_SelectedElement);
	}

	//------------------------------------------------------------------------
	/// Given the category, select the appropriate element in the panel
	//------------------------------------------------------------------------
	void SelectElementPair(const fsLocator& i_Category)
	{
		if (l_pMainWindow == NULL)
			return;

		//hide the checkboxes
		fsLocator element_dir = l_pElementPanel->SelectItemPair(i_Category);

		itString name("");
		if(element_dir.GetNumNames() > 0)
			name = element_dir.GetLastName();
		
		UpdateElementName(name);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void HighlightItems()
	{
		l_pElementPanel->Initialize();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WriteTimelineItemsToChunk()
	{
		l_pActionTimeline->WriteChunk();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateTimelineItems()
	{
		std::vector<fcuiTimelineItemData> timelineItems;
		l_pActionTimeline->ReadChunk( timelineItems );
		l_pActionTimeline->PopulateTimeline( timelineItems );
	}

}	//end namespace fcuiFormMgr
