/*****************************************************************************
**	quiSplashScreen.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiSplashScreen.hpp"

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif
#include "ToolUIQt/tqt/tqtSystem.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/Fs/fsFileUtil.hpp"

#include <string>


namespace
{
#ifdef USE_QT
	// How long the splash screen stays up when it times out
	const int c_SplashTimeoutMS = 5000;

	QPointer<QSplashScreen> l_Splash;
#endif

	bool m_bisSplashOn = true;
}

//----------------------------------------------------------------------------
// StartUp - display splash screen
//----------------------------------------------------------------------------
void  quiSplashScreen::StartUp(const fsLocator &i_SplashImage,
							   const std::string& i_Message)
{
	// Check if Splash Screen loading is disabled
	if(!splashscreen::GetSplashFlag()) return;

#ifdef USE_QT
	if (fsFileUtil::FileExists(i_SplashImage))
	{
		//grab the splash image location
		std::string img_location;
		fsFileUtil::LocatorToUnicodeString(i_SplashImage, img_location);

		QPixmap pixmap(QString::fromStdString(img_location));
		if (!pixmap.isNull())
		{
			delete l_Splash;
			l_Splash = new QSplashScreen(pixmap, Qt::WindowStaysOnTopHint);
			l_Splash->setAttribute(Qt::WA_DeleteOnClose);

			//for images with transparent edges, shape the window to the image
			if (pixmap.hasAlphaChannel())
				l_Splash->setMask(pixmap.mask());

			QLabel* pMessage = new QLabel(tqtQString(i_Message), l_Splash);
			pMessage->move(198, 134);
			pMessage->setStyleSheet("QLabel { color: rgb(206,206,206); background: transparent; }");
			pMessage->adjustSize();

			l_Splash->show();

			if (m_bSplashTimeout)
			{
				QSplashScreen* pSplash = l_Splash;
				QTimer::singleShot(c_SplashTimeoutMS, pSplash, &QWidget::close);
			}
		}
	}
	QApplication::processEvents();
#endif
}

//----------------------------------------------------------------------------
// ShutDown - hide the splash screen
//----------------------------------------------------------------------------
void  quiSplashScreen::ShutDown()
{
	if(!splashscreen::GetSplashFlag()) return;

#ifdef USE_QT
	if (l_Splash && !m_bSplashTimeout)
		l_Splash->close();
#endif
}

//----------------------------------------------------------------------------
// Set the duration for the splash screen
//----------------------------------------------------------------------------
void quiSplashScreen::SetDoSplashTimeout(bool i_bSplashTimeout)
{
	m_bSplashTimeout = i_bSplashTimeout;
}

//--------------------------------------------------------------------
// Check the flag to see if splash screen is enabled
//--------------------------------------------------------------------
bool splashscreen::GetSplashFlag()
{
	return m_bisSplashOn;
}

//--------------------------------------------------------------------------------
// Set the flag for splash screen. This is currently being set only by '/NoSplash'
// command line option.
//---------------------------------------------------------------------------------
void splashscreen::SetSplashFlag(bool i_bisSplashOn)
{
	m_bisSplashOn = i_bisSplashOn;
}
