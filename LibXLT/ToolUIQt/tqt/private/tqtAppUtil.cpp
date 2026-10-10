/*****************************************************************************
**  tqtAppUtil.cpp
**
**     see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtAppUtil.hpp"

#include "ToolUIQt/tqt/tqtMessaging.hpp"

#ifdef USE_QT

namespace tqtAppUtil
{
	namespace
	{
		// QApplication keeps references to argc and argv, so they have to
		// outlive it.
		int l_Argc = 0;
		char l_AppName[260] = "";
		char* l_Argv[2] = { l_AppName, NULL };

		QApplication* l_pApplication = NULL;

		// ----------------------------------------------------------------------------
		// Determine if the widget with focus wants hot keys
		// ----------------------------------------------------------------------------
		bool window_wants_hotkeys()
		{
			QWidget *pWindow = QApplication::focusWidget();

			// Keys typed into text fields, or into a control that records
			// hot keys, belong to that control.
			if (pWindow)
			{
				if (pWindow->property(tqtMessaging::c_NoHotKeysProperty).toBool())
					return false;
				QLineEdit* pLineEdit = qobject_cast<QLineEdit*>(pWindow);
				if (pLineEdit && !pLineEdit->isReadOnly())
					return false;
				QPlainTextEdit* pPlainTextEdit = qobject_cast<QPlainTextEdit*>(pWindow);
				if (pPlainTextEdit && !pPlainTextEdit->isReadOnly())
					return false;
				QTextEdit* pTextEdit = qobject_cast<QTextEdit*>(pWindow);
				if (pTextEdit && !pTextEdit->isReadOnly())
					return false;
				if (qobject_cast<QAbstractSpinBox*>(pWindow))
					return false;
			}

			// Start with widget that has focus and search up through
			// parents and see if any parent has registered a desire
			// to process hot keys.
			while (pWindow)
			{
				if (tqtMessaging::WantsHotKeys(pWindow))
					return true;
				pWindow = pWindow->parentWidget();
			}
			return false;
		}

		// ----------------------------------------------------------------------------
		// Watches every key press in the application in order to process
		// hot keys, the way wxApp::FilterEvent did.
		// ----------------------------------------------------------------------------
		class HotKeyFilter : public QObject
		{
		public:
			virtual bool eventFilter(QObject* i_pWatched, QEvent* i_pEvent)
			{
				if ( i_pEvent->type() == QEvent::KeyPress &&
					 i_pWatched == QApplication::focusWidget() )
				{
					if (tqtMessaging::ShouldProcessHotKeys())
					{
						if (window_wants_hotkeys())
						{
							// Handle hot keys here
							return tqtMessaging::ProcessKeyEvent(*static_cast<QKeyEvent*>(i_pEvent));
						}
					}
				}
				return QObject::eventFilter(i_pWatched, i_pEvent);
			}
		};

		HotKeyFilter* l_pHotKeyFilter = NULL;

	} // local namespace

	//--------------------------------------------------------------------
	// Initializes Qt, returns true if successful.
	//--------------------------------------------------------------------
	bool Init(const char* i_pAppName)
	{
		if (l_pApplication)
			return true;

		strncpy_s(l_AppName, i_pAppName ? i_pAppName : "", _TRUNCATE);
		l_Argc = 1;

		// Each panel draws with DirectX into its own native child window
		QApplication::setAttribute(Qt::AA_NativeWindows, false);
		QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);

		l_pApplication = new QApplication(l_Argc, l_Argv);
		QApplication::setQuitOnLastWindowClosed(false);

		l_pHotKeyFilter = new HotKeyFilter;
		l_pApplication->installEventFilter(l_pHotKeyFilter);

		return true;
	}

	//--------------------------------------------------------------------
	// Runs the application loop
	//--------------------------------------------------------------------
	void Run()
	{
		// app execution
		if (l_pApplication)
			l_pApplication->exec();
	}

	//--------------------------------------------------------------------
	// DeInitializes Qt
	//--------------------------------------------------------------------
	void CleanUp()
	{
		if (l_pApplication)
		{
			l_pApplication->removeEventFilter(l_pHotKeyFilter);
			delete l_pHotKeyFilter;
			l_pHotKeyFilter = NULL;

			delete l_pApplication;
			l_pApplication = NULL;
		}
	}
}

#endif // USE_QT
