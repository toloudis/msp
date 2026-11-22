/****************************************************************************\
**	tqcHotKeyCtrl.hpp
**
**	Read-only text box that shows the hot key typed into it
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_HOTKEYCTRL_HPP
#error tqcHotKeyCtrl.hpp multiply included
#endif
#define TQC_HOTKEYCTRL_HPP

#ifndef TQC_EVENT_HPP
#include "ToolUIQt/tqc/tqcEvent.hpp"
#endif

#ifdef USE_QT

//============================================================================
//============================================================================
class tqcHotKeyCtrl : public QLineEdit, public tqcValueChangedNotifier
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcHotKeyCtrl(QWidget* i_pParent);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		QString GetValue() const { return this->text(); }
		void SetValue(const QString& i_Value) { this->setText(i_Value); }

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void keyPressEvent(QKeyEvent* i_pEvent);
		virtual bool event(QEvent* i_pEvent);
};

#endif // USE_QT
