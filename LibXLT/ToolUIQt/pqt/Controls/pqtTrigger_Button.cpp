/****************************************************************************\
**	pqtTrigger_Button.hpp
**
**		Intermediate class between the property (prtyTrigger) and 
**	the control (Button).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtTrigger_Button.hpp"
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"

#include "Core/prty/prtyTrigger.hpp"
#include "Core/prty/prtyButtonUIInfo.hpp"

#ifdef USE_QT

namespace
{
	// Create derivation of widget that notifies the control manager
	// when it is deleted by Qt
	class CleanUpButton : public QPushButton
	{
	public:
		CleanUpButton(QWidget* i_pParent)
			: QPushButton(i_pParent) {}
		~CleanUpButton()
		{
			pqtControlMgr::DeleteControl(this);
		}
	};
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pqtTrigger_Button::pqtTrigger_Button(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										 QWidget* i_pParent)
:	pqtControl(i_pUIInfo)
{
	DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");
	// create the actual control
	//m_pActualControl = new wxButton(i_pParent, wxID_ANY, wxT(""));
	m_pActualControl = new CleanUpButton(i_pParent);

	//	set the control values
	this->UpdateControl(i_pUIInfo.get());

	//	hook up events
	//
	QObject::connect(m_pActualControl, &QPushButton::clicked, [this](bool) { button_Clicked(); });
};

//----------------------------------------------------------------------------
//	Destructor - dispose of controls
//----------------------------------------------------------------------------
pqtTrigger_Button::~pqtTrigger_Button()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
QWidget* pqtTrigger_Button::GetControl()
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
void pqtTrigger_Button::PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty)
{
	// Nothing to do in button property changed
	//if (m_bLocalChangeNoUpdate) return;

	//const bool newvalue = (static_cast<prtyTrigger*>(i_pProperty))->GetValue();

	//m_bLocalChangeNoUpdate = true;
	//if ( m_pActualControl->GetValue() != newvalue )
	//{
	//	m_pActualControl->SetValue(newvalue);
	//}
	//m_bLocalChangeNoUpdate = false;
}

//----------------------------------------------------------------------------
// Update control to match the changes in the UI Info
//----------------------------------------------------------------------------
void pqtTrigger_Button::UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
{
	m_pActualControl->setEnabled(!i_pUIInfo->GetReadOnly());

	prtyButtonUIInfo* pBUII = static_cast<prtyButtonUIInfo*>(i_pUIInfo);
	m_pActualControl->setText( tqtQString(pBUII->m_ButtonText) );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pqtTrigger_Button::button_Clicked()
{
	if (m_bLocalChangeNoUpdate) return;

	m_bLocalChangeNoUpdate = true;
	//if (m_OriginalValue != m_pActualControl->Checked)
	{
		const int num_properties = this->GetNumberOfProperties();
		if (num_properties > 1)
			undoUndoMgr::BeginMultipleOperationBlock();

		for (int i=0; i < num_properties; ++i)
		{
			// Toggle value of property to trigger callback
			prtyTrigger* pActualProperty = static_cast<prtyTrigger*>(GetProperty(i));
			pActualProperty->TriggerCallbacks();
		}

		if (num_properties > 1)
			undoUndoMgr::EndMultipleOperationBlock();
	}
	m_bLocalChangeNoUpdate = false;
}
#endif
