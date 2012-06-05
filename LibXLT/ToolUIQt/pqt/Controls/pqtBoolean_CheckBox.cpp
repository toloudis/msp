/****************************************************************************\
**	pqtBoolean_CheckBox.hpp
**
**		Intermediate class between the property (prtyBoolean) and 
**	the control (CheckBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtBoolean_CheckBox.hpp"
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"

#include "Core/prty/prtyBoolean.hpp"


#ifdef USE_QT

//============================================================================
//============================================================================
namespace
{
	// Create derivation of widget that notifies the control manager
	// when it is deleted by wxWidgets
	class CleanUpCheckBox : public QCheckBox
	{
	public:
		CleanUpCheckBox(QWidget* i_pParent)
			: QCheckBox(i_pParent) {}
		~CleanUpCheckBox()
		{
			pqtControlMgr::DeleteControl(this);
		}
	};
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pqtBoolean_CheckBox::pqtBoolean_CheckBox(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										 QWidget* i_pParent)
:	pqtControl(i_pUIInfo)
{
	DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

	bool newvalue = false;
	bool bSameValue = pqtControlUtil::GetCommonValue<bool, prtyBoolean>(i_pUIInfo, newvalue);

	// create the actual control
	m_pActualControl = new CleanUpCheckBox(i_pParent);
	//m_pActualControl = new QCheckBox(i_pParent, wxID_ANY, wxT(""));

	//	set the control values
	this->UpdateControl(i_pUIInfo.get());

	if (bSameValue)
	{
		m_pActualControl->setChecked(newvalue);
	}

#ifdef QT_FINISH_PORT
	//	hook up events
	//
	this->Connect( m_pActualControl->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
					wxCommandEventHandler(pqtBoolean_CheckBox::CheckBox_ValueChanged) );
	m_pActualControl->PushEventHandler(this);
#endif
};

//----------------------------------------------------------------------------
//	Destructor - dispose of controls
//----------------------------------------------------------------------------
pqtBoolean_CheckBox::~pqtBoolean_CheckBox()
{
#ifdef QT_FINISH_PORT
	m_pActualControl->RemoveEventHandler(this);
#endif
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
QWidget* pqtBoolean_CheckBox::GetControl()
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
void pqtBoolean_CheckBox::PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty)
{
	if (m_bLocalChangeNoUpdate) return;

	const bool newvalue = (static_cast<prtyBoolean*>(i_pProperty))->GetValue();

	m_bLocalChangeNoUpdate = true;
	if ( m_pActualControl->isChecked() != newvalue )
	{
		m_pActualControl->setChecked(newvalue);
	}
	m_bLocalChangeNoUpdate = false;
}

//----------------------------------------------------------------------------
// Update control to match the changes in the UI Info
//----------------------------------------------------------------------------
void pqtBoolean_CheckBox::UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
{
	m_pActualControl->setDisabled(i_pUIInfo->GetReadOnly());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#ifdef QT_FINISH_PORT
void pqtBoolean_CheckBox::CheckBox_ValueChanged(wxCommandEvent& i_Event)
{
	if (m_bLocalChangeNoUpdate) return;

	m_bLocalChangeNoUpdate = true;
	//if (m_OriginalValue != m_pActualControl->GetValue())
	{
		pqtControlUtil::SetCommonValue<bool, prtyBoolean>(this, m_pActualControl->GetValue());
	}
	m_bLocalChangeNoUpdate = false;
}
#endif
#endif	// USE_QT

