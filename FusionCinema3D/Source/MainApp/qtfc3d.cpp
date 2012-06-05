#include "stdafx.h"
#include "qtfc3d.h"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Support/pyth/pythUtil.hpp"
#include "Core/app/private/appApplicationPAC.hpp"
#include <QtGui/QWidget>
#include "FCSupport/fcmd/fcmdModeMgr.hpp"

//------------------------------------------------------------------
// Set up the UI and then fill the background
//------------------------------------------------------------------
QtFC3D::QtFC3D(QWidget *parent)
	: QMainWindow(parent)
{
	winid = NULL;
		ui.setupUi(this);
		/*QPixmap bg("MockUp.jpg");
		QPalette p(palette());
		p.setBrush(QPalette::Background, bg);
		
		setAutoFillBackground(true);
		setPalette(p);*/
		SetWinId(ui.widget_5->winId());
		ui.page_5->setFocus();
		//connect(ui.newMovie_4, SIGNAL(clicked()),ui.stackedWidget, SLOT(setCurrentIndex(2)));
		QObject::connect(ui.newMovie_4, SIGNAL(clicked()), this, SLOT(setIndex1()));
		QObject::connect(ui.newMovie_2, SIGNAL(clicked()), this, SLOT(setIndex1()));
		QObject::connect(ui.newMovie_3, SIGNAL(clicked()), this, SLOT(setIndex1()));
		QObject::connect(ui.editMovie, SIGNAL(clicked()), this, SLOT(setIndex2()));
		QObject::connect(ui.editMovie_2, SIGNAL(clicked()), this, SLOT(setIndex2()));
		QObject::connect(ui.editMovie_3, SIGNAL(clicked()), this, SLOT(setIndex2()));
		QObject::connect(ui.DancingBride, SIGNAL(clicked()), this, SLOT(setIndex3()));

		if(fcmdModeMgr::Instance != NULL)
		{
			fcmdModeMgr::Instance->CreateNewMode(ui.castButton, fcmdModeMgr::e_Cast);
		}

}
//------------------------------------------------------------------
//------------------------------------------------------------------
QtFC3D::~QtFC3D()
{
}

//------------------------------------------------------------------
// Get the HWND for the window that acts as the MSP window
//------------------------------------------------------------------
HWND QtFC3D::GetWinId()
{
	return winid;
}
//------------------------------------------------------------------
// Set the HWND for the window that acts as the MSP window
//------------------------------------------------------------------
void QtFC3D::SetWinId(HWND i_winId)
{
	winid = i_winId; 
}

//------------------------------------------------------------------
// This is a function to test the communication between MSP and Qt
//------------------------------------------------------------------
void QtFC3D::setIndex1()
{
	ui.stackedWidget->setCurrentIndex(1);
	ui.page_4->setFocus();
}
void QtFC3D::closeEvent(QCloseEvent* i_Event)
{
	appApplicationPAC::Instance()->Exit();
	i_Event->accept();
}

void QtFC3D::setIndex2()
{
	ui.stackedWidget->setCurrentIndex(2);
	ui.page_3->setFocus();
	
}

void QtFC3D::setIndex3()
{
	ui.stackedWidget->setCurrentIndex(3);
}
