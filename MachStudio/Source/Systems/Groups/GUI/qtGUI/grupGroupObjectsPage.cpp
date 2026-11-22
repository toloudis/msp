/*****************************************************************************
**	grupGroupObjectsPage.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Groups/GUI/qtGUI/grupGroupObjectsPage.hpp"

#include "Systems/Groups/Undo/grupOperations.hpp"
#include "Systems/Groups/Object/grupGroupObject.hpp"

#include <algorithm>
#include <vector>

#ifdef USE_QT

#include <QListWidget>
#include <QVBoxLayout>

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
grupGroupObjectsPage* grupGroupObjectsPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
grupGroupObjectsPage::grupGroupObjectsPage( QWidget* parent )
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
grupGroupObjectsPage::~grupGroupObjectsPage()
{
	if (grupGroupObjectsPage::Instance == this)
		grupGroupObjectsPage::Instance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------
void grupGroupObjectsPage::Clear()
{
	//	clear the controls
	m_checkList_Objects->clear();
	m_GroupName = nameString();
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------
void grupGroupObjectsPage::Update(const nameString& i_GroupName)
{
	m_GroupName = i_GroupName;
	m_bUpdating = true;
	m_checkList_Objects->clear();

	std::vector<nameString> all_objects;
	grpsGroupMgr::GetAllObjects(all_objects);

	std::vector<nameString> set_objects;
	grpsGroupMgr::GetObjectsInGroup(m_GroupName, set_objects);

	const int num_objects = all_objects.size();
	for (int i=0; i<num_objects; i++)
	{
		std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_objects[i]);
		bool checked = (it != set_objects.end());
		QListWidgetItem* pItem = new QListWidgetItem( tqtQString(all_objects[i].GetString()), m_checkList_Objects );
		pItem->setFlags( pItem->flags() | Qt::ItemIsUserCheckable );
		pItem->setCheckState( checked ? Qt::Checked : Qt::Unchecked );
	}
	m_bUpdating = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void grupGroupObjectsPage::checkList_Objects_Toggle( QListWidgetItem* i_pItem )
{
	if (!m_bUpdating && !m_GroupName.IsEmpty())
	{
		bool bChecked = (i_pItem->checkState() == Qt::Checked);
		nameString cur_object( tqtStdString(i_pItem->text()) );
		if (bChecked)
		{
			grupOperations::AddObjectToGroup(m_GroupName, cur_object);
		}
		else
		{
			grupOperations::RemoveObjectFromGroup(m_GroupName, cur_object);
		}
	}
}

#endif // USE_QT
