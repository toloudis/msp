/*****************************************************************************
**	ControlsTestDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "ControlsTestDialogUtil.hpp"

#include "ControlsTestDialog.h"
#include "ControlsTestMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/prty/prtyObject.hpp"
#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

//	these includes for testing only!!!
#ifdef USE_QT
#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#endif

//============================================================================
//============================================================================
namespace ControlsTestDialogUtil
{
	//--------------------------------------------------------------------
	//  Show load preferences dialog modally
	//--------------------------------------------------------------------
	void Show()
	{
		//
		prtyObject* pDO = ControlsTestMgr::GetDataObject();
		pDO->SortListByCategory();
		const bool cbSHOW_CATEGORY = true;
		const bool cbAUTO_COLLAPSE = false;

		DBG_ASSERT(tqtSystem::g_pMainForm != NULL, "MainForm not yet initialized.");

		//	get/create a "central" widget
		QWidget *centralwidget;
		if ((centralwidget = tqtSystem::g_pMainForm->centralWidget()) == NULL)
		{
			centralwidget = new QWidget(tqtSystem::g_pMainForm);
			centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
			tqtSystem::g_pMainForm->setCentralWidget(centralwidget);
		}

//		ControlsTestDialog dialog(tqtSystem::g_pMainForm);
		pqtFormControlBuilder::BuildForm(centralwidget, "ControlsTest",
			(pDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );
		//dialog.show();

		//	TEST CODE!!!!!
		/*
		if (centralwidget != NULL)
		{
			// layout
			QGridLayout* main_layout = new QGridLayout( centralwidget );
			main_layout->setObjectName( QString::fromUtf8("Main Layout") );
			main_layout->setSpacing(2);
			main_layout->setContentsMargins(0, 0, 0, 0);
			main_layout->setSizeConstraint(QLayout::SetDefaultConstraint);
			// label
			QLabel *label1 = new QLabel(centralwidget);
			label1->setText(QString("One!"));
			QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
			sizePolicy.setHorizontalStretch(0);
			sizePolicy.setVerticalStretch(0);
			sizePolicy.setHeightForWidth(label1->sizePolicy().hasHeightForWidth());
			label1->setSizePolicy(sizePolicy);
			main_layout->addWidget( label1 );
			// label
			QLabel *label2 = new QLabel(centralwidget);
			label2->setText(QString("Two!"));
			sizePolicy.setHeightForWidth(label2->sizePolicy().hasHeightForWidth());
			label2->setSizePolicy(sizePolicy);
			main_layout->addWidget( label2 );
			//
			QMetaObject::connectSlotsByName(tqtSystem::g_pMainForm);
		}
		*/
	}

}	// end of namespace

