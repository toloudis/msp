/*****************************************************************************
**	brshColorDialog.cpp
**
**		see. hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "Support/brsh/qtGUI/brshColorDialog.hpp"

#include "Support/brsh/brshColorDialogUtil.hpp"
#include "Support/brsh/brshPaintBrushMgr.hpp"

#include "Core/fs/fsLocator.hpp"
#include "Core/prty/prtyObject.hpp"

#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"

#ifdef USE_QT

#include <QCloseEvent>
#include <QScrollArea>
#include <QVBoxLayout>

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
brshColorDialog* brshColorDialog::Instance = NULL;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
brshColorDialog::brshColorDialog( QWidget* parent, const char* i_Title,
									const char* i_Caption )
:	QWidget(parent)
{
	QVBoxLayout* bSizer1 = new QVBoxLayout( this );

	//	Add the brush page
	QScrollArea* pScroll = new QScrollArea( this );
	pScroll->setWidgetResizable( true );
	pScroll->setFrameShape( QFrame::NoFrame );
	pScroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	QWidget* pTabPage_Brush = new QWidget();
	pScroll->setWidget( pTabPage_Brush );
	bSizer1->addWidget( pScroll, 1 );

	//	create the entries for the paint brush tab
	prtyObject* pDO = brshPaintBrushMgr::GetBrushObject();
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	pqtFormControlBuilder::BuildForm(pTabPage_Brush, "PaintBrush", (pDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );

	// Add this panel to the pane manager
	tqtPaneMgr::AddPane(this, tqtPaneInfo().Name(i_Title).Caption(i_Caption).Show().Layer(1).Left().BestSize(375, 175));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
brshColorDialog::~brshColorDialog()
{
	if (brshColorDialog::Instance == this)
	{
		brshColorDialog::Instance = NULL;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
void brshColorDialog::closeEvent( QCloseEvent* event )
{
	brshColorDialogUtil::CleanUp();
	QWidget::closeEvent(event);
}
#endif
