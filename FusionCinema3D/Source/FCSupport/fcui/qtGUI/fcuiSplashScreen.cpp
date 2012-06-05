/*****************************************************************************
**	fcuiSplashScreen.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcui/qtGUI/fcuiSplashScreen.hpp"

#include "FCSupport/anim/qtGUI/animCategoryPixmap.hpp"
#include "FCSupport/anim/qtGUI/animElementPixmap.hpp"
#include "FCSupport/anim/qtGUI/animMoviePixmap.hpp"
#include "FCSupport/anim/qtGUI/animModePixmap.hpp"
#include "FCSupport/anim/qtGUI/animSubjectPixmap.hpp"
#include "FCSupport/fcmd/fcmdConstants.hpp"
#include "FCSupport/fcmd/fcmdModeMgr.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
fcuiSplashScreen::fcuiSplashScreen(const QPixmap& i_Pixmap, Qt::WindowFlags f )
:QSplashScreen(i_Pixmap, f)
{
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
fcuiSplashScreen::~fcuiSplashScreen()
{
}

//----------------------------------------------------------------------------
// Show the splash screen
//----------------------------------------------------------------------------
void fcuiSplashScreen::ShowScreen()
{
	QPropertyAnimation* animation = new QPropertyAnimation(this);
	animation->setDuration(5000);
	animation->setStartValue(0.5);
	animation->setEndValue(1.0);
	animation->start(QPropertyAnimation::DeleteWhenStopped);
	this->show();
	//Todo - Set this message based on the type
	//this->showMessage("Loading ....");
	
}

//----------------------------------------------------------------------------
// Hide the splash screen
//----------------------------------------------------------------------------
void fcuiSplashScreen::HideScreen(QWidget* i_Window)
{
	this->finish(i_Window);
}