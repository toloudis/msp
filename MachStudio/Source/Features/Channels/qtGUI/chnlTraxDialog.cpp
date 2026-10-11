/****************************************************************************\
**	chnlTraxDialog.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/qtGUI/chnlTraxDialog.hpp"

#include "Features/Channels/chnlOperations.hpp"
#include "Features/Channels/chnlSnapUtil.hpp"
#include "Features/Channels/chnlTimeTickMgr.hpp"
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Features/Channels/Markers/chnlMarkerOperations.hpp"
#include "Features/Channels/Notes/chnlNotesMgr.hpp"
#include "Features/Channels/Notes/chnlNotesOperations.hpp"
#include "Features/Channels/qtGUI/chnlClipDriver.hpp"
#include "Features/Channels/qtGUI/chnlMarkerBar.hpp"
#include "Features/Channels/qtGUI/chnlTimeLabel.hpp"
#include "Features/Channels/qtGUI/chnlTimelineGuideUtil.hpp"
#include "Features/Channels/qtGUI/chnlTimelinePanel.hpp"
#include "Features/Channels/qtGUI/chnlTimeSlider.hpp"
#include "Features/Prefs/PrefsMgr.hpp"

#undef GetTimeFormat

#include "Support/tmln/tmlnChannel.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnDriverDialogUtil.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnTimeEditUIInfo.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/Ma/maFunctions.hpp"
#include "Core/Fs/fsFileUtil.hpp"
//#include "Graphics/g3d/g3dConstants.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiPropertyDialog.hpp"
#include "ToolUIQt/tqc/tqcRangedFloat.hpp"
#include "ToolUIQt/tqt/tqtMessaging.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

#include <algorithm>
#include <iterator>
#include <sstream>


#ifdef USE_QT

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QToolButton>
#include <QVBoxLayout>

//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	QString get_icon_directory()
	{
		itString wide_dir;
		fsFileUtil::LocatorToUnicodeString(guiMenuMgr::GetIconDirectory(), wide_dir);
		return QString::fromWCharArray(wide_dir.GetString());
	}

	const float c_InitialZoom = 60.0f;
	const int c_DefaultSnapInterval = 120;

	//--------------------------------------------------------------------
	// Small button with a tool tip
	//--------------------------------------------------------------------
	QPushButton* create_button(QWidget* i_pParent, const char* i_pText, const char* i_pToolTip, int i_Width, int i_Height = -1)
	{
		QPushButton* pButton = new QPushButton(i_pText, i_pParent);
		pButton->setToolTip(i_pToolTip);
		pButton->setAutoDefault(false);
		if (i_Height > 0)
			pButton->setFixedSize(i_Width, i_Height);
		else
			pButton->setFixedWidth(i_Width);
		return pButton;
	}

	//--------------------------------------------------------------------
	// Button for locking a channel
	//--------------------------------------------------------------------
	QToolButton* create_lock_button(QWidget* i_pParent, tmlnChannel &i_Channel, chnlChannelControl* i_pChannelCtrl)
	{
		const QString icon_dir = get_icon_directory();
		const QIcon unlock(icon_dir + "\\padlock_disabled.png");
		const QIcon lock(icon_dir + "\\padlock.png");

		QToolButton* pButton = new QToolButton(i_pParent);
		pButton->setAutoRaise(true);
		pButton->setIcon(i_Channel.IsLocked() ? lock : unlock);
		tmlnChannel* pChannel = &i_Channel;
		QObject::connect(pButton, &QToolButton::clicked, [pButton, pChannel, i_pChannelCtrl, lock, unlock]()
		{
			bool bChecked = !pChannel->IsLocked();
			pChannel->ChannelLocked(bChecked);
			i_pChannelCtrl->SetLocked(bChecked);
			pButton->setIcon(bChecked ? lock : unlock);
		});
		return pButton;
	}

	//--------------------------------------------------------------------
	// Delete the widgets in a layout, keeping the final stretch
	//--------------------------------------------------------------------
	void clear_layout(QVBoxLayout* io_pLayout)
	{
		while (io_pLayout->count() > 1)
		{
			QLayoutItem* pItem = io_pLayout->takeAt(0);
			delete pItem->widget();
			delete pItem;
		}
	}

	//--------------------------------------------------------------------
	// Scroll area whose content widget is sized by its layout
	//--------------------------------------------------------------------
	QScrollArea* create_scroll_area(QWidget* i_pParent, QWidget* i_pContent)
	{
		QScrollArea* pScroll = new QScrollArea(i_pParent);
		pScroll->setWidgetResizable(true);
		pScroll->setWidget(i_pContent);
		return pScroll;
	}

	//--------------------------------------------------------------------
	// Vertical layout with no spacing, its items stacked at the top
	//--------------------------------------------------------------------
	QVBoxLayout* create_stack_layout(QWidget* i_pParent)
	{
		QVBoxLayout* pLayout = new QVBoxLayout(i_pParent);
		pLayout->setContentsMargins(0, 0, 0, 0);
		pLayout->setSpacing(0);
		pLayout->addStretch(1);
		return pLayout;
	}

	//--------------------------------------------------------------------
	// convert clip to a driver clip type and 
	// get the driver pointer from it
	//--------------------------------------------------------------------
	tmlnDriver* get_driver_for_clip(chnlChannelClip* i_pClip)
	{
		// Cast clip to our driver clip type in order
		// to get the tmlnDriver
		chnlClipDriver* clip = dynamic_cast<chnlClipDriver*>(i_pClip);
		if (clip)
		{
			tmlnDriver* driver = &(clip->Driver());
			return driver;
		}
		return NULL;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	enum AlignType
	{
		e_NoMatch = -1,
		e_TimeTick = 0,
		e_Marker,
		e_CurrentTime
	};

	//--------------------------------------------------------------------
	// Align given time with important other times like markers and
	//	time ticks and the current time.
	//--------------------------------------------------------------------
	bool align_time(const maTime& i_Time, maTime &o_ClosestTime, AlignType& o_AlignType)
	{
		bool bMatch = false;

		maTime mtime;
		if (chnlTimeTickMgr::SnapTimeToTicks(i_Time, mtime))
		{
			bMatch = true;
			o_ClosestTime = mtime;
			o_AlignType = e_TimeTick;
		}
		if (chnlMarkerMgr::SnapTimeToMarkers(i_Time, mtime))
		{
			bMatch = true;
			o_ClosestTime = mtime;
			o_AlignType = e_Marker;
		}

		maTime curtime = tmlnTimeLine::GetValue();
		if (chnlSnapUtil::TimesMatch(i_Time, curtime))
		{
			bMatch = true;
			o_ClosestTime = curtime;
			o_AlignType = e_CurrentTime;
		}

		return bMatch;
	}

	//--------------------------------------------------------------------
	// Align given clip with important times
	//--------------------------------------------------------------------
	void align_moving_clip(chnlChannelClip* i_pClip)
	{
		AlignType align_type = e_NoMatch;
		maTime match_time;
		if (align_time(i_pClip->GetInteractStart(), match_time, align_type))
		{
			i_pClip->AlignInteractStart( match_time );
		}
		else if (align_time(i_pClip->GetInteractEnd(), match_time, align_type))
		{
			i_pClip->AlignInteractEnd( match_time );
		}
		
		switch (align_type)
		{
			default:
			case e_NoMatch:
				chnlTimelineGuideUtil::HideMarkerGuide();
				break;
			case e_TimeTick:
				chnlTimelineGuideUtil::ShowMarkerGuide(match_time, QColor(0,200,0));
				break;
			case e_Marker:
			case e_CurrentTime:
				chnlTimelineGuideUtil::ShowMarkerGuide(match_time, QColor(0,100,0));
				break;
		}
	}

	//--------------------------------------------------------------------
	// Align all selected clips in this control with important times
	//--------------------------------------------------------------------
	void align_selected_clips(chnlChannelControl* i_pChannelControl)
	{
		
		std::for_each(i_pChannelControl->GetSelectedClips().begin(),
					i_pChannelControl->GetSelectedClips().end(),
					align_moving_clip);
	}

} // end of namespace

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
chnlTraxDialog* chnlTraxDialog::FormInstance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlTraxDialog::chnlTraxDialog( QWidget* parent,
							  const char* title )
: QWidget( parent ),
	m_nFilterState(0),
	m_SnapInterval(c_DefaultSnapInterval),
	m_bDriverSelectStart(false),
	m_DriverSelectStartTime(0.0f),
	m_bFitZoomToWidth(true),
	m_bFittingZoom(false)
{
	QGridLayout* fgSizer_dialog = new QGridLayout( this );
	fgSizer_dialog->setColumnStretch( 1, 1 );
	fgSizer_dialog->setRowStretch( 1, 1 );

	// Upper left: zoom, display filter, snap, notes and markers
	QGridLayout* fgSizer_upperleft = new QGridLayout();
	fgSizer_upperleft->setColumnStretch( 0, 1 );

	QHBoxLayout* bSizer_zoom = new QHBoxLayout();
	m_slider_zoom = new tqcRangedFloat( this );
	m_slider_zoom->ShowValue( false );
	m_slider_zoom->SetMaximum( 500 );
	m_slider_zoom->SetValue( c_InitialZoom );
	m_slider_zoom->SetMinimum( 3 );
	m_slider_zoom->SetExponent( 2 );
	m_slider_zoom->setToolTip( "Zoom slider" );
	bSizer_zoom->addWidget( m_slider_zoom, 1 );
	QPushButton* pZoomCenter = create_button( this, "Z", "Fit time to Channel Editor size", 20, 20 );
	bSizer_zoom->addWidget( pZoomCenter );
	fgSizer_upperleft->addLayout( bSizer_zoom, 0, 0 );

	m_choice_Display = new QComboBox( this );
	m_choice_Display->addItem( "Multiple Selected" );
	m_choice_Display->addItem( "Selected" );
	m_choice_Display->addItem( "Similar" );
	m_choice_Display->addItem( "All" );
	m_choice_Display->setToolTip( "Display Filter" );
	m_choice_Display->setMinimumWidth( 120 );
	fgSizer_upperleft->addWidget( m_choice_Display, 0, 1 );

	m_checkBox_Snap = new QCheckBox( "Snap", this );
	fgSizer_upperleft->addWidget( m_checkBox_Snap, 1, 1, Qt::AlignRight );

	QVBoxLayout* bSizer2 = new QVBoxLayout();
	struct RowInfo { const char* m_pLabel; const char* m_pRemoveTip; const char* m_pAddTip; const char* m_pPrevTip; const char* m_pNextTip;
					 void (*m_Remove)(); void (*m_Add)(); void (*m_Prev)(); void (*m_Next)(); };
	const RowInfo rows[] =
	{
		{ "Notes", "Delete Note", "Add Note at current time", "Previous Note", "Next Note",
		  &chnlNotesOperations::DeleteNote, &chnlNotesOperations::AddNote, &chnlNotesOperations::MoveToPrevNote, &chnlNotesOperations::MoveToNextNote },
		{ "Markers", "Delete Marker", "Add Marker", "Previous Marker", "Next Marker",
		  &chnlMarkerOperations::DeleteMarker, &chnlMarkerOperations::AddMarker, &chnlMarkerOperations::MoveToPrevMarker, &chnlMarkerOperations::MoveToNextMarker },
	};
	for (const RowInfo& row : rows)
	{
		QHBoxLayout* pRow = new QHBoxLayout();
		pRow->setSpacing( 0 );
		pRow->addWidget( new QLabel( row.m_pLabel, this ), 1 );
		QPushButton* pRemove = create_button( this, "-", row.m_pRemoveTip, 16, 16 );
		QPushButton* pAdd = create_button( this, "+", row.m_pAddTip, 16, 16 );
		QPushButton* pPrev = create_button( this, "<", row.m_pPrevTip, 16, 16 );
		QPushButton* pNext = create_button( this, ">", row.m_pNextTip, 16, 16 );
		pRow->addWidget( pRemove );
		pRow->addWidget( pAdd );
		pRow->addSpacing( 4 );
		pRow->addWidget( pPrev );
		pRow->addWidget( pNext );
		bSizer2->addLayout( pRow );
		QObject::connect( pRemove, &QPushButton::clicked, row.m_Remove );
		QObject::connect( pAdd, &QPushButton::clicked, row.m_Add );
		QObject::connect( pPrev, &QPushButton::clicked, row.m_Prev );
		QObject::connect( pNext, &QPushButton::clicked, row.m_Next );
	}
	fgSizer_upperleft->addLayout( bSizer2, 1, 0 );

	// Driver conflict messages
	m_staticText2 = new QLabel( this );
	m_staticText2->setStyleSheet( "color: red" );
	fgSizer_upperleft->addWidget( m_staticText2, 2, 0, 1, 2 );

	fgSizer_dialog->addLayout( fgSizer_upperleft, 0, 0 );

	// Upper right: time labels, current time and markers. They scroll
	// horizontally with the channels.
	QWidget* pTimeline = new QWidget();
	QVBoxLayout* bSizer_timeline = create_stack_layout( pTimeline );
	m_timeLabel = new chnlTimeLabel( pTimeline );
	m_timeLabel->SetTimeScale( c_InitialZoom );
	bSizer_timeline->insertWidget( 0, m_timeLabel );
	m_timeSlider = new chnlTimeSlider( pTimeline );
	m_timeSlider->setAutoFillBackground( true );
	m_timeSlider->setStyleSheet( "background: white" );
	m_timeSlider->SetTimeScale( c_InitialZoom );
	bSizer_timeline->insertWidget( 1, m_timeSlider );
	m_markerBar = new chnlMarkerBar( pTimeline );
	m_markerBar->setStyleSheet( "background: white" );
	m_markerBar->SetTimeScale( c_InitialZoom );
	bSizer_timeline->insertWidget( 2, m_markerBar );

	m_scrollwin_timeline = create_scroll_area( this, pTimeline );
	m_scrollwin_timeline->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	m_scrollwin_timeline->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	m_scrollwin_timeline->setFrameShape( QFrame::NoFrame );
	m_scrollwin_timeline->setFixedHeight( 32 + 16 + 16 );
	fgSizer_dialog->addWidget( m_scrollwin_timeline, 0, 1, Qt::AlignBottom );

	// Lower left: driver buttons and channel names
	QHBoxLayout* bSizer_left = new QHBoxLayout();
	QVBoxLayout* bSizer_buttons = new QVBoxLayout();
	QHBoxLayout* bSizer16 = new QHBoxLayout();
	bSizer16->setSpacing( 0 );
	QPushButton* pPrevTick = create_button( this, "<-", "Move time to the previous tick", 25, 25 );
	QPushButton* pNextTick = create_button( this, "->", "Move time to the next tick", 25, 25 );
	bSizer16->addWidget( pPrevTick );
	bSizer16->addWidget( pNextTick );
	bSizer_buttons->addLayout( bSizer16 );
	QPushButton* pDelete = create_button( this, "Delete", "Delete selected driver", 50 );
	QPushButton* pCopy = create_button( this, "Copy", "Copy selected driver", 50 );
	QPushButton* pPaste = create_button( this, "Paste", "Paste copied driver at current time", 50 );
	QPushButton* pSplit = create_button( this, "Split", "Split selected driver at current time", 50 );
	QPushButton* pMove = create_button( this, "Move", "Move driver", 50 );
	QPushButton* pSel = create_button( this, "Sel->", "Select all drivers later in time of currently selected driver", 50 );
	for (QPushButton* pButton : { pDelete, pCopy, pPaste, pSplit, pMove, pSel })
		bSizer_buttons->addWidget( pButton );
	bSizer_buttons->addStretch( 1 );
	bSizer_left->addLayout( bSizer_buttons );

	QWidget* pNames = new QWidget();
	m_bSizer_names = create_stack_layout( pNames );
	m_scrollwin_names = create_scroll_area( this, pNames );
	m_scrollwin_names->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	m_scrollwin_names->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	m_scrollwin_names->setMinimumWidth( 120 );
	bSizer_left->addWidget( m_scrollwin_names, 1 );
	fgSizer_dialog->addLayout( bSizer_left, 1, 0 );

	// Lower right: the channels
	chnlTimelinePanel* pChannelPanel = new chnlTimelinePanel( NULL );
	m_pChannelPanel = pChannelPanel;
	m_bSizer_channels = create_stack_layout( pChannelPanel );
	m_scrollwin_channels = create_scroll_area( this, pChannelPanel );
	m_scrollwin_channels->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
	m_scrollwin_channels->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
	fgSizer_dialog->addWidget( m_scrollwin_channels, 1, 1 );

	// The timeline scrolls horizontally and the names vertically with the
	// channels, and scrolling the names (with the mouse wheel) scrolls the
	// channels too.
	QScrollBar* pChannelsH = m_scrollwin_channels->horizontalScrollBar();
	QScrollBar* pChannelsV = m_scrollwin_channels->verticalScrollBar();
	QScrollBar* pTimelineH = m_scrollwin_timeline->horizontalScrollBar();
	QScrollBar* pNamesV = m_scrollwin_names->verticalScrollBar();
	QObject::connect( pChannelsH, &QScrollBar::valueChanged, pTimelineH, &QScrollBar::setValue );
	QObject::connect( pTimelineH, &QScrollBar::valueChanged, pChannelsH, &QScrollBar::setValue );
	QObject::connect( pChannelsV, &QScrollBar::valueChanged, pNamesV, &QScrollBar::setValue );
	QObject::connect( pNamesV, &QScrollBar::valueChanged, pChannelsV, &QScrollBar::setValue );

	// Connect Events
	pChannelPanel->SetMouseCallbacks(
		[this](QMouseEvent* i_pEvent) { ChannelPanel_MouseDown(i_pEvent); },
		[this](QMouseEvent* i_pEvent) { ChannelPanel_MouseMove(i_pEvent); },
		[this](QMouseEvent* i_pEvent) { ChannelPanel_MouseUp(i_pEvent); } );
	m_slider_zoom->SetValueChangedCallback( [this]()
	{
		if (!m_bFittingZoom)
			m_bFitZoomToWidth = false;	// the user zoomed
		this->SetTimeScale( m_slider_zoom->GetValue() );
	} );
	m_timeSlider->SetValueChangedCallback( [this]() { current_time_changed(); } );
	QObject::connect( m_choice_Display, QOverload<int>::of(&QComboBox::activated), [this](int) { FilterIndexChanged(); } );
	QObject::connect( m_checkBox_Snap, &QCheckBox::clicked, [this]() { checkBox_Snap_Clicked(); } );
	QObject::connect( pZoomCenter, &QPushButton::clicked, [this]() { CalculateMaxZoom(); } );
	QObject::connect( pPrevTick, &QPushButton::clicked, []() { chnlOperations::MoveTimePrevTick(); } );
	QObject::connect( pNextTick, &QPushButton::clicked, []() { chnlOperations::MoveTimeNextTick(); } );
	QObject::connect( pDelete, &QPushButton::clicked, []() { chnlOperations::DeleteSelectedDrivers(); } );
	QObject::connect( pCopy, &QPushButton::clicked, []() { chnlOperations::CopyDrivers(); } );
	QObject::connect( pPaste, &QPushButton::clicked, []() { chnlOperations::PasteDrivers(); } );
	QObject::connect( pSplit, &QPushButton::clicked, []() { chnlOperations::SplitDrivers(); } );
	QObject::connect( pMove, &QPushButton::clicked, [this]() { button_Move_Click(); } );
	QObject::connect( pSel, &QPushButton::clicked, [this]()
	{
		std::for_each(m_Channels.begin(), m_Channels.end(),
			std::mem_fn(&chnlChannelControl::SelectClipsAfter));
	} );

	// Snapping is "on" by default
	this->SetSnapInterval((int)tmlnTimeLine::GetFPS());
	m_checkBox_Snap->setChecked(true);

	//	set the trax editor to the current timeline max
	this->SetTimeRange(tmlnTimeLine::GetMinimum(), tmlnTimeLine::GetMaximum());

	// set up the time formats to the current settings
	this->UpdateTimeLabel();

	// Add this panel to the pane manager
	tqtPaneMgr::AddPane(this,
		tqtPaneInfo().Name(title).Caption(title).Show().Layer(2).Bottom().MinimizeButton(true).BestSize(800, 300));

	// Register for hot keys
	tqtMessaging::WindowWantsHotKeys(this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlTraxDialog::~chnlTraxDialog()
{
	if (chnlTraxDialog::FormInstance == this)
		chnlTraxDialog::FormInstance = NULL;

	chnlOperations::ClearMonitors();
}

//--------------------------------------------------------------------
// Update channels based on selection
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateChannels()
{
	m_scrollwin_channels->setUpdatesEnabled(false);
	m_scrollwin_names->setUpdatesEnabled(false);
	setup_display(tmlnSelectionUtil::GetSelectedScriptObject());
	m_scrollwin_channels->setUpdatesEnabled(true);
	m_scrollwin_names->setUpdatesEnabled(true);

	center_view();

	resize_scrolled_windows();
}
void chnlTraxDialog::Select(tmlnScriptObject* i_pObject)
{
	m_scrollwin_channels->setUpdatesEnabled(false);
	m_scrollwin_names->setUpdatesEnabled(false);
	setup_display(i_pObject);
	m_scrollwin_channels->setUpdatesEnabled(true);
	m_scrollwin_names->setUpdatesEnabled(true);

	center_view();

	resize_scrolled_windows();
}

//--------------------------------------------------------------------
// Remove all channel  controls from form
//--------------------------------------------------------------------
void chnlTraxDialog::ClearChannels()
{
	chnlOperations::ClearMonitors();

	m_Channels.clear();

	clear_layout(m_bSizer_names);
	clear_layout(m_bSizer_channels);
	m_staticText2->setText("");

}

//--------------------------------------------------------------------
// SetTimeRange changes length of timeline controls, setting
//	 minimum and maximum time range.
//--------------------------------------------------------------------
void chnlTraxDialog::SetTimeRange(const maTime& i_MinTime, const maTime& i_MaxTime)
{
	//if (i_Time > 0)
	{
		//TIME - should the wxWidgets time controls use seconds or maTime?
		float min_time = i_MinTime.AsSeconds();
		float max_time = i_MaxTime.AsSeconds();
		this->m_timeLabel->SetTimeRange( min_time, max_time );
		this->m_timeSlider->SetTimeRange( min_time, max_time );
		this->m_markerBar->SetTimeRange( min_time, max_time );

		// Set total time for channels also
		std::vector<chnlChannelControl*>::iterator it;
		for (it = m_Channels.begin(); it != m_Channels.end(); ++it)
			(*it)->SetTimeRange( min_time, i_MaxTime.AsSeconds() );

		chnlTimelineGuideUtil::SetMinTime( i_MinTime );

		// Everytime range of timeline is changed (i.e. file load)
		// the timeline is scaled so that the full timeline is visible
		this->CalculateMaxZoom();

		resize_scrolled_windows();
	}
}

//----------------------------------------------------------------------------
// Zoom time scale so that full timeline is visible
//----------------------------------------------------------------------------
void chnlTraxDialog::CalculateMaxZoom()
{
	// Calculate a zoom value such that the full timeline is visible
	float totaltime = (tmlnTimeLine::GetMaximum() - tmlnTimeLine::GetMinimum()).AsSeconds();
	const int c_PanelEdgeSize = 50; // small amount of pixels to shrink to avoid scrollbars
	int panel_width = m_scrollwin_timeline->width() - c_PanelEdgeSize;
	
	float zoom = panel_width / totaltime;
	maFunctions::Clamp(zoom, m_slider_zoom->GetMinimum(), m_slider_zoom->GetMaximum());
	m_bFittingZoom = true;
	this->m_slider_zoom->SetValue( zoom );
	this->SetTimeScale( zoom );
	m_bFittingZoom = false;
	m_bFitZoomToWidth = true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual
void chnlTraxDialog::resizeEvent(QResizeEvent* i_pEvent)
{
	QWidget::resizeEvent(i_pEvent);

	// the timeline gets its new width once the layout has run
	if (m_bFitZoomToWidth && i_pEvent->size().width() != i_pEvent->oldSize().width())
	{
		QTimer::singleShot(0, this, [this]()
		{
			if (m_bFitZoomToWidth)
				this->CalculateMaxZoom();
		});
	}
}

//--------------------------------------------------------------------
//	Reset the scale.  This is important when the user changes the
//	global frame rate and the channel drivers need to resize
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateScale()
{
	float zoom = this->m_slider_zoom->GetValue();
	this->SetTimeScale( zoom );
}

float chnlTraxDialog::GetSliderZoom()
{
	return this->m_slider_zoom->GetValue();
}

void chnlTraxDialog::SetSliderZoom(float zoom)
{
	this->m_slider_zoom->SetValue(zoom);
}
//--------------------------------------------------------------------
// TimeScale changes the zoom left/right
//--------------------------------------------------------------------
void chnlTraxDialog::SetTimeScale(float i_Scale)
{
	float new_scale = i_Scale;

	//	adjust the value for the frame-rate
	//new_scale = new_scale * (g3dConstants::c_fDefaultFrameRate / 24.0f);

	if (new_scale > 0)
	{
		this->m_timeLabel->SetTimeScale( new_scale );
		this->m_timeSlider->SetTimeScale( new_scale );
		this->m_markerBar->SetTimeScale( new_scale );

		// Set time scale for channels also
		std::for_each(m_Channels.begin(), m_Channels.end(),
			std::bind(std::mem_fn(&chnlChannelControl::SetTimeScale),
						 std::placeholders::_1, new_scale));

		chnlTimelineGuideUtil::SetTimeScale( new_scale );
		m_pChannelPanel->update();

		//	adjust the timeline position based on the selected object or current time.
		//
		center_view();

		resize_scrolled_windows();
	}
}

//--------------------------------------------------------------------
// SetSnapInterval alters grid unit for moving drivers in channels
//--------------------------------------------------------------------
void chnlTraxDialog::SetSnapInterval(int i_Interval)
{
	m_SnapInterval = i_Interval;
	std::for_each(m_Channels.begin(), m_Channels.end(),
		std::bind(std::mem_fn(&chnlChannelControl::SetSnapInterval),
					 std::placeholders::_1, i_Interval));
}
//--------------------------------------------------------------------
//  This driver has changed its properties related to the
//	trax editor display, so update the channels related to it.
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateDriver(tmlnDriver *i_pDriver)
{
	std::vector<chnlChannelControl*>::iterator it;
	for (it = m_Channels.begin(); it != m_Channels.end(); ++it)
	{
		chnlChannelControl *pChannel = (*it);
		const int num_clips = pChannel->GetNumClips();
		for (int i=0; i<num_clips; ++i)
		{
			chnlClipDriver* pClip = dynamic_cast<chnlClipDriver*>(pChannel->GetClip(i));
			if (pClip)
			{
				if (&pClip->Driver() == i_pDriver)
				{
					pClip->Update();

					// invalidate the control in order to force a redraw soon
					pChannel->update();

					// Note, driver can occur in more than one channel, 
					// but only once per channel. So, one level "break" 
					// is used here instead of a "return"
					break;
				}
			}
		}
	}

}

//--------------------------------------------------------------------
// Time format has changed, alter display
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateTimeLabel()
{
	// Check the format in the timeline label
	m_timeLabel->SetFramesPerSecond( tmlnTimeLine::GetFPS() );

	bool bFrameTimeFormat = (tmlnTimeLine::GetTimeFormat() == tmlnTimeLine::e_Frames);
	m_timeLabel->SetDisplayFrames(bFrameTimeFormat);
	m_timeLabel->SetDisplayMinutes((tmlnTimeLine::GetTimeFormat() == tmlnTimeLine::e_HHMMSSFR)
								 || (tmlnTimeLine::GetTimeFormat() == tmlnTimeLine::e_HHMMSSMS));

	//	update the title of the panel with the time string
	//
	int frames;
	tmlnTimeUtil::GetTimeInFrames(tmlnTimeLine::GetValue(), frames);

	std::ostringstream str;
	if (bFrameTimeFormat)
	{
		// Just display frames
		str << "[" << frames << "]";
	}
	else
	{
		// Display time format and frames for convenience
		std::string time_string;
		tmlnTimeUtil::GetTimeString(tmlnTimeLine::GetValue(), time_string);
		str << time_string << "[" << frames << "]";
	}
	tqtPaneMgr::SetCaption( this, str.str() );
	
	// Check the snap interval, if it is attached to the current FPS
	if (m_checkBox_Snap->isChecked())
		this->SetSnapInterval((int)tmlnTimeLine::GetFPS());
}

//--------------------------------------------------------------------
// Timeline time has changed, alter display
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateCurrentTime()
{
	// set the time
	this->m_timeSlider->SetCurrentTime( tmlnTimeLine::GetValue().AsSeconds() );

	//	update the Time GuideLine
	chnlTimelineGuideUtil::SetCurrentTime( tmlnTimeLine::GetValue() );
	m_pChannelPanel->update();

	UpdateTimeLabel();
}

//--------------------------------------------------------------------
// Look for time ticks based on begin and end times of drivers
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateTimeTicks()
{
	this->m_timeSlider->ClearTimeTicks();

	const std::set<maTime>& time_ticks = chnlTimeTickMgr::GetTimeTicks();
	std::set<maTime>::const_iterator it;
	for (it = time_ticks.begin(); it != time_ticks.end(); ++it)
	{
		this->m_timeSlider->AddTimeTick(it->AsSeconds());	//TIME - time ticks in GUI should be seconds?
	}
}

//--------------------------------------------------------------------
// Update marker bar's display of markers and notes
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateMarkersAndNotes()
{
	this->m_markerBar->ClearMarkers();
	this->m_markerBar->ClearNotes();

	//	Markers
	for (int i=0; i < chnlMarkerMgr::GetNumMarkers(); ++i)
	{
		const chnlMarkerDataItem& mdata = chnlMarkerMgr::GetMarkerData(i);
		this->m_markerBar->AddMarker( mdata.m_Time.GetValue().AsSeconds(), 
									(chnlMarkerIcon::MarkerType) mdata.m_TimeMarkerType.GetValue(),
									 mdata.m_Note.GetValue() );
	}

	//	Notes
	for (int i=0; i < chnlNotesMgr::GetNumNotes(); ++i)
	{
		const chnlNoteDataItem& ndata = chnlNotesMgr::GetNoteData(i);
		this->m_markerBar->AddNote( ndata.m_Time.GetValue().AsSeconds(), 
									 (chnlNoteIcon::NoteStatus) ndata.m_Status.GetValue(),
									 ndata.m_Note.GetValue().c_str() );
	}
}

//------------------------------------------------------------------------
// Get set of drivers that are selected in the trax editor. If skip locked
// is true, then only drivers in channels that are unlocked will be returned.
//------------------------------------------------------------------------
void chnlTraxDialog::GetSelectedDrivers(std::set<tmlnDriver*> &o_Drivers, 
										bool i_bSkipLocked)
{
	std::vector<chnlChannelControl*>::iterator it;
	for (it = m_Channels.begin(); it != m_Channels.end(); ++it)
	{
		chnlChannelControl *pChannel = (*it);

		//	don't allow this interaction if the channel is locked
		if (!i_bSkipLocked || !pChannel->GetLocked())
		{
			std::transform(pChannel->GetSelectedClips().begin(), 
							pChannel->GetSelectedClips().end(),
							std::inserter(o_Drivers, o_Drivers.begin()),
							get_driver_for_clip);
		}				
	}
}

//------------------------------------------------------------------------
// Deselect all drivers in the channel interface
//------------------------------------------------------------------------
void chnlTraxDialog::ClearSelection()
{
	std::for_each(m_Channels.begin(), m_Channels.end(), 
		std::mem_fn(&chnlChannelControl::ClearSelection));
}

//------------------------------------------------------------------------
// Select driver in the channel interface - this will do an append
//	to the selection, it will not deselect other drivers.
//------------------------------------------------------------------------
void chnlTraxDialog::AddToSelection( tmlnDriver* i_pDriver )
{
	const int num_channels = m_Channels.size();
	for (int i=0; i<num_channels; i++)
	{	
		for (int c=0; c<m_Channels[i]->GetNumClips(); c++)
		{
			chnlClipDriver* clip = dynamic_cast<chnlClipDriver*>(m_Channels[i]->GetClip(c));
			if (clip)
			{
				if (&clip->Driver() == i_pDriver)
				{
					const bool append_selection = true;
					m_Channels[i]->SelectClip(clip, append_selection);
				}
			}
		}
	}
}

//--------------------------------------------------------------------
// Interaction Callbacks from channel controls
//--------------------------------------------------------------------
//virtual 
void chnlTraxDialog::ClipSelected(chnlChannelControl* i_pChannel, chnlChannelClip* i_pClip, bool i_bAppended)
{
	tmlnDriver *pEditingDriver = NULL;
	if (i_pClip)
	{
		// Got selected clip
		chnlClipDriver* clip = dynamic_cast<chnlClipDriver*>(i_pClip);
		if (clip)
			pEditingDriver = (&clip->Driver());
	}

	const int num_channels = m_Channels.size();
	// If we are clearing the selection, or not appending to 
	// the selection, then clear selection from other channels
	if (i_pClip == NULL || i_bAppended == false)
	{
		for (int i=0; i<num_channels; i++)
		{	
			if (m_Channels[i] != i_pChannel)
				m_Channels[i]->ClearSelection();
		}
		
		//	Clear out the driver dialog, if needed
		tmlnDriverDialogUtil::PrepareForSelection(pEditingDriver);
	}

	// Now, look for all other occurrences of this driver on other channels
	if (i_pClip != NULL)
	{
		chnlClipDriver* sel_clip = dynamic_cast<chnlClipDriver*>(i_pClip);
		if (sel_clip)
		{
			tmlnDriver *pSelDriver = &sel_clip->Driver();
			for (int i=0; i<num_channels; i++)
			{	
				if (m_Channels[i] != i_pChannel)
				{
					for (int c=0; c<m_Channels[i]->GetNumClips(); c++)
					{
						chnlClipDriver* clip = dynamic_cast<chnlClipDriver*>(m_Channels[i]->GetClip(c));
						if (clip)
						{
							if (&clip->Driver() == pSelDriver)
							{
								const bool append_selection = true;
								m_Channels[i]->SelectClip(clip, append_selection);
							}
						}
					}
				}
			}
		}
	}


	// Prepare all selected clips in all channels for the interaction
	std::for_each(m_Channels.begin(), m_Channels.end(), 
					std::mem_fn(&chnlChannelControl::BeginInteraction));

	// Update the properties dialog for the selected driver
	// (Should this merge all driver properties together like with objects?)
	if (pEditingDriver && guiDialogTabbedMgr::IsVisible("Driver"))
	{
		// Got selected clip
		pEditingDriver->DoEditProperties();
	}
}
void chnlTraxDialog::ClipDeselected(chnlChannelControl* i_pChannel, chnlChannelClip* i_pClip)
{
	const int num_channels = m_Channels.size();

	// Deselect all occurrences of this driver on other channels
	if (i_pClip != NULL)
	{
		chnlClipDriver* sel_clip = dynamic_cast<chnlClipDriver*>(i_pClip);
		if (sel_clip)
		{
			tmlnDriver *pSelDriver = &sel_clip->Driver();
			for (int i=0; i<num_channels; i++)
			{	
				if (m_Channels[i] != i_pChannel)
				{
					for (int c=0; c<m_Channels[i]->GetNumClips(); c++)
					{
						chnlClipDriver* clip = dynamic_cast<chnlClipDriver*>(m_Channels[i]->GetClip(c));
						if (clip)
						{
							if (&clip->Driver() == pSelDriver)
							{
								m_Channels[i]->DeselectClip(clip);
							}
						}
					}
				}
			}
		}
	}

	//	Clear out the driver dialog
	tmlnDriverDialogUtil::ClearDriverProperties();		
}
//virtual 
void chnlTraxDialog::ClipMoved(chnlChannelControl*, float i_TimeDelta)
{
	//TIME - wxGUI time controls could use maTime
	maTime time_delta = maTime::FromSeconds( i_TimeDelta );

	//bool canBemoved = true;
	const int num_channels = m_Channels.size();
	std::vector<chnlChannelControl*> m_SelectedChannel;
	int num_conflicts = 0;
	for (int i=0; i<num_channels; i++)
	{
		
		std::set<chnlChannelClip*> selectedClips = m_Channels[i]->GetSelectedClips();
		if(selectedClips.size() > 0) m_SelectedChannel.push_back(m_Channels[i]);
		std::set<chnlChannelClip*>::iterator it;
		for (it = selectedClips.begin(); it!= selectedClips.end(); ++it )
		{
			chnlChannelClip* clip = (*it);
			if((clip->GetBeginTime() + time_delta )  < tmlnTimeLine::GetMinimum()
				|| (clip->GetBeginTime() + time_delta) > tmlnTimeLine::GetMaximum())
			return;
		}
	}

	for( int cha = 0; cha < m_Channels.size(); cha ++ )
	{
		std::set<chnlChannelClip*> selectedClips = m_Channels[cha]->GetSelectedClips();
		std::set<chnlChannelClip*>::iterator it;
		for (it = selectedClips.begin(); it!= selectedClips.end(); ++it )
		{
			chnlChannelClip * clip = (*it);
			num_conflicts += check_channel_for_driver_conflicts(m_Channels[cha],clip, time_delta );
		}
	}
	
	show_driver_conflict_error(num_conflicts);
	for(int m = 0 ; m < m_SelectedChannel.size() ; m++)
	{
		if(m_SelectedChannel[m]->GetLocked())
		{
			return; // Even if one channel is locked, abstain from moving any other channels
		}
	}
	// Slide all selected clips in all channels for the interaction
	std::for_each( m_Channels.begin(), m_Channels.end(), 
		std::bind(std::mem_fn(&chnlChannelControl::InteractMoveSelectedClips),
					 std::placeholders::_1, i_TimeDelta));
	

	// Adjust the selected clips to align with time ticks and markers
	if (PrefsMgr::Data().m_ChannelEditor_SnapActive.GetValue())
	{
		std::for_each(m_Channels.begin(), m_Channels.end(), align_selected_clips);
		m_pChannelPanel->update();
	}
}
//virtual 
void chnlTraxDialog::ClipMoveFinished(chnlChannelControl*)
{
	int num_conflicts = check_channel_for_driver_conflicts();
	show_driver_conflict_error(num_conflicts);
	
	// End interaction in all channels
	std::for_each(m_Channels.begin(), m_Channels.end(), 
					std::mem_fn(&chnlChannelControl::FinishInteraction));
	chnlTimelineGuideUtil::HideMarkerGuide();
	m_pChannelPanel->update();

	
}
//virtual 
void chnlTraxDialog::ClipResized(chnlChannelControl*)
{
	
}

//----------------------------------------------------------------------------
// Dragging on the channel panel selects the clips within the dragged
//	time span. The panel's coordinates already include the scrolling.
//----------------------------------------------------------------------------
void chnlTraxDialog::ChannelPanel_MouseDown( QMouseEvent* i_pEvent )
{
	const QPoint pos = i_pEvent->position().toPoint();
	m_bDriverSelectStart = true;
	m_DriverSelectStartTime = get_time_from_point( pos.x(), pos.y() );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::ChannelPanel_MouseMove( QMouseEvent* i_pEvent )
{
	if (m_bDriverSelectStart)
	{
		const QPoint pos = i_pEvent->position().toPoint();
		bool shiftIsPressed = (i_pEvent->modifiers() & Qt::ShiftModifier) != 0;
		float curr_time = get_time_from_point( pos.x(), pos.y() );
		select_clips_within_time( m_DriverSelectStartTime, curr_time, shiftIsPressed );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::ChannelPanel_MouseUp( QMouseEvent* )
{
	m_bDriverSelectStart = false;
	this->update();
}

//------------------------------------------------------------------------
// combo box of filters changed
//------------------------------------------------------------------------
void chnlTraxDialog::FilterIndexChanged()
{
	this->m_nFilterState = this->m_choice_Display->currentIndex();
	this->UpdateChannels();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::checkBox_Snap_Clicked()
{
	 if (this->m_checkBox_Snap->isChecked())
		 this->SetSnapInterval((int)tmlnTimeLine::GetFPS());
	 else
		 this->SetSnapInterval(c_DefaultSnapInterval);
}

//--------------------------------------------------------------------
// Add category label to form
//--------------------------------------------------------------------
void chnlTraxDialog::add_category(const itString& i_CategoryName)
{
	QLabel *pCategory = new QLabel( QString::fromWCharArray(i_CategoryName.GetString()) );
	pCategory->setAlignment( Qt::AlignCenter );
	QFont font = pCategory->font();
	font.setPointSize( 8 );
	font.setBold( true );
	pCategory->setFont( font );
	pCategory->setFixedHeight( 20 );
	pCategory->setAutoFillBackground( true );
	QPalette palette = pCategory->palette();
	palette.setColor( QPalette::Window, palette.color(QPalette::Highlight) );
	palette.setColor( QPalette::WindowText, palette.color(QPalette::HighlightedText) );
	pCategory->setPalette( palette );
	m_bSizer_names->insertWidget( m_bSizer_names->count()-1, pCategory );

	// Add spacer on channel side of controls
	QWidget* pSpacer = new QWidget();
	pSpacer->setFixedHeight( 20 );
	m_bSizer_channels->insertWidget( m_bSizer_channels->count()-1, pSpacer );
}

//--------------------------------------------------------------------
// Add channel control and label to form
//--------------------------------------------------------------------
chnlChannelControl* chnlTraxDialog::add_channel(tmlnChannel& i_Channel)
{
	// Compute an alternating color for the channels
	bool channel_alt = (m_Channels.size() % 2 == 1);
	const QColor bg_color = (channel_alt) ? QColor(250, 245, 230) : QColor(255, 255, 255);

	// Add channel control to main scrolled window in lower right
	chnlChannelControl *pChannelCtrl = new chnlChannelControl( NULL, i_Channel.IsLocked() );
	pChannelCtrl->setAutoFillBackground( true );
	QPalette palette = pChannelCtrl->palette();
	palette.setColor( QPalette::Window, bg_color );
	pChannelCtrl->setPalette( palette );
	m_bSizer_channels->insertWidget( m_bSizer_channels->count()-1, pChannelCtrl, 0, Qt::AlignLeft );
	pChannelCtrl->SetTimeScale( this->m_timeLabel->GetTimeScale() );
	pChannelCtrl->SetTimeRange(	m_timeLabel->GetMinTime(), m_timeLabel->GetMaxTime() );
	pChannelCtrl->SetSnapInterval( this->m_SnapInterval );

	pChannelCtrl->SetInteractionCallback(this);
	m_Channels.push_back(pChannelCtrl);

	// Panel holds spacer, channel name label, button for locked state,
	// as high as the channel control
	QWidget* pNamePanel = new QWidget();
	pNamePanel->setAutoFillBackground( true );
	pNamePanel->setPalette( palette );
	pNamePanel->setFixedHeight( pChannelCtrl->height() );
	QHBoxLayout* bSizer = new QHBoxLayout( pNamePanel );
	bSizer->setContentsMargins( 10, 0, 5, 0 );

	QLabel *pLabel = new QLabel( tqtQString(i_Channel.GetName()), pNamePanel );
	QFont font = pLabel->font();
	font.setPointSize( 8 );
	font.setBold( true );
	pLabel->setFont( font );
	bSizer->addWidget( pLabel, 1 );

	bSizer->addWidget( create_lock_button( pNamePanel, i_Channel, pChannelCtrl ) );

	// Add to layout in scrolled window on left
	m_bSizer_names->insertWidget( m_bSizer_names->count()-1, pNamePanel );

	return pChannelCtrl;
}

//----------------------------------------------------------------------------
// The scrolled windows follow the sizes of their contents
//----------------------------------------------------------------------------
void chnlTraxDialog::resize_scrolled_windows()
{
	m_scrollwin_timeline->widget()->adjustSize();
	m_pChannelPanel->updateGeometry();
	m_scrollwin_names->widget()->updateGeometry();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::center_view()
{
	// could center on current time or on selected clips
	center_view_on_currenttime();
}

//----------------------------------------------------------------------------
//	adjust the timeline position based on the current time.
//----------------------------------------------------------------------------
void chnlTraxDialog::center_view_on_currenttime()
{
	int time_pos = this->m_timeSlider->GetCurrentTimePosition();
	int width = m_scrollwin_channels->viewport()->width();

	int centered_pos = time_pos - (width / 2);
	if (centered_pos < 0) centered_pos = 0;

	m_scrollwin_channels->horizontalScrollBar()->setValue(centered_pos);
}

//----------------------------------------------------------------------------
// Calculate the time based on the point on the channel panel
//----------------------------------------------------------------------------
float chnlTraxDialog::get_time_from_point(int i_CurrentX, int i_CurrentY)
{
	return m_timeSlider->GetTimeAtPosition( i_CurrentX );
}

void chnlTraxDialog::button_Move_Click()
{
	if (any_drivers_selected())
	{
		prtyFloat moveAmount(tmlnTimeUtil::GetTimeFormatString(), 0.0f);
		moveAmount.AddCallback(new prtyCallbackWrapper<chnlTraxDialog>(this, &chnlTraxDialog::MoveAmountChanged));

		prtyPropertyUIInfoContainer moveInfo;
		moveInfo.Add(new tmlnTimeEditUIInfo(&moveAmount));

		// Start interaction, preparing the selected drivers to move delta amounts
		std::for_each(m_Channels.begin(), m_Channels.end(), 
			std::mem_fn(&chnlChannelControl::BeginInteraction));

		if (guiPropertyDialog::ShowModal("Move Drivers", 
										 moveInfo, 
										 "Time amount to move drivers:") != guiPropertyDialog::e_OK)
		{
			// If the user pressed "Cancel", then put the drivers back.
			// Our property callback is still active, so all we have to do is set 
			// the move amount to 0
			moveAmount.SetValue(0.0f); 
		}
		
		// Finish interaction, setting deltas into drivers
		std::for_each(m_Channels.begin(), m_Channels.end(), 
			std::mem_fn(&chnlChannelControl::FinishInteraction));
	}
}
//--------------------------------------------------------------------
// Move driver dialog's time amount changes, set the
// delta amount into the drivers.
//--------------------------------------------------------------------
void chnlTraxDialog::MoveAmountChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	prtyFloat *pMoveAmount = dynamic_cast<prtyFloat*>(i_pProperty);
	DBG_ASSERT(pMoveAmount, "Move amount callback should be a float property");
	 
	std::for_each(m_Channels.begin(), m_Channels.end(),
		std::bind(std::mem_fn(&chnlChannelControl::InteractMoveSelectedClips),
					 std::placeholders::_1, pMoveAmount->GetValue()));
}

//----------------------------------------------------------------------------
// Display channels for objects based on the filter state
//----------------------------------------------------------------------------
void chnlTraxDialog::setup_display(tmlnScriptObject* i_pObject)
 {
	switch (m_nFilterState)
	{
		case 0:
			// Show all selected objects
			multiple_selected_display();
			break;
		case 1:
			// Single selected object display
			single_object_display(i_pObject);
			break;
		case 2:
			// Show system objects
			system_object_display(i_pObject);
			break;
		case 3:
			// Show all objects
			all_object_display();
			break;
	}
}

//----------------------------------------------------------------------------
// Display channels for all selected objects
//----------------------------------------------------------------------------
void chnlTraxDialog::multiple_selected_display()
{
	this->ClearChannels();
	
	std::vector<tmlnScriptObject*> objects;
	tmlnSelectionUtil::GetSelectedScriptObjects(objects);

	for (int i=0; i < objects.size(); i++)
	{
		add_channels_for_object(objects[i]);
	}

	//this->labelNoChannels->Visible = (objects.empty());

	check_channel_for_driver_conflicts();
	chnlTimeTickMgr::GatherTimeTicks(objects);
	this->UpdateTimeTicks();
}

//----------------------------------------------------------------------------
// Display channels for the selected object only
//----------------------------------------------------------------------------
void chnlTraxDialog::single_object_display(tmlnScriptObject* i_pObject)
{
	this->ClearChannels();
	

	add_channels_for_object(i_pObject);
	check_channel_for_driver_conflicts();
	//this->labelNoChannels->Visible = (i_pObject == NULL);

	chnlTimeTickMgr::GatherTimeTicks(i_pObject);
	this->UpdateTimeTicks();
}

//----------------------------------------------------------------------------
// Display channels for all objects of the same type as the given object
//----------------------------------------------------------------------------
void chnlTraxDialog::system_object_display(tmlnScriptObject* i_pObject)
{
	this->ClearChannels();

	std::vector<tmlnScriptObject*> objects;

	if (i_pObject != NULL)
	{
		tmlnTimelineMgr::GetObjects( objects, tmlnTimelineMgr::GetObjectCategory(i_pObject) );
	}

	for (int i=0; i < objects.size(); i++)
	{
		// In "system" display, only add objects with drivers
		if (objects[i]->GetNumDrivers() > 0)
			add_channels_for_object(objects[i]);
	}

	//this->labelNoChannels->Visible = (objects.empty());

	chnlTimeTickMgr::GatherTimeTicks(objects);
	this->UpdateTimeTicks();
}

//----------------------------------------------------------------------------
// Display channels for all script objects
//----------------------------------------------------------------------------
void chnlTraxDialog::all_object_display()
{
	this->ClearChannels();

	std::vector<tmlnScriptObject*> objects;
	tmlnTimelineMgr::GetObjects( objects );
	for (int i=0; i < objects.size(); i++)
	{
		// In "all object" display, only add objects with drivers
		if (objects[i]->GetNumDrivers() > 0)
			add_channels_for_object(objects[i]);
	}

	//this->labelNoChannels->Visible = (objects.empty());

	chnlTimeTickMgr::GatherTimeTicks(objects);
	this->UpdateTimeTicks();
}
 
//----------------------------------------------------------------------------
// Add channels for the given object to our display
//----------------------------------------------------------------------------
void chnlTraxDialog::add_channels_for_object(tmlnScriptObject* i_pObject)
{
	if (!i_pObject) return;

	// Add Category based on object's name
	this->add_category(itString(i_pObject->GetDisplayName().c_str()));

	// Add Channels for object that have a driver ONLY
	//
	tmlnChannelSet& chnl_set = i_pObject->ChannelSet();
	int num_channels = chnl_set.GetNumChannels();
	for (int i=0; i<num_channels; i++)
	{
		tmlnChannel& channel = chnl_set.Channel(i);

		//	if drivers for channel, then add it.
		//
		if ( channel.GetNumDrivers() > 0 )
		{
			chnlChannelControl *pChannelCtrl = this->add_channel(channel);
				//this->add_channel(channel.GetName(), channel.IsLocked() );
			//pChannelCtrl->MultiChannel = channel.IsMultiChannel();
			
			for (int c=0; c<channel.GetNumDrivers(); c++)
			{
				tmlnDriver& driver = channel.Driver(c);

				chnlClipDriver *pClip = new chnlClipDriver(	driver,
															driver.GetName(),
															driver.GetBeginTime(), 
															driver.GetEndTime() );
				//pClip->Category = gcnew System::String(driver.GetCategory().c_str());

				maFloatRGBA color = driver.GetClipFillColor();
				pClip->SetFillColor(color.GetRed(), 
									color.GetGreen(), 
									color.GetBlue());

				pChannelCtrl->AddClip( pClip );

				chnlOperations::MonitorChanges(driver);
			}
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::current_time_changed()
 {
	float time = this->m_timeSlider->GetCurrentTime();
	//float origtime = time;

	tmlnTimeUtil::AdjustTimeToFrame( time );
	//if (time != origtime)

	// the "Deferred" call will set the timeline after the
	// current render thread finishes
	tmlnTimeLine::SetTimeDeferred( maTime::FromSeconds(time) );
 }

//----------------------------------------------------------------------------
// Return true if any drivers are selected in any channels
//----------------------------------------------------------------------------
bool chnlTraxDialog::any_drivers_selected()
{
	std::vector<chnlChannelControl*>::iterator it;
	for (it = m_Channels.begin(); it != m_Channels.end(); ++it)
	{
		chnlChannelControl *pChannel = (*it);
		if (!pChannel->GetSelectedClips().empty())
			return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::select_clips_within_time( float i_StartTime, float i_CurrentTime, bool shiftIsPressed )
{
	//	loop through the channels and driver clips and select all the ones that fall within the "box"
	//
	const int num_channels = m_Channels.size();
	float begin_time, end_time;
	if(!shiftIsPressed)
	{
		for (int i=0; i<num_channels; i++)
		{	
			m_Channels[i]->ClearSelection();
		}
	}

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

	
	for (int i=0; i<num_channels; i++)
	{	
		m_Channels[i]->SelectClips( begin_time, end_time );
	}
}
//----------------------------------------------------------------------------
//Driver Conflict functions
//----------------------------------------------------------------------------

void chnlTraxDialog::show_driver_conflict_error(int i_NumConflicts)
{
	if(i_NumConflicts ==0)
	{
		m_staticText2->setText("");
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool chnlTraxDialog::check_driver_conflicts(chnlChannelClip* i_pClip1, const maTime& i_Delta, chnlChannelClip* i_pClip2, chnlChannelControl* i_pChannel ) const
{
	if(i_pClip1 == i_pClip2)
	{
		return false;
	}
	std::ostringstream oss;
	bool Overlaps = false;
	
	std::string driverName = i_pClip1->GetName();
	
	maTime c1s = i_pClip1->GetBeginTime()  + i_Delta;
	maTime c1e = i_pClip1->GetEndTime()  + i_Delta;
	maTime c2s = i_pClip2->GetBeginTime();
	maTime c2e = i_pClip2->GetEndTime();
	
	std::string time_str;
	if  ((c2s <= c1s) && (c2e > c1s))		// clip 2 start inside
	{
		tmlnTimeUtil::GetTimeString(c1s, time_str);
		oss << "Driver \""<< driverName << "\" conflicts at " << time_str;
		Overlaps = true;
	}
	else if ((c2s < c1e) && (c2e > c1e))		// clip 2 end inside
	{
		tmlnTimeUtil::GetTimeString(c1e, time_str);
		oss << "Driver \""<< driverName << "\" conflicts at " << time_str;
		Overlaps =  true;
	}
	else if ((c1s < c2s) && (c1e > c2s))		// clip 1 start inside
	{
		tmlnTimeUtil::GetTimeString(c2s, time_str);
		oss << "Driver \""<< driverName << "\" conflicts at " << time_str;
		Overlaps =  true;
	}
	else if ((c1s < c2e) && (c1e > c2e))   	// clip 1 end inside
	{
		tmlnTimeUtil::GetTimeString(c1s, time_str);
		oss << "Driver \""<< driverName << "\" conflicts at " << time_str;
		Overlaps = true;
	}
	else if ((c1s == c2s) && ( c1e == c2e))  // keys overlap
	{
		tmlnTimeUtil::GetTimeString(c1s, time_str);
		oss << "Driver \""<< driverName << "\" conflicts at " << time_str;
		Overlaps =  true;
	}
	

	if(Overlaps)
	{
		m_staticText2->setText( tqtQString(oss.str()) );
		DBG_LOG(oss.str().c_str());
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int chnlTraxDialog::check_channel_for_driver_conflicts(chnlChannelControl *i_pChannel, chnlChannelClip* i_pClip, const maTime& i_Delta  ) const
{
	int clips_count = i_pChannel->GetNumClips();
	int num_conflicts = 0;
	for (int j=0; j < clips_count; j++)
	{
		chnlChannelClip* clip2 = dynamic_cast<chnlChannelClip*>(i_pChannel->GetClip(j));
		if(check_driver_conflicts(i_pClip, i_Delta, clip2, i_pChannel))
		{
			++num_conflicts;
		}
	}
	return num_conflicts;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int chnlTraxDialog::check_channel_for_driver_conflicts(chnlChannelControl* i_pChannel, const maTime& i_Delta) const
{
	int clips_count = i_pChannel->GetNumClips(); 
	int num_conflicts = 0;
	for (int i=0; i < clips_count; ++i)
	{
		chnlChannelClip* clip1 = dynamic_cast<chnlChannelClip*>(i_pChannel->GetClip(i));
		for (int j=i+1; j < clips_count; ++j)
		{
			chnlChannelClip* clip2 = dynamic_cast<chnlChannelClip*>(i_pChannel->GetClip(j));
			if(check_driver_conflicts(clip1, i_Delta, clip2, i_pChannel))
			{
				++num_conflicts;
			}
		}
	}
	return num_conflicts;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int chnlTraxDialog::check_channel_for_driver_conflicts() const
{
	int num_conflicts = 0;
	for(int i=0; i < m_Channels.size();i++)
	{
		chnlChannelControl* channel = dynamic_cast<chnlChannelControl*>(m_Channels[i]);
		num_conflicts +=check_channel_for_driver_conflicts(channel, maTime::c_ZeroTime);
	}
	return num_conflicts;
}
		

#endif // USE_QT
