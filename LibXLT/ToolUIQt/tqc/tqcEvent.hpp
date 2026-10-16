/****************************************************************************\
**	tqcEvent.hpp
**
**	Value Changed notification for the custom controls: the value is
**	different after ENTER is pressed, or the control is left, or the
**	user changed it some other way (slider, spinner, picker...).
**
**	This replaces the wxEVT_VALUE_CHANGED event of the wxWidgets controls.
**	It is a plain callback so the controls don't need Qt's moc.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_EVENT_HPP
#error tqcEvent.hpp multiply included
#endif
#define TQC_EVENT_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <functional>

#ifdef USE_QT

//============================================================================
//============================================================================
class tqcValueChangedNotifier
{
public:
	typedef std::function<void()> ValueChangedFunction;

	//--------------------------------------------------------------------
	// Set the function to call when the user changes the value
	//--------------------------------------------------------------------
	void SetValueChangedCallback(ValueChangedFunction i_Callback)
	{
		m_ValueChanged = i_Callback;
	}

	//--------------------------------------------------------------------
	// Call the value changed callback, if there is one
	//--------------------------------------------------------------------
	void NotifyValueChanged()
	{
		if (m_ValueChanged)
			m_ValueChanged();
	}

private:
	ValueChangedFunction m_ValueChanged;
};

#endif // USE_QT
