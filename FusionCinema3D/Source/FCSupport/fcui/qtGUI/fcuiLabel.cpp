/*****************************************************************************
**	fcuiLabel.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcui/qtGUI/fcuiLabel.hpp"

#include "FCSupport/path/pathDirectoryParser.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/it/itStringUtil.hpp"


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------
fcuiLabel::fcuiLabel(QWidget* i_pParent, fsLocator i_Name, int i_labelNumber, int i_Width)
:	QLabel(i_pParent)
{
	SetUp( i_pParent, i_Name, i_labelNumber, i_Width );
}


//----------------------------------------------------------------------------
// Destructors
//----------------------------------------------------------------------------
fcuiLabel::~fcuiLabel()
{
	this->setText("");
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fcuiLabel::SetUp(QWidget* i_pParent,fsLocator i_Name, int i_labelNumber, int i_Width)
{
	// set the Qt flags
	//this->alignment(Qt::AlignHCenter,Qt::AlignVCenter);

	QFont newFont;
	newFont.setFamily("Raavi");
	newFont.setPointSize(10);
	this->setGeometry(QRect(i_labelNumber*i_Width,0,60,20));
	QPalette palette;
	QBrush whitebrush(QColor(255, 255, 255, 255));
	whitebrush.setStyle(Qt::SolidPattern);
	palette.setBrush(QPalette::Active, QPalette::WindowText, whitebrush); 
	palette.setBrush(QPalette::Inactive, QPalette::WindowText, whitebrush);
	palette.setBrush(QPalette::Disabled, QPalette::WindowText, whitebrush);
	this->setPalette(palette);
	this->setAlignment(Qt::AlignCenter);
	this->setFont(newFont);
	itString locator = i_Name.GetLastName();
	QString str = QString::fromLocal8Bit(itStringUtil::GetStdString(locator).c_str());

	this->setText(str);
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fcuiLabel::Clear()
{
	this->setText("");
}
