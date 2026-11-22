/*****************************************************************************
**	envtEnvironmentSwlPage.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/GUI/qtGUI/envtEnvironmentSwlPage.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/swl/swlPropertyObject.hpp"
#include "Support/swl/swlScriptObject.hpp"
#include "Support/swl/swlSelectionUtil.hpp"
#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"

#include <vector>

#ifdef USE_QT

#include <QScrollArea>
#include <QVBoxLayout>

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
envtEnvironmentSwlPage* envtEnvironmentSwlPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envtEnvironmentSwlPage::envtEnvironmentSwlPage( QWidget* parent )
	: QWidget(parent)
{
	QVBoxLayout* bSizer = new QVBoxLayout( this );

	QScrollArea* pScroll = new QScrollArea( this );
	pScroll->setWidgetResizable( true );
	pScroll->setFrameShape( QFrame::NoFrame );
	pScroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	m_tabPage_SoftwareLighting = new QWidget();
	pScroll->setWidget( m_tabPage_SoftwareLighting );
	bSizer->addWidget( pScroll, 1 );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envtEnvironmentSwlPage::~envtEnvironmentSwlPage()
{
	if (envtEnvironmentSwlPage::Instance == this)
		envtEnvironmentSwlPage::Instance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------
void envtEnvironmentSwlPage::Clear()
{
	//	clear the controls
	m_EnvironmentName = nameString();
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------
void envtEnvironmentSwlPage::Update(const nameString& i_EnvironmentName)
{
	m_EnvironmentName = i_EnvironmentName;

	swlScriptObject* script_obj = swlSelectionUtil::GetSelectedScriptObject();
	if (script_obj)
	{
		pqtFormControlBuilder::BuildForm(this->m_tabPage_SoftwareLighting, "Software Lighting",
			(script_obj->GetPropertyUI()->GetListContainer()), true, false );

		// If property object is read-only (i.e. locked materials),
		// disable whole panel.
		this->m_tabPage_SoftwareLighting->setEnabled(!script_obj->GetPropertyUI()->IsReadOnly());
	}
	else
	{
		pqtFormControlBuilder::ClearForm(this->m_tabPage_SoftwareLighting);
	}
}

#endif // USE_QT
