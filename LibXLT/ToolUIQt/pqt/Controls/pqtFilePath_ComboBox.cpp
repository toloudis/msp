/****************************************************************************\
**	pqtFilePath_ComboBox.cpp
**
**		see .hpp
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtFilePath_ComboBox.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/prty/prtyFilePath.hpp"
#include "Core/prty/prtyFilePathComboBoxUIInfo.hpp"
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"

#include <algorithm>

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pqtFilePath_ComboBox::pqtFilePath_ComboBox(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										   QWidget* i_pParent)
:	pqtControl(i_pUIInfo),
	m_pActualControl(NULL),
	m_bHasValue(false)
{
	DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

	fsLocator newvalue;
	bool bSameValue = pqtControlUtil::GetCommonValue<fsLocator, prtyFilePath>(i_pUIInfo, newvalue);

	// create the actual control
	m_pActualControl = new pqtControlUtil::CleanUpWidget<QComboBox>(i_pParent);

	//	set the control values
	this->UpdateControl(i_pUIInfo.get());

	if (bSameValue)
	{
		SetValueIntoControl(newvalue);
		m_OriginalValue = newvalue;
	}

	//	hook up events
	//
	QObject::connect(m_pActualControl, QOverload<int>::of(&QComboBox::activated), [this](int) { ComboBox_ValueChanged(); });
}

//----------------------------------------------------------------------------
//	Destructor - dispose of controls
//----------------------------------------------------------------------------
pqtFilePath_ComboBox::~pqtFilePath_ComboBox()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
QWidget* pqtFilePath_ComboBox::GetControl()
{
	return m_pActualControl;
}

//----------------------------------------------------------------------------
// Update control to match the changes in the UI Info
//----------------------------------------------------------------------------
void pqtFilePath_ComboBox::UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
{
	m_pActualControl->setEnabled(!i_pUIInfo->GetReadOnly());

	// Preserve old value
	const bool bHadValue = m_bHasValue;
	const fsLocator old_value = GetValueFromControl();

	// Add choices to control
	QSignalBlocker blocker(m_pActualControl);
	m_pActualControl->clear();
	m_Values.clear();
	m_bHasValue = false;

	prtyFilePathComboBoxUIInfo* pUII = dynamic_cast<prtyFilePathComboBoxUIInfo*>(i_pUIInfo);
	DBG_ASSERT(pUII != NULL, "pqtFilePath_ComboBox needs a prtyFilePathComboBoxUIInfo");
	if (pUII)
	{
		const std::vector<fsLocator>& values = pUII->GetValues();
		const size_t num_choices = (std::min)(values.size(), pUII->m_List.size());
		for (size_t i = 0; i < num_choices; ++i)
		{
			m_pActualControl->addItem(tqtQString(pUII->m_List[i]));
			m_Values.push_back(values[i]);
		}
	}

	// Put old value back in without triggering a callback.
	if (bHadValue)
		SetValueIntoControl(old_value);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pqtFilePath_ComboBox::PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty)
{
	if (m_bLocalChangeNoUpdate) return;

	const fsLocator newvalue = (static_cast<prtyFilePath*>(i_pProperty))->GetValue();

	m_bLocalChangeNoUpdate = true;
	if ( !m_bHasValue || !prtyFilePathComboBoxUIInfo::IsSamePath(GetValueFromControl(), newvalue) )
	{
		SetValueIntoControl(newvalue);
		m_OriginalValue = newvalue;
	}
	m_bLocalChangeNoUpdate = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pqtFilePath_ComboBox::ComboBox_ValueChanged()
{
	if (m_bLocalChangeNoUpdate) return;

	m_bLocalChangeNoUpdate = true;
	const int selection = m_pActualControl->currentIndex();
	// picking the entry that is already chosen changes nothing
	if (selection >= 0 &&
		!prtyFilePathComboBoxUIInfo::IsSamePath(GetValueFromControl(), m_OriginalValue))
	{
		const fsLocator value = GetValueFromControl();
		if (!pqtControlUtil::SetCommonValue<fsLocator, prtyFilePath>(this, value))
		{
			// user did not confirm change, diplay the old value
			SetValueIntoControl(m_OriginalValue);
		}
		else
			m_OriginalValue = value;
	}
	m_bLocalChangeNoUpdate = false;
}

//----------------------------------------------------------------------------
// Select the entry for the given path, adding one if there is none
//----------------------------------------------------------------------------
void pqtFilePath_ComboBox::SetValueIntoControl(const fsLocator& i_Value)
{
	if (i_Value.GetNumNames() == 0)
	{
		m_pActualControl->setCurrentIndex(-1);
		m_bHasValue = false;
		return;
	}

	int index = -1;
	for (size_t i = 0; i < m_Values.size(); i++)
	{
		if (prtyFilePathComboBoxUIInfo::IsSamePath(m_Values[i], i_Value))
		{
			index = (int)i;
			break;
		}
	}

	if (index < 0)
	{
		// not one of the choices (an old or custom file): show its path
		std::string label;
		fsFileUtil::LocatorToUnicodeString(i_Value, label);
		m_pActualControl->addItem(QString::fromStdString(label));
		index = m_pActualControl->count() - 1;
		m_Values.push_back(i_Value);
	}

	m_pActualControl->setCurrentIndex(index);
	m_bHasValue = true;
}

//----------------------------------------------------------------------------
// Path of the selected entry (empty if nothing is selected)
//----------------------------------------------------------------------------
fsLocator pqtFilePath_ComboBox::GetValueFromControl() const
{
	const int selection = m_pActualControl->currentIndex();
	if (selection < 0 || selection >= (int)m_Values.size())
		return fsLocator();
	return m_Values[selection];
}

#endif // USE_QT
