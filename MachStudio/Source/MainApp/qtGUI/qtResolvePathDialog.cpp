/*****************************************************************************
**	qtResolvePathDialog.cpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/qtGUI/qtResolvePathDialog.hpp"

#include "Core/Fs/fsFileUtil.hpp"


#ifdef USE_QT

namespace
{
}	// end of namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
qtResolvePathDialog::qtResolvePathDialog( QWidget* parent, 
										  const fsLocator& i_Filename,
										  const std::string &i_Category )
: qtResolvePathDialogBase( parent ),
  m_ResolveChoice(e_Skip) // this sets default value for when user closes dialog with "X" in corner
{
	itString filename;
	fsFileUtil::LocatorToUnicodeString(i_Filename,filename);
	m_staticText_Filename->setText(QString::fromWCharArray(filename.GetString()));

	std::string skipAllString("Skip All");
	if (!i_Category.empty())
	{
		skipAllString += " ";
		skipAllString += i_Category;
	}
	m_button_SkipAll->setText(tqtQString(skipAllString));

	this->adjustSize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
qtResolvePathDialog::~qtResolvePathDialog()
{
}

//--------------------------------------------------------------------
// Event handlers
//--------------------------------------------------------------------
void qtResolvePathDialog::buttonLocateClick()
{
	m_ResolveChoice = e_Locate;
	this->accept();
}

void qtResolvePathDialog::buttonRetryClick()
{
	m_ResolveChoice = e_Retry;
	this->accept();
}

void qtResolvePathDialog::buttonSkipClick()
{
	m_ResolveChoice = e_Skip;
	this->accept();
}

void qtResolvePathDialog::buttonSkipAllClick()
{
	m_ResolveChoice = e_SkipAll;
	this->accept();
}

void qtResolvePathDialog::buttonAbortClick()
{
	m_ResolveChoice = e_Abort;
	this->accept();
}


#endif // USE_QT
