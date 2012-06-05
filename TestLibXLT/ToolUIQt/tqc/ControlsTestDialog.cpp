/********************************************************************************************\
**	ControlsTestDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
//#include "stdafx.h"
#include "ControlsTestDialog.h"

//#include "FCSupport/fcmd/fcmdModeMgr.hpp"
//#include "Support/pyth/pythUtil.hpp"
//
//#include "Core/app/private/appApplicationPAC.hpp"
//#include "Tool/gui/guiSingleDocHandler.hpp"

#include <QtGui/QWidget>


//----------------------------------------------------------------------------
// Set up the UI and then fill the background
//----------------------------------------------------------------------------
ControlsTestDialog::ControlsTestDialog(QWidget *parent)
:	QMainWindow(parent)
{
	//winid = NULL;
	//ui.setupUi(this);
	///*QPixmap bg("MockUp.jpg");
	//QPalette p(palette());
	//p.setBrush(QPalette::Background, bg);
	//
	//setAutoFillBackground(true);
	//setPalette(p);*/
	//SetWinId(ui.widget_5->winId());
	//ui.page_5->setFocus();
	////connect(ui.newMovie_4, SIGNAL(clicked()),ui.stackedWidget, SLOT(setCurrentIndex(2)));
	//QObject::connect(ui.newMovie_4, SIGNAL(clicked()), this, SLOT(setIndex1()));
	//QObject::connect(ui.newMovie_2, SIGNAL(clicked()), this, SLOT(setIndex1()));
	//QObject::connect(ui.newMovie_3, SIGNAL(clicked()), this, SLOT(setIndex1()));
	//QObject::connect(ui.editMovie, SIGNAL(clicked()), this, SLOT(setIndex2()));
	//QObject::connect(ui.editMovie_2, SIGNAL(clicked()), this, SLOT(setIndex2()));
	//QObject::connect(ui.editMovie_3, SIGNAL(clicked()), this, SLOT(setIndex2()));
	//QObject::connect(ui.DancingBride, SIGNAL(clicked()), this, SLOT(setIndex3()));

	//if(fcmdModeMgr::Instance != NULL)
	//{
	//	fcmdModeMgr::Instance->CreateNewMode(ui.castButton, fcmdModeMgr::e_Cast);
	//}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ControlsTestDialog::~ControlsTestDialog()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
QWidget* ControlsTestDialog::GetTabPageMain()
{
	return this->centralWidget();
}

//----------------------------------------------------------------------------
// Get the HWND for the window that acts as the MSP window
//----------------------------------------------------------------------------
HWND ControlsTestDialog::GetWinId()
{
	return winid;
}

//----------------------------------------------------------------------------
// Set the HWND for the window that acts as the MSP window
//----------------------------------------------------------------------------
void ControlsTestDialog::SetWinId(HWND i_winId)
{
	winid = i_winId;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ControlsTestDialog::closeEvent(QCloseEvent* i_Event)
{
//	appApplicationPAC::Instance()->Exit();
	i_Event->accept();
}

//----------------------------------------------------------------------------
// This is a function to test the communication between MSP and Qt
//----------------------------------------------------------------------------
//void ControlsTestDialog::setIndex1()
//{
//	ui.stackedWidget->setCurrentIndex(1);
//	ui.page_4->setFocus();
//}
//void ControlsTestDialog::setIndex2()
//{
//	ui.stackedWidget->setCurrentIndex(2);
//	ui.page_3->setFocus();
//}
//void ControlsTestDialog::setIndex3()
//{
//	ui.stackedWidget->setCurrentIndex(3);
//}

