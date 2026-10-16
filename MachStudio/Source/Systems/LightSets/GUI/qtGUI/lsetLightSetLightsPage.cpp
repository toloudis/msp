/*****************************************************************************
**	lsetLightSetLightsPage.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/GUI/qtGUI/lsetLightSetLightsPage.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"
#include "Systems/LightSets/Object/lsetScriptObject.hpp"

#include <algorithm>
#include <vector>

#ifdef USE_QT

#include <QListWidget>
#include <QVBoxLayout>

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
lsetLightSetLightsPage* lsetLightSetLightsPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetLightSetLightsPage::lsetLightSetLightsPage( QWidget* parent )
: QWidget( parent ),
  m_bUpdating( false )
{
	QVBoxLayout* bSizer3 = new QVBoxLayout( this );
	m_checkList_Lights = new QListWidget( this );
	m_checkList_Lights->setSortingEnabled( true );
	bSizer3->addWidget( m_checkList_Lights, 1 );

	QObject::connect( m_checkList_Lights, &QListWidget::itemChanged,
		[this](QListWidgetItem* i_pItem) { checkList_Lights_Toggle(i_pItem); } );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetLightSetLightsPage::~lsetLightSetLightsPage()
{
	if (lsetLightSetLightsPage::Instance == this)
		lsetLightSetLightsPage::Instance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------
void lsetLightSetLightsPage::Clear()
{
	//	clear the controls
	m_checkList_Lights->clear();
	m_LightSetName = nameString();
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------
void lsetLightSetLightsPage::Update(const nameString& i_LightSetName)
{
	m_LightSetName = i_LightSetName;
	m_bUpdating = true;
	m_checkList_Lights->clear();

	std::vector<nameString> all_objects;
	ltstLightSetMgr::GetAllLights(all_objects);

	std::vector<nameString> set_objects;
	ltstLightSetMgr::GetLightsInSet(m_LightSetName, set_objects);

	const int num_objects = all_objects.size();
	for (int i=0; i<num_objects; i++)
	{
		std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_objects[i]);
		bool checked = (it != set_objects.end());
		QListWidgetItem* pItem = new QListWidgetItem( tqtQString(all_objects[i].GetString()), m_checkList_Lights );
		pItem->setFlags( pItem->flags() | Qt::ItemIsUserCheckable );
		pItem->setCheckState( checked ? Qt::Checked : Qt::Unchecked );
	}
	m_bUpdating = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void lsetLightSetLightsPage::checkList_Lights_Toggle( QListWidgetItem* i_pItem )
{
	if (!m_bUpdating && !m_LightSetName.IsEmpty())
	{
		bool bChecked = (i_pItem->checkState() == Qt::Checked);
		nameString cur_object( tqtStdString(i_pItem->text()) );
		if (bChecked)
		{
			lsetOperations::AddLightToSet(m_LightSetName, cur_object);
		}
		else
		{
			lsetOperations::RemoveLightFromSet(m_LightSetName, cur_object);
		}
	}
}

#endif // USE_QT
