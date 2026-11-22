/****************************************************************************\
**	tqtInputFeed.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtInputFeed.hpp"

#include "Input/in/inDeviceMgr.hpp"

#ifdef USE_QT

namespace tqtInputFeed
{
	namespace
	{
		//--------------------------------------------------------------------
		// Returns the inMouse button for a Qt button, or -1
		//--------------------------------------------------------------------
		int mouse_button_index(Qt::MouseButton i_Button)
		{
			switch (i_Button)
			{
			case Qt::LeftButton:	return inMouse::e_LEFT;
			case Qt::RightButton:	return inMouse::e_RIGHT;
			case Qt::MiddleButton:	return inMouse::e_MIDDLE;
			case Qt::BackButton:	return inMouse::e_EXTRA1;
			case Qt::ForwardButton:	return inMouse::e_EXTRA2;
			default:				return -1;
			}
		}

		//--------------------------------------------------------------------
		// Passes key events to the keyboard
		//--------------------------------------------------------------------
		void feed_key(QKeyEvent* i_pEvent, bool i_bDown)
		{
			inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
			// Keys Qt makes up itself have no scan code
			if (pKeyboard && i_pEvent->nativeScanCode() != 0)
				pKeyboard->SetNativeKeyState(i_pEvent->nativeScanCode(), i_bDown);
		}

		//--------------------------------------------------------------------
		// Marks every key as up, for when the key-up events will go to
		// another window or application
		//--------------------------------------------------------------------
		void release_keys()
		{
			if (inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard())
				pKeyboard->ReleaseAll();
		}

		void reset_mouse_motion();

		// ----------------------------------------------------------------------------
		// Watches every key and mouse button event in the application. It
		// never stops an event.
		// ----------------------------------------------------------------------------
		class InputFilter : public QObject
		{
		public:
			virtual bool eventFilter(QObject* i_pWatched, QEvent* i_pEvent)
			{
				switch (i_pEvent->type())
				{
				// A key bound to a shortcut arrives only as a ShortcutOverride
				case QEvent::ShortcutOverride:
				case QEvent::KeyPress:
					feed_key(static_cast<QKeyEvent*>(i_pEvent), true);
					break;

				case QEvent::KeyRelease:
					if (!static_cast<QKeyEvent*>(i_pEvent)->isAutoRepeat())
						feed_key(static_cast<QKeyEvent*>(i_pEvent), false);
					break;

				case QEvent::MouseButtonPress:
				case QEvent::MouseButtonDblClick:
				{
					inMouse* pMouse = inDeviceMgr::GetMouse();
					const int button = mouse_button_index(static_cast<QMouseEvent*>(i_pEvent)->button());
					if (pMouse && button >= 0)
						pMouse->NotifyButtonPressed(button);
					break;
				}

				case QEvent::WindowDeactivate:
					release_keys();
					break;

				case QEvent::ApplicationStateChange:
					if (static_cast<QApplicationStateChangeEvent*>(i_pEvent)->applicationState() != Qt::ApplicationActive)
						release_keys();
					// The app doesn't think while it is inactive
					reset_mouse_motion();
					break;

				default:
					break;
				}
				return QObject::eventFilter(i_pWatched, i_pEvent);
			}
		};

		// ----------------------------------------------------------------------------
		// Reads the cursor and buttons from Qt
		// ----------------------------------------------------------------------------
		class MouseSource : public inMouseSource
		{
		public:
			MouseSource() : m_bHaveLastPos(false), m_X(0), m_Y(0) {}

			virtual bool Sample(int& o_ScreenX, int& o_ScreenY, unsigned int& o_Buttons)
			{
				if (QGuiApplication::applicationState() != Qt::ApplicationActive)
				{
					m_bHaveLastPos = false;
					return false;
				}

				// Qt positions are in device independent pixels. Move in
				// physical pixels, at the scale of the screen the cursor
				// is on, so the mouse feels the same on every screen.
				const QPoint pos = QCursor::pos();
				if (m_bHaveLastPos)
				{
					const QScreen* pScreen = QGuiApplication::screenAt(pos);
					const qreal scale = pScreen ? pScreen->devicePixelRatio() : 1.0;
					m_X += qRound((pos.x() - m_LastPos.x()) * scale);
					m_Y += qRound((pos.y() - m_LastPos.y()) * scale);
				}
				m_LastPos = pos;
				m_bHaveLastPos = true;

				o_ScreenX = m_X;
				o_ScreenY = m_Y;

				const Qt::MouseButtons buttons = QGuiApplication::mouseButtons();
				o_Buttons = 0;
				const Qt::MouseButton c_Buttons[] = { Qt::LeftButton, Qt::RightButton, Qt::MiddleButton,
													  Qt::BackButton, Qt::ForwardButton };
				for (Qt::MouseButton button : c_Buttons)
				{
					if (buttons & button)
						o_Buttons |= 1u << mouse_button_index(button);
				}
				return true;
			}

			//--------------------------------------------------------------------
			// Forget the last cursor position, so the next Sample adds no motion
			//--------------------------------------------------------------------
			void ResetMotion()
			{
				m_bHaveLastPos = false;
			}

		private:
			bool m_bHaveLastPos;
			QPoint m_LastPos;
			int m_X;	// physical pixels moved
			int m_Y;
		};

		InputFilter* l_pInputFilter = NULL;
		MouseSource* l_pMouseSource = NULL;

		//--------------------------------------------------------------------
		// Forgets the cursor position, so motion made while the app wasn't
		// thinking doesn't arrive all at once
		//--------------------------------------------------------------------
		void reset_mouse_motion()
		{
			if (l_pMouseSource)
				l_pMouseSource->ResetMotion();
			if (inMouse* pMouse = inDeviceMgr::GetMouse())
				pMouse->ResetMotion();
		}

	} // local namespace

	//--------------------------------------------------------------------
	// Init starts feeding the input package
	//--------------------------------------------------------------------
	void Init()
	{
		if (l_pInputFilter || !qApp)
			return;

		// Installed after tqtAppUtil's hot key filter, so it runs first and
		// sees the keys that the hot keys use up
		l_pInputFilter = new InputFilter;
		qApp->installEventFilter(l_pInputFilter);

		l_pMouseSource = new MouseSource;
		if (inMouse* pMouse = inDeviceMgr::GetMouse())
			pMouse->SetSource(l_pMouseSource);
	}

	//--------------------------------------------------------------------
	// CleanUp stops feeding the input package
	//--------------------------------------------------------------------
	void CleanUp()
	{
		if (!l_pInputFilter)
			return;

		if (inMouse* pMouse = inDeviceMgr::GetMouse())
			pMouse->SetSource(NULL);
		delete l_pMouseSource;
		l_pMouseSource = NULL;

		if (qApp)
			qApp->removeEventFilter(l_pInputFilter);
		delete l_pInputFilter;
		l_pInputFilter = NULL;
	}
}

#endif // USE_QT
