/****************************************************************************\
**	tqcHotKeyCtrl.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcHotKeyCtrl.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "ToolUIQt/tqt/tqtMessaging.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcHotKeyCtrl::tqcHotKeyCtrl(QWidget* i_pParent)
:	QLineEdit(i_pParent)
{
	this->setReadOnly(true);
	this->setProperty(tqtMessaging::c_NoHotKeysProperty, true);
}

//----------------------------------------------------------------------------
// Take Tab and shortcut keys too, so that they can become hot keys
//----------------------------------------------------------------------------
bool tqcHotKeyCtrl::event(QEvent* i_pEvent)
{
	if (i_pEvent->type() == QEvent::ShortcutOverride)
	{
		i_pEvent->accept();
		return true;
	}
	return QLineEdit::event(i_pEvent);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcHotKeyCtrl::keyPressEvent(QKeyEvent* i_pEvent)
{
	bool bHaveHotKey = false;
	std::string hot_key_str;
	if (i_pEvent->key() == Qt::Key_Backspace)
	{
		// Backspace clears the control
		hot_key_str = "";
		bHaveHotKey = true;
	}
	else if (tqtMessaging::ConvertKeyEventToString(*i_pEvent, hot_key_str))
	{
		bHaveHotKey = true;
	}

	if (bHaveHotKey)
	{
		QString hot_key_value = tqtQString(hot_key_str);
		QString prev_key_value = this->text();
		if (hot_key_value != prev_key_value)
		{
			cmaCommandMgr::ResetValidHotKey();
			this->setText( hot_key_value );

			// trigger the callback
			NotifyValueChanged();

			//if the hot key is not valid, the text field will be empty
			if(!cmaCommandMgr::IsValidHotKey())
			{
				this->setText(prev_key_value);
				NotifyValueChanged();
			}
		}
	}
	i_pEvent->accept();
}

#endif // USE_QT
