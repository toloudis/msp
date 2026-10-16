/*****************************************************************************
**  prefsLayoutChoice.hpp
**
**     Choice box displaying current and available cameras and
**	director's cuts
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/qtGUI/prefsLayoutChoice.hpp"

#include "Features/Prefs/qtGUI/prefsLayoutMgr.hpp"

#ifdef USE_QT

//--------------------------------------------------------------------
//	Static pointer to the instance of the control, will be cleared
//	when the panel grid is deleted.
//--------------------------------------------------------------------
prefsLayoutChoice* prefsLayoutChoice::Instance = NULL;

//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
prefsLayoutChoice::prefsLayoutChoice(QWidget* parent)
:	QComboBox(parent),
	m_bDisableNotify(false)
{
	this->setMinimumWidth(160);
	this->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	QObject::connect(this, QOverload<int>::of(&QComboBox::activated), [this](int) { SelectedIndexChanged(); });
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
prefsLayoutChoice::~prefsLayoutChoice()
{
	if (prefsLayoutChoice::Instance == this)
		prefsLayoutChoice::Instance = NULL;

}

//------------------------------------------------------------------------
// Set list of layout names
//------------------------------------------------------------------------
void prefsLayoutChoice::Update()
{			
	QString last_selection = this->currentText();
	this->m_bDisableNotify = true;
	this->clear();

	std::vector<std::string> layout_names;
	prefsLayoutMgr::GetLayoutNames(layout_names);

	// Add names of layouts
	const int num_layouts = static_cast<int>(layout_names.size());
	for (int i=0; i<num_layouts; i++)
	{
		this->addItem( tqtQString(layout_names[i]) );
	}

	// Try to restore last selection, if it is still available
	this->setCurrentIndex(this->findText(last_selection));
	this->m_bDisableNotify = false;
}

//------------------------------------------------------------------------
// Select the layout with the given name, if it is in the list
//------------------------------------------------------------------------
void prefsLayoutChoice::SetStringSelection(const std::string& i_Name)
{
	const int index = this->findText(tqtQString(i_Name));
	if (index >= 0)
		this->setCurrentIndex(index);
}

//------------------------------------------------------------------------
// event callbacks
//------------------------------------------------------------------------
void prefsLayoutChoice::SelectedIndexChanged()
{
	if (!m_bDisableNotify && !this->currentText().isEmpty())
	{
		std::string layout_name(tqtStdString(this->currentText()));
		prefsLayoutMgr::SwitchToNamedLayout(layout_name);
	}
}

#endif // USE_QT