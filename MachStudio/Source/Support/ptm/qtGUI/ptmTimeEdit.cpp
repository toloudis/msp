/****************************************************************************\
**	ptmTimeEdit.cpp
**
**	see .hpp
**	
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/ptm/qtGUI/ptmTimeEdit.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptmTimeEdit::ptmTimeEdit(QWidget* i_pParent)
:	QLineEdit("0", i_pParent)
{		
	// editingFinished comes on ENTER and when the control loses focus
	QObject::connect(this, &QLineEdit::editingFinished, [this]() { text_changed(); });

	// Add this control as a time interest
	tmlnTimeLine::AddTimeInterest(this);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptmTimeEdit::~ptmTimeEdit()
{
	// Unregister time interest
	tmlnTimeLine::RemoveTimeInterest(this);
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maTime& ptmTimeEdit::GetTimeValue() const
{
	return m_TimeValue;
}
void ptmTimeEdit::SetTimeValue(const maTime& i_Value)
{
	m_TimeValue = i_Value;
	std::string time_string;
	tmlnTimeUtil::GetTimeString(m_TimeValue, time_string);
	QString text = tqtQString(time_string);
	if (this->text() != text)
		this->setText(text);
}

//--------------------------------------------------------------------
//	TimeFormatChanged - timeline time display format has changed
//--------------------------------------------------------------------
//virtual 
void ptmTimeEdit::TimeFormatChanged( int i_TimeFormat )
{
	// Force through a "Set" again in order to alter text in the text box
	// to fit the new time format
	this->SetTimeValue(m_TimeValue);
}

//--------------------------------------------------------------------
//	FrameRateChanged - timeline frame rate (frames per second) changed
//--------------------------------------------------------------------
//virtual 
void ptmTimeEdit::FrameRateChanged( float i_FrameRate )
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ptmTimeEdit::text_changed()
{
	std::string text = tqtStdString(this->text());
	if (!text.empty())
	{
		maTime new_value = m_TimeValue;
		if (tmlnTimeUtil::ParseTimeString(text, new_value))
		{
			if (m_TimeValue != new_value)
			{
				m_TimeValue = new_value;

				// trigger the callback
				NotifyValueChanged();
			}
		}
		else
		{
			// Error parsing time string, display error message
			std::string message("Please enter time in the current time format: ");
			std::string time_format = tmlnTimeUtil::GetTimeFormatString();
			message += time_format;
			guiMessageBox::Show(message.c_str(), "Time entry parsing");
		}
	}
}

#endif // USE_QT
