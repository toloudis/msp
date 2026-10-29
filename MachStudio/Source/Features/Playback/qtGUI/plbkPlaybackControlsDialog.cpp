/*****************************************************************************
**	plbkPlaybackControlsDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Playback/qtGUI/plbkPlaybackControlsDialog.hpp"

#include "Features/Playback/plbkPackage.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"

#ifdef USE_QT

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QToolButton>
#include <QVBoxLayout>

//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	// Icon directory, with a trailing separator
	//--------------------------------------------------------------------
	QString get_icon_directory()
	{
		std::string wide_dir;
		fsFileUtil::LocatorToUnicodeString(guiMenuMgr::GetIconDirectory(), wide_dir);
		return QString::fromStdString(wide_dir) + "\\";
	}

	//--------------------------------------------------------------------
	// Image button with normal, hover and pressed images
	//--------------------------------------------------------------------
	QToolButton* create_image_button(QWidget* i_pParent, const QString& i_IconDir,
									 const char* i_pName, const char* i_pToolTip)
	{
		const QString base = i_IconDir + i_pName;
		QIcon icon(base + ".png");
		icon.addPixmap(QPixmap(base + "_highlight.PNG"), QIcon::Active);
		icon.addPixmap(QPixmap(base + "_pressed.PNG"), QIcon::Selected);

		QToolButton* pButton = new QToolButton(i_pParent);
		pButton->setIcon(icon);
		pButton->setIconSize(QSize(30, 30));
		pButton->setAutoRaise(true);
		pButton->setToolTip(i_pToolTip);
		return pButton;
	}
}

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
plbkPlaybackControlsDialog* plbkPlaybackControlsDialog::DialogInstance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
plbkPlaybackControlsDialog::plbkPlaybackControlsDialog( QWidget* parent,
														const char* title )
:	QWidget( parent )
{
	const QString icon_dir = get_icon_directory();

	QVBoxLayout* bSizer_panel = new QVBoxLayout( this );

	// Transport buttons: each one switches to the playback mode and
	// runs one of its operations.
	QHBoxLayout* pButtons = new QHBoxLayout();
	pButtons->setSpacing( 3 );
	struct ButtonInfo { const char* m_pIcon; const char* m_pToolTip; void (plbkModePlayback::*m_Op)(); };
	const ButtonInfo buttons[] =
	{
		{ "playback-begin",		"Move to beginning",	&plbkModePlayback::GoBegin },
		{ "playback-fastrev",	"Fast rewind",			&plbkModePlayback::FastRev },
		{ "playback-playrev",	"Play Reverse",			&plbkModePlayback::PlayRev },
		{ "playback-pause",		"Pause Playback",		NULL },
		{ "playback-playfwd",	"Play Forward",			&plbkModePlayback::Play },
		{ "playback-fastfwd",	"Fast forward",			&plbkModePlayback::FastFwd },
		{ "playback-end",		"Move to the end",		&plbkModePlayback::GoEnd },
	};
	for (const ButtonInfo& info : buttons)
	{
		QToolButton* pButton = create_image_button( this, icon_dir, info.m_pIcon, info.m_pToolTip );
		pButtons->addWidget( pButton );
		if (info.m_Op)
		{
			void (plbkModePlayback::*op)() = info.m_Op;
			QObject::connect( pButton, &QToolButton::clicked, [this, op]()
			{
				set_playbackmode();
				plbkModePlayback* pMode = get_playbackmode();
				(pMode->*op)();
			} );
		}
		else
		{
			// Pause leaves the playback mode
			QObject::connect( pButton, &QToolButton::clicked, [this]()
			{
				if (is_playbackmode())
				{
					get_playbackmode()->Pause();
					close_playbackmode();
				}
			} );
		}
	}
	bSizer_panel->addLayout( pButtons );

	QHBoxLayout* bSizer_Settings = new QHBoxLayout();
	m_checkBox_loop = new QCheckBox( "Loop", this );
	m_checkBox_loop->setToolTip( "Loop" );
	bSizer_Settings->addWidget( m_checkBox_loop );
	m_checkBox_lowres = new QCheckBox( "Low-Res", this );
	m_checkBox_lowres->setToolTip( "Low-Res Playback" );
	bSizer_Settings->addWidget( m_checkBox_lowres );
	m_checkBox_mute = new QCheckBox( "Mute", this );
	m_checkBox_mute->setToolTip( "Mute" );
	bSizer_Settings->addWidget( m_checkBox_mute );
	bSizer_panel->addLayout( bSizer_Settings );
	bSizer_panel->setAlignment( bSizer_Settings, Qt::AlignHCenter );

	QHBoxLayout* bSizer4 = new QHBoxLayout();
	m_slider_SloMo = new QSlider( Qt::Horizontal, this );
	m_slider_SloMo->setRange( 0, 100 );
	m_slider_SloMo->setToolTip( "Speed of Playback" );
	bSizer4->addWidget( m_slider_SloMo, 1 );
	bSizer4->addWidget( new QLabel( "Slo-Mo", this ) );
	bSizer_panel->addLayout( bSizer4 );

	//	set the default from the prefs
	PrefsData& data = PrefsMgr::Data();
	this->m_checkBox_loop->setChecked(data.m_bPlaybackLoopAtEnd.GetValue());
	this->m_checkBox_lowres->setChecked(data.m_bPlaybackLowRes.GetValue());
	this->m_checkBox_mute->setChecked(data.m_bMuteAudio.GetValue());

	QObject::connect( m_checkBox_loop, &QCheckBox::clicked, [this]() { CheckBox_Loop_OnCheck(); } );
	QObject::connect( m_checkBox_lowres, &QCheckBox::clicked, [this]() { CheckBox_LowRes_OnCheck(); } );
	QObject::connect( m_checkBox_mute, &QCheckBox::clicked, [this]() { CheckBox_Mute_OnCheck(); } );
	QObject::connect( m_slider_SloMo, &QSlider::valueChanged, [this](int) { Slider_SloMo_ScrollChanged(); } );

	// Add this panel to the pane manager, as a floating pane that
	// goes away when it is closed
	tqtPaneMgr::AddPane(this, tqtPaneInfo().Name(title).Caption(title).Show(false).Layer(2).Float().DestroyOnClose(true));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
plbkPlaybackControlsDialog::~plbkPlaybackControlsDialog()
{
	if (is_playbackmode())
	{
		plbkModePlayback* pMode = get_playbackmode();
		pMode->ExitMode();
	}

	if (plbkPlaybackControlsDialog::DialogInstance == this)
		plbkPlaybackControlsDialog::DialogInstance = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void plbkPlaybackControlsDialog::CheckBox_Loop_OnCheck()
{
	if (is_playbackmode())
	{
		plbkModePlayback* pMode = get_playbackmode();
		if (pMode != NULL)
			pMode->SetLooping( m_checkBox_loop->isChecked() );
	}
	PrefsData& prefsdata = PrefsMgr::Data();
	prefsdata.m_bPlaybackLoopAtEnd.SetValue( m_checkBox_loop->isChecked() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void plbkPlaybackControlsDialog::CheckBox_LowRes_OnCheck()
{
	if (is_playbackmode())
	{
		plbkModePlayback* pMode = get_playbackmode();
		if (pMode != NULL)
			pMode->SetLowRes( m_checkBox_lowres->isChecked() );
	}
	PrefsData& prefsdata = PrefsMgr::Data();
	prefsdata.m_bPlaybackLowRes.SetValue( m_checkBox_lowres->isChecked() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void plbkPlaybackControlsDialog::CheckBox_Mute_OnCheck()
{
	if (is_playbackmode())
	{
		plbkModePlayback* pMode = get_playbackmode();
		if (pMode != NULL)
			pMode->SetMute( m_checkBox_mute->isChecked() );
	}
	PrefsData& prefsdata = PrefsMgr::Data();
	prefsdata.m_bMuteAudio.SetValue( m_checkBox_mute->isChecked() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void plbkPlaybackControlsDialog::Slider_SloMo_ScrollChanged()
{
	if (is_playbackmode())
	{
		set_slomo( get_playbackmode() );
	}
}

//--------------------------------------------------------------------
//	set the slow motion from the slider
//--------------------------------------------------------------------
void plbkPlaybackControlsDialog::set_slomo(plbkModePlayback* i_pMode)
{
	if (i_pMode == NULL)
		return;

	int val = m_slider_SloMo->value();
	if (val == 0)
		i_pMode->SetSloMo( 1 );
	else
		i_pMode->SetSloMo( 1.0f/(tmlnTimeLine::GetFPS() * (val)));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
plbkModePlayback* plbkPlaybackControlsDialog::get_playbackmode()
{
	modeMode* pMode = modeModeMgr::GetCurrentMode();
	plbkModePlayback* pPBMode = dynamic_cast<plbkModePlayback*>( pMode );
	DBG_ASSERT( pPBMode != 0, "Mode Playback not found" );
	return pPBMode;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void plbkPlaybackControlsDialog::set_playbackmode()
{
	if (!is_playbackmode())
	{
		modeModeMgr::SetCurrentMode( plbkPackage::GetModePlaybackID() );
		if (is_playbackmode())
		{
			plbkModePlayback* pMode = get_playbackmode();
			if (pMode != NULL)
			{
				set_slomo( pMode );
				pMode->SetMute( m_checkBox_mute->isChecked() );
				pMode->SetLowRes( m_checkBox_lowres->isChecked() );
				pMode->SetLooping( m_checkBox_loop->isChecked() );
			}
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void plbkPlaybackControlsDialog::close_playbackmode()
{
	if (is_playbackmode())
	{
		plbkModePlayback* pMode = get_playbackmode();
		pMode->ExitMode();
	}
}

//--------------------------------------------------------------------
//	return true if the current mode is playback
//--------------------------------------------------------------------
bool plbkPlaybackControlsDialog::is_playbackmode()
{
	modeMode* pMode = modeModeMgr::GetCurrentMode();
	plbkModePlayback* pPBMode = dynamic_cast<plbkModePlayback*>( pMode );
	return (pPBMode != NULL);
}

#endif
