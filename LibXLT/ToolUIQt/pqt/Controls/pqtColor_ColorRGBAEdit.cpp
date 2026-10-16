/****************************************************************************\
**	pqtColor_ColorRGBAEdit.hpp
**
**		Intermediate class between the property (prtyColor) and 
**	the control (ColorRGBAEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtColor_ColorRGBAEdit.hpp"
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"
#ifndef TQC_EVENT_HPP
#include "ToolUIQt/tqc/tqcEvent.hpp"
#endif

#include "Core/prty/prtyColor.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pqtColor_ColorRGBAEdit::pqtColor_ColorRGBAEdit(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										 QWidget* i_pParent)
:	pqtControl(i_pUIInfo)
{
	DBG_ASSERT( i_pUIInfo != 0, "UI Info cannot be NULL");

	maFloatRGBA newvalue;
	bool bSameValue = pqtControlUtil::GetCommonValue<maFloatRGBA, prtyColor>(i_pUIInfo, newvalue);

	// create the actual control
	m_pActualControl = new pqtControlUtil::CleanUpWidget<tqcColorRGBAEdit>(i_pParent);
	//m_pActualControl = new tqcColorRGBAEdit(i_pParent);

	//	set the control values
	this->UpdateControl(i_pUIInfo.get());

	if (bSameValue)
	{
		m_pActualControl->SetValue(newvalue);
	}

	//	hook up events
	//
	m_pActualControl->SetValueChangedCallback([this]() { ColorRGBAEdit_ValueChanged(); });
};

//----------------------------------------------------------------------------
//	Destructor - dispose of controls
//----------------------------------------------------------------------------
pqtColor_ColorRGBAEdit::~pqtColor_ColorRGBAEdit()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
QWidget* pqtColor_ColorRGBAEdit::GetControl()
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
void pqtColor_ColorRGBAEdit::PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty)
{
	if (m_bLocalChangeNoUpdate) return;

	const maFloatRGBA newvalue = (static_cast<prtyColor*>(i_pProperty))->GetValue();

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
void pqtColor_ColorRGBAEdit::UpdateControl(prtyPropertyUIInfo* i_pUIInfo)
{
	m_pActualControl->setEnabled(!i_pUIInfo->GetReadOnly());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pqtColor_ColorRGBAEdit::ColorRGBAEdit_ValueChanged()
{
	if (m_bLocalChangeNoUpdate) return;

	m_bLocalChangeNoUpdate = true;
	//if (m_OriginalValue != m_pActualControl->GetValue())
	{
		pqtControlUtil::SetCommonValue<maFloatRGBA, prtyColor>(this, m_pActualControl->GetValue());
	}
	m_bLocalChangeNoUpdate = false;
}
#endif
