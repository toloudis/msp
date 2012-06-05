/****************************************************************************\
**	pwxGradient_GradientRGBAEdit.hpp
**
**		Intermediate class between the property (prtyGradient) and 
**	the control (GradientColorEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxGradient_GradientColorEdit.hpp"
#include "ToolUIWx/pwx/Controls/pwxControlUtil.hpp"

#include "ToolUIWx/twc/twcEvent.hpp"

#include "Core/prty/prtyGradient.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pwxGradient_GradientColorEdit::pwxGradient_GradientColorEdit(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										 wxWindow* i_pParent)
:	pwxControl(i_pUIInfo)
{
	DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

	maGradient newvalue;
	bool bSameValue = pwxControlUtil::GetCommonValue<maGradient, prtyGradient>(i_pUIInfo, newvalue);

	// create the actual control
	m_pActualControl = new pwxControlUtil::CleanUpWidget<twcGradientColorEdit>(i_pParent);
	//m_pActualControl = new twcGradientColorEdit(i_pParent);

	//	set the control values
	this->UpdateControl(i_pUIInfo.get());

	if (bSameValue)
	{
		m_pActualControl->SetValue(newvalue);
	}

	//	hook up events
	//
	this->Connect( m_pActualControl->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(pwxGradient_GradientColorEdit::GradientColorEdit_ValueChanged) );
	m_pActualControl->PushEventHandler(this);
};

//----------------------------------------------------------------------------
//	Destructor - dispose of controls
//----------------------------------------------------------------------------
pwxGradient_GradientColorEdit::~pwxGradient_GradientColorEdit()
{
	m_pActualControl->RemoveEventHandler(this);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
wxWindow* pwxGradient_GradientColorEdit::GetControl()
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
void pwxGradient_GradientColorEdit::PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty)
{
	if (m_bLocalChangeNoUpdate) return;

	const maGradient newvalue = (static_cast<prtyGradient*>(i_pProperty))->GetValue();

	m_bLocalChangeNoUpdate = true;
	if (!(m_pActualControl->GetValue() == newvalue))
	{
		m_pActualControl->SetValue(newvalue);
	}
	m_bLocalChangeNoUpdate = false;
}

//----------------------------------------------------------------------------
// Update control to match the changes in the UI Info
//----------------------------------------------------------------------------
void pwxGradient_GradientColorEdit::UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
{
	m_pActualControl->Enable(!i_pUIInfo->GetReadOnly());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pwxGradient_GradientColorEdit::GradientColorEdit_ValueChanged(wxCommandEvent& i_Event)
{
	if (m_bLocalChangeNoUpdate) return;

	m_bLocalChangeNoUpdate = true;
	//if (m_OriginalValue != m_pActualControl->GetValue())
	{
		pwxControlUtil::SetCommonValue<maGradient, prtyGradient>(this, m_pActualControl->GetValue());
	}
	m_bLocalChangeNoUpdate = false;
}
#endif
