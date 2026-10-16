/*****************************************************************************
**	cptrRenderOptionsDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/qtGUI/cptrRenderOptionsDialog.hpp"

#include "Features/Capture/cptrRenderOptionsDialogUtil.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/RenderLayers/rdrLayersDialogUtil.hpp"
#include "Features/RenderLayers/qtGUI/rdrLayersDialog.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/rprf/rprfPrefsUtil.hpp"

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#include "Tool/gpx/gpxRenderControl.hpp"
#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"

#ifdef USE_QT

#include <QCloseEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrRenderOptionsDialog::cptrRenderOptionsDialog( QWidget* parent )
:	QDialog(parent)
{
	this->setAttribute( Qt::WA_DeleteOnClose );
	this->setWindowTitle( "Render" );

	QVBoxLayout* bSizer1 = new QVBoxLayout( this );
	QTabWidget* pNotebook = new QTabWidget( this );
	bSizer1->addWidget( pNotebook, 1 );

	QPushButton* pRender = new QPushButton( "Render", this );
	pRender->setAutoDefault( false );
	bSizer1->addWidget( pRender, 0, Qt::AlignHCenter );

	//	Add the Options tab page
	QScrollArea* pScroll = new QScrollArea( pNotebook );
	pScroll->setWidgetResizable( true );
	pScroll->setFrameShape( QFrame::NoFrame );
	pScroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	QWidget* pTabPage_MainOptions = new QWidget();
	pScroll->setWidget( pTabPage_MainOptions );
	pNotebook->addTab( pScroll, "Output" );

	//	create the entries for the options tab
	prtyObject* pDO = captRenderOutputDataUtil::GetDataObject();
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	pqtFormControlBuilder::BuildForm(pTabPage_MainOptions, "Render Layers", (pDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );

	//	Add the Render Layers tab page
	rdrLayersDialog* pDialog_RenderLayers = new rdrLayersDialog( pNotebook );
	pNotebook->addTab( pDialog_RenderLayers, "Render Layers" );
	pNotebook->setCurrentWidget( pDialog_RenderLayers );
	pDialog_RenderLayers->Update();
	rdrLayersDialog::Instance = pDialog_RenderLayers;
	rprfPrefsUtil::SetUpdateFunction(&rdrLayersDialogUtil::UpdateTabs);

	this->resize( 640, 560 );

	QObject::connect( pRender, &QPushButton::clicked, [this]() { button_Render_OnButtonClick(); } );
}

//--------------------------------------------------------------------
//	if the dialog is closing and the user didn't hit render then exit out
//	of capture mode. The dialog deletes itself when closed.
//--------------------------------------------------------------------
void cptrRenderOptionsDialog::closeEvent( QCloseEvent* i_pEvent )
{
	if ( !cptrRenderUtil::GetCapture() )
	{
		gpxRenderControl::ConfirmSingleThread();
		cptrRenderOptionsDialogUtil::SetExitting(true);
	}
	QDialog::closeEvent( i_pEvent );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderOptionsDialog::button_Render_OnButtonClick()
{
	cptrRenderOptionsDialogUtil::Hide();
	this->close();
}

#endif
