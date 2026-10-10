/*****************************************************************************
**  quiStatusBarMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiStatusBarMgr.hpp"
#include "ToolUIQt/tqc/tqcStatusBar.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"


//===========================================================================
//	quiStatusBarMgr functions
//===========================================================================

namespace
{
	int l_ErrorPaneIndex = -1;

#ifdef USE_QT
	// "Locked" indicator shown over the mode field
	QPointer<QLabel> l_ModeBMP;
#endif // USE_QT
}

//---------------------------------------------------------------------------
//	SetText()
//---------------------------------------------------------------------------
void quiStatusBarMgr::SetText( int i_PanelIndex, const char * i_Text )
{
#ifdef USE_QT
	if (tqtSystem::g_pStatusBar)
	{
		tqtSystem::g_pStatusBar->SetStatusText(tqtQString(i_Text), i_PanelIndex);
	}
#endif
}

//---------------------------------------------------------------------------
//	SetToolTip() - set help text for an individual panel
//---------------------------------------------------------------------------
void quiStatusBarMgr::SetToolTip( int i_PanelIndex, const char * i_Text )
{
#ifdef USE_QT
	if (tqtSystem::g_pStatusBar)
	{
		tqtSystem::g_pStatusBar->SetPanelToolTip(i_PanelIndex, i_Text ? i_Text : "");
	}
#endif
}

//---------------------------------------------------------------------------
// Set which pane to use for error messages
//---------------------------------------------------------------------------
void quiStatusBarMgr::SetErrorPaneIndex(int i_Index)
{
	l_ErrorPaneIndex = i_Index;
}

//---------------------------------------------------------------------------
//	SetErrorMessage() sets text into main messaging panel
//---------------------------------------------------------------------------
void quiStatusBarMgr::SetErrorMessage( const char * i_Text )
{
	if (l_ErrorPaneIndex >= 0)
		SetText(l_ErrorPaneIndex, i_Text);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void quiStatusBarMgr::ClearErrorMessage()
{
	if (l_ErrorPaneIndex >= 0)
		SetText(l_ErrorPaneIndex, "");
}

//---------------------------------------------------------------------------
//	CreateModeBitmap - Create the indicator shown over the mode panel
//---------------------------------------------------------------------------
void quiStatusBarMgr::CreateModeBitmap()
{
#ifdef USE_QT
	if (tqtSystem::g_pStatusBar && !l_ModeBMP)
	{
		QLabel* pModeField = tqtSystem::g_pStatusBar->GetField(0);
		if (pModeField)
		{
			l_ModeBMP = new QLabel("Locked", pModeField);
			l_ModeBMP->setStyleSheet("QLabel { background-color: red; color: black; padding-left: 10px; }");
			l_ModeBMP->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

			QHBoxLayout* pLayout = new QHBoxLayout(pModeField);
			pLayout->setContentsMargins(0, 0, 0, 0);
			pLayout->addWidget(l_ModeBMP);

			l_ModeBMP->hide();
		}
	}
#endif
}

//---------------------------------------------------------------------------
//	ShowBitmap - Show the bitmap associated to the mode panel in the status bar
//---------------------------------------------------------------------------
void quiStatusBarMgr::ShowModeBitmap()
{
#ifdef USE_QT
	if (l_ModeBMP)
		l_ModeBMP->show();
#endif
}

//---------------------------------------------------------------------------
//	HideBitmap - Hide the bitmap associated to the mode panel in the status bar
//---------------------------------------------------------------------------
void quiStatusBarMgr::HideModeBitmap()
{
#ifdef USE_QT
	if (l_ModeBMP)
		l_ModeBMP->hide();
#endif
}
