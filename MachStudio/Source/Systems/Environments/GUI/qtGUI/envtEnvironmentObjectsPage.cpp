/*****************************************************************************
**	envtEnvironmentObjectsPage.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/GUI/qtGUI/envtEnvironmentObjectsPage.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"
#include "Systems/Environments/Object/envtScriptObject.hpp"

#include <algorithm>
#include <vector>

#ifdef USE_QT

#include <QListWidget>
#include <QVBoxLayout>

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
envtEnvironmentObjectsPage* envtEnvironmentObjectsPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envtEnvironmentObjectsPage::envtEnvironmentObjectsPage( QWidget* parent )
: QWidget( parent ),
  m_bUpdating( false )
{
	QVBoxLayout* bSizer3 = new QVBoxLayout( this );
	m_checkList_Objects = new QListWidget( this );
	m_checkList_Objects->setSortingEnabled( true );
	bSizer3->addWidget( m_checkList_Objects, 1 );

	QObject::connect( m_checkList_Objects, &QListWidget::itemChanged,
		[this](QListWidgetItem* i_pItem) { checkList_Objects_Toggle(i_pItem); } );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envtEnvironmentObjectsPage::~envtEnvironmentObjectsPage()
{
	if (envtEnvironmentObjectsPage::Instance == this)
		envtEnvironmentObjectsPage::Instance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------
void envtEnvironmentObjectsPage::Clear()
{
	//	clear the controls
	m_checkList_Objects->clear();
	m_EnvironmentName = nameString();
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------
void envtEnvironmentObjectsPage::Update(const nameString& i_EnvironmentName)
{
	m_EnvironmentName = i_EnvironmentName;
	m_bUpdating = true;
	m_checkList_Objects->clear();

	std::vector<nameString> all_objects;
	evmtEnvironmentMgr::GetAllObjects(all_objects);

	std::vector<nameString> set_objects;
	evmtEnvironmentMgr::GetObjectsInSet(m_EnvironmentName, set_objects);

	const int num_objects = all_objects.size();
	if (i_EnvironmentName == evmtEnvironmentMgr::GetSwlEnvironmentName())
	{
		// Software lighting applies to all of the objects
		for (int i=0; i<num_objects; i++)
		{
			QListWidgetItem* pItem = new QListWidgetItem( tqtQString(all_objects[i].GetString()), m_checkList_Objects );
			pItem->setFlags( pItem->flags() | Qt::ItemIsUserCheckable );
			pItem->setCheckState( Qt::Checked );
		}
		m_checkList_Objects->setEnabled(false);
	}
	else
	{
		m_checkList_Objects->setEnabled(true);
		for (int i=0; i<num_objects; i++)
		{
			std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_objects[i]);
			bool checked = (it != set_objects.end());
			QListWidgetItem* pItem = new QListWidgetItem( tqtQString(all_objects[i].GetString()), m_checkList_Objects );
			pItem->setFlags( pItem->flags() | Qt::ItemIsUserCheckable );
			pItem->setCheckState( checked ? Qt::Checked : Qt::Unchecked );
		}
	}
	m_bUpdating = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void envtEnvironmentObjectsPage::checkList_Objects_Toggle( QListWidgetItem* i_pItem )
{
	if (!m_bUpdating)
	{
		bool bChecked = (i_pItem->checkState() == Qt::Checked);
		nameString cur_object( tqtStdString(i_pItem->text()) );
		if (bChecked)
		{
			envtOperations::AddObjectToEnvironment(m_EnvironmentName, cur_object);
		}
		else
		{
			envtOperations::RemoveObjectFromEnvironment(m_EnvironmentName, cur_object);
		}
	}
}

#endif // USE_QT
