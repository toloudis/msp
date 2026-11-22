/****************************************************************************\
**	tqcColorPicker.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcColorPicker.hpp"

#include "ToolUIQt/tqc/private/tqcColorDialog.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcColorPicker::tqcColorPicker(QWidget* i_pParent)
:	QPushButton(i_pParent)
{
	this->setMinimumWidth(20);
	QObject::connect(this, &QPushButton::clicked, [this]() { OnButtonClick(); });
	SetValue(m_Value);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcColorPicker::~tqcColorPicker()
{
	// if our color dialog is still up, close it
	if (m_pColorDialog && (m_pColorDialog == tqcColorDialog::Instance))
	{
		m_pColorDialog->SetColorChangedCallback(tqcColorDialog::ColorChangedFunction());
		m_pColorDialog->close();
	}
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maFloatRGBA& tqcColorPicker::GetValue() const
{
	return m_Value;
}
void tqcColorPicker::SetValue(const maFloatRGBA& i_Value)
{
	m_Value = i_Value;
	QColor color = tqcToQColor(m_Value);
	this->setStyleSheet(QString("QPushButton { background-color: %1; border: 1px solid gray; }").arg(color.name()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcColorPicker::OnButtonClick()
{
	// If we already have a color dialog, then don't do anything
	if (m_pColorDialog && (m_pColorDialog == tqcColorDialog::Instance))
	{
		m_pColorDialog->raise();
		m_pColorDialog->activateWindow();
		return;
	}

	// if a color picker is already up, close it
	if (tqcColorDialog::Instance)
	{
		tqcColorDialog::Instance->close();
	}

	m_pColorDialog = new tqcColorDialog(tqtSystem::g_pMainForm, m_Value);
	tqcColorDialog::Instance = m_pColorDialog;
	m_pColorDialog->SetColorChangedCallback([this]() { OnColorDialogChange(); });
	m_pColorDialog->show();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcColorPicker::OnColorDialogChange()
{
	if (m_pColorDialog)
	{
		maFloatRGBA new_value = m_pColorDialog->GetValue();
		if (!(new_value == m_Value))
		{
			this->SetValue( new_value );

			// trigger the callback
			NotifyValueChanged();
		}
	}
}

#endif // USE_QT
