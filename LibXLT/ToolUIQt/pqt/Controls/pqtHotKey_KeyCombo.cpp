/****************************************************************************\
**	pqtHotKey_KeyCombo.hpp
**
**		Intermediate class between the property (prtyHotKey) and 
**	the control (HotKeyCtrl).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtHotKey_KeyCombo.hpp"
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"

#include "Core/prty/prtyHotKey.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pqtHotKey_KeyCombo::pqtHotKey_KeyCombo(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										 QWidget* i_pParent)
:	pqtControl(i_pUIInfo)
{
	DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

	std::string newvalue;
	bool bSameValue = pqtControlUtil::GetCommonValue<std::string, prtyHotKey>(i_pUIInfo, newvalue);

	// create the actual control
	m_pActualControl = new pqtControlUtil::CleanUpWidget<tqcHotKeyCtrl>(i_pParent);
	//m_pActualControl = new tqcHotKeyCtrl(i_pParent, wxID_ANY, wxT(""));

	//	set the control values
	this->UpdateControl(i_pUIInfo.get());

	if (bSameValue)
	{
		m_pActualControl->SetValue(tqtQString(newvalue));
	}

	//	hook up events
	//
	m_pActualControl->SetValueChangedCallback([this]() { HotKeyCtrl_ValueChanged(); });
};

//----------------------------------------------------------------------------
//	Destructor - dispose of controls
//----------------------------------------------------------------------------
pqtHotKey_KeyCombo::~pqtHotKey_KeyCombo()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
QWidget* pqtHotKey_KeyCombo::GetControl()
{
	return m_pActualControl;
}

//----------------------------------------------------------------------------
//	Note: right now, PropertyChanged will not create a circular update because
//	of the m_bLocalChangeNoUpdate flag.  This IS NOT true the other way
//	around.  If a control calls its "ValueChanged" then the property will
//	call all of its callback controls and one of them could have been the one
//	that originally updated the property value(s).  By checking the diff of 
//	the values we can avoid a repetitive setting of the control's values.
//----------------------------------------------------------------------------
void pqtHotKey_KeyCombo::PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty)
{
	if (m_bLocalChangeNoUpdate) return;

		QString newvalue = tqtQString((static_cast<prtyHotKey*>(i_pProperty))->GetValue());

	m_bLocalChangeNoUpdate = true;
	if ( m_pActualControl->GetValue() != newvalue )
	{
		m_pActualControl->SetValue(newvalue);
	}
	m_bLocalChangeNoUpdate = false;
}

//----------------------------------------------------------------------------
// Update control to match the changes in the UI Info
//----------------------------------------------------------------------------
void pqtHotKey_KeyCombo::UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
{
	m_pActualControl->setEnabled(!i_pUIInfo->GetReadOnly());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pqtHotKey_KeyCombo::HotKeyCtrl_ValueChanged()
{
	if (m_bLocalChangeNoUpdate) return;

	m_bLocalChangeNoUpdate = true;
	//if (m_OriginalValue != m_pActualControl->GetValue())
	{
		std::string hot_key_str = tqtStdString(m_pActualControl->GetValue());
		pqtControlUtil::SetCommonValue<std::string, prtyHotKey>(this, hot_key_str);
	}
	m_bLocalChangeNoUpdate = false;
}
#endif
