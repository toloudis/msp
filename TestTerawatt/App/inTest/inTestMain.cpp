#include <stdio.h>
#include <windows.h>

#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

#include "appApplication.hpp"
#include "appApplicationPAC.hpp"
#include "appFlowEvent.hpp"
#include "appFlowEventHandler.hpp"
#include "appPackage.hpp"
#include "dbgLog.hpp"
#include "dbgPackage.hpp"
#include "envError.hpp"
#include "envPackage.hpp"
#include "g2dFontHandle.hpp"
#include "g2dFontUtil.hpp"
#include "g2dPackage.hpp"
#include "g2dRGBColor.hpp"
#include "g2dScreen.hpp"
#include "g2dScreenDrawUtil.hpp"
#include "inDeviceMgr.hpp"
#include "inPackage.hpp"
#include "inPCVJoy.hpp"
#include "itPackage.hpp"

namespace
{

	itString DigitalDirStrings[9] = {
		itString("NONE"),
		itString("NORTH"),
		itString("NORTHEAST"),
		itString("EAST"),
		itString("SOUTHEAST"),
		itString("SOUTH"),
		itString("SOUTHWEST"),
		itString("WEST"),
		itString("NORTHWEST")
	};

	enum Sticks
	{
		e_Stick0 = 0,
		e_Stick1 = 1
	};
}

class MyApp	: public appApplication,
				public appFlowEventHandler
{
public:
	
	MyApp();
	~MyApp();

	virtual void Think();

	virtual void ReceiveStartEvent(appStartEvent& i_Event);
	virtual void ReceiveStopEvent(appStopEvent& i_Event);
	virtual void ReceiveSuspendEvent(appSuspendEvent& i_Event);
	virtual void ReceiveResumeEvent(appResumeEvent& i_Event);

private:
	int m_ThinkCount;
	int m_Frame;
	g2dFontHandle m_Font;
	g2dFontHandle m_BigFont;
	inKeyboard *m_K;
	int m_KCoordX;
	int m_KCoordY;
	inMouse *m_M;
	int m_MCoordX;
	int m_MCoordY;
	inGamepad *m_G;
	int m_GCoordX;
	int m_GCoordY;
	inPCVJoy *m_VJ;
	int m_VJCoordX;
	int m_VJCoordY;
	int m_VJStick0CoordX;
	int m_VJStick0CoordY;
	int m_VJStick1CoordX;
	int m_VJStick1CoordY;
	int m_ValueOffset;
};

MyApp::MyApp() 
:	m_ThinkCount(0),
	m_Frame(0),
	m_KCoordX(50),
	m_KCoordY(100),
	m_MCoordX(700),
	m_MCoordY(100),
	m_GCoordX(50),
	m_GCoordY(550),
	m_VJCoordX(250),
	m_VJCoordY(300),
	m_VJStick0CoordX(190),
	m_VJStick0CoordY(340),
	m_VJStick1CoordX(350),
	m_VJStick1CoordY(340),
	m_ValueOffset(80)
{
	this->SetWindowSize(100, 100, 320, 240);
	this->SetWindowTitle(itString("Testing Input"));
	m_G = NULL;
}

MyApp::~MyApp() 
{
}

void MyApp::Think() 
{
	int i;
	char dummy[64];//dummy string for passing into itoa type stuff
	++m_ThinkCount;
	++m_Frame;

	g2dScreenDrawUtil::Clear(g2dRGBColor(0x00, 0x00, 0x00));

	itString strKeyboard("Keyboard: ");
	itString strMouse("Mouse: ");
	itString strGamepad("Gamepad: ");
	itString strVJ("VirtualJoystick");
	itString strStick0("Stick 0");
	itString strStick1("Stick 1 (inverted)");

	//strKeyboard += m_K->GetDeviceName();
	//strMouse += m_M->GetDeviceName();
	//strGamepad += m_G->GetDeviceName();

	itString strKDisplayPressed("Pressed: ");
	itString strMDisplayPressed("Pressed: ");
	itString strGDisplayPressed("Pressed: ");
	itString strVJDisplayPressed("Pressed: ");
	itString strKDisplayHeld("Held: ");
	itString strMDisplayHeld("Held: ");
	itString strGDisplayHeld("Held: ");
	itString strVJDisplayHeld("Held: ");
	itString strKDisplayReleased("Released: ");
	itString strMDisplayReleased("Released: ");
	itString strGDisplayReleased("Released: ");
	itString strVJDisplayReleased("Released: ");

	itString strMDisplaydX("dX: ");
	itString strMDisplaydY("dY: ");
	itString strMDisplaydZ("dZ: ");

	itString strGDisplaydX("dX: ");
	itString strGDisplaydY("dY: ");
	itString strGDisplaydZ("dZ: ");

	itString strGDisplayAnalogDir("AnalogDir: ");
	itString strGDisplayPressure("Pressure: ");
	itString strGDisplayTwist("Twist: ");
	itString strGDisplayDigitalDir("DigitalDir: ");

	itString strVJDisplayAnalogDir("AnalogDir: ");
	itString strVJDisplayPressure("Pressure: ");
	itString strVJDisplayTwist("Twist: ");
	itString strVJDisplayDigitalDir("DigitalDir: ");

	itString strVJDisplaydX("dX: ");
	itString strVJDisplaydY("dY: ");
	itString strVJDisplaydZ("dZ: ");

	g2dScreenDrawUtil::DrawText(m_KCoordX, m_KCoordY - 80, m_BigFont, strKeyboard, g2dRGBColor(0x00, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText(m_MCoordX, m_MCoordY - 80, m_BigFont, strMouse, g2dRGBColor(0x00, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText(m_GCoordX, m_GCoordY - 80, m_BigFont, strGamepad, g2dRGBColor(0x00, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText(m_VJCoordX, m_VJCoordY - 80, m_BigFont, strVJ, g2dRGBColor(0x00, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText(m_VJStick0CoordX, m_VJStick0CoordY - 25, m_BigFont, strStick0, g2dRGBColor(0x00, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText(m_VJStick1CoordX, m_VJStick1CoordY - 25, m_BigFont, strStick1, g2dRGBColor(0x00, 0x00, 0xFF));


	// draw out all the labels

	g2dScreenDrawUtil::DrawText(m_KCoordX, m_KCoordY - 60, m_Font, strKDisplayPressed, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_MCoordX, m_MCoordY - 60, m_Font, strMDisplayPressed, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_GCoordX, m_GCoordY - 60, m_Font, strGDisplayPressed, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJCoordX, m_VJCoordY - 60, m_Font, strVJDisplayPressed, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_KCoordX, m_KCoordY - 40, m_Font, strKDisplayHeld, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_MCoordX, m_MCoordY - 40, m_Font, strMDisplayHeld, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_GCoordX, m_GCoordY - 40, m_Font, strGDisplayHeld, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJCoordX, m_VJCoordY - 40, m_Font, strVJDisplayHeld, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_KCoordX, m_KCoordY - 20, m_Font, strKDisplayReleased, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_MCoordX, m_MCoordY - 20, m_Font, strMDisplayReleased, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_GCoordX, m_GCoordY - 20, m_Font, strGDisplayReleased, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJCoordX, m_VJCoordY - 20, m_Font, strVJDisplayReleased, g2dRGBColor(0xa0, 0x00, 0xa0));

	g2dScreenDrawUtil::DrawText(m_MCoordX, m_MCoordY, m_Font, strMDisplaydX, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_MCoordX, m_MCoordY + 20, m_Font, strMDisplaydY, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_MCoordX, m_MCoordY + 40, m_Font, strMDisplaydZ, g2dRGBColor(0xa0, 0x00, 0xa0));

	g2dScreenDrawUtil::DrawText(m_GCoordX, m_GCoordY, m_Font, strGDisplaydX, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_GCoordX, m_GCoordY + 20, m_Font, strGDisplaydY, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_GCoordX, m_GCoordY + 40, m_Font, strGDisplaydZ, g2dRGBColor(0xa0, 0x00, 0xa0));

	g2dScreenDrawUtil::DrawText(m_GCoordX, m_GCoordY + 60, m_Font, strGDisplayAnalogDir, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_GCoordX, m_GCoordY + 80, m_Font, strGDisplayPressure, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_GCoordX, m_GCoordY + 100, m_Font, strGDisplayTwist, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_GCoordX, m_GCoordY + 120, m_Font, strGDisplayDigitalDir, g2dRGBColor(0xa0, 0x00, 0xa0));

	g2dScreenDrawUtil::DrawText(m_VJStick0CoordX, m_VJStick0CoordY, m_Font, strVJDisplaydX, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJStick0CoordX, m_VJStick0CoordY + 20, m_Font, strVJDisplaydY, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJStick0CoordX, m_VJStick0CoordY + 40, m_Font, strVJDisplaydZ, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJStick0CoordX, m_VJStick0CoordY + 60, m_Font, strVJDisplayAnalogDir, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJStick0CoordX, m_VJStick0CoordY + 80, m_Font, strVJDisplayPressure, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJStick0CoordX, m_VJStick0CoordY + 100, m_Font, strVJDisplayTwist, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJStick0CoordX, m_VJStick0CoordY + 120, m_Font, strVJDisplayDigitalDir, g2dRGBColor(0xa0, 0x00, 0xa0));

	g2dScreenDrawUtil::DrawText(m_VJStick1CoordX, m_VJStick1CoordY, m_Font, strVJDisplaydX, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJStick1CoordX, m_VJStick1CoordY + 20, m_Font, strVJDisplaydY, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJStick1CoordX, m_VJStick1CoordY + 40, m_Font, strVJDisplaydZ, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJStick1CoordX, m_VJStick1CoordY + 60, m_Font, strVJDisplayAnalogDir, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJStick1CoordX, m_VJStick1CoordY + 80, m_Font, strVJDisplayPressure, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJStick1CoordX, m_VJStick1CoordY + 100, m_Font, strVJDisplayTwist, g2dRGBColor(0xa0, 0x00, 0xa0));
	g2dScreenDrawUtil::DrawText(m_VJStick1CoordX, m_VJStick1CoordY + 120, m_Font, strVJDisplayDigitalDir, g2dRGBColor(0xa0, 0x00, 0xa0));

	inDeviceMgr::Think();

	strKDisplayPressed = "";
	strMDisplayPressed = "";
	strGDisplayPressed = "";
	strVJDisplayPressed = "";
	strKDisplayHeld = "";
	strMDisplayHeld = "";
	strGDisplayHeld = "";
	strVJDisplayHeld = "";
	strKDisplayReleased = "";
	strMDisplayReleased = "";
	strGDisplayReleased = "";
	strVJDisplayReleased = "";

	strMDisplaydX = "";
	strMDisplaydY = "";
	strMDisplaydZ = "";

	strGDisplaydX = "";
	strGDisplaydY = "";
	strGDisplaydZ = "";

	strGDisplayAnalogDir = "";
	strGDisplayPressure = "";
	strGDisplayTwist = "";
	strGDisplayDigitalDir = "";

	strVJDisplayAnalogDir = "";
	strVJDisplayPressure = "";
	strVJDisplayTwist = "";
	strVJDisplayDigitalDir = "";

	strVJDisplaydX = "";
	strVJDisplaydY = "";
	strVJDisplaydZ = "";

	for (i = -1; i < inKeys::e_NUMTERAWATTKEYS; ++i)
	{
		if (m_K->IsPressed(static_cast<inKeys::Keys>(i)))
		{
			if (-1 == i)
			{
				strKDisplayPressed += itString(" True ");
			}
			else
			{
				strKDisplayPressed += inKeys::TerawattKeyStrings[i];
				strKDisplayPressed += itString(" ");
			}
		}

		if (m_M->IsPressed(i))
		{
			if (-1 == i)
			{
				strMDisplayPressed = itString(" True ");
			}
			else
			{
				strMDisplayPressed += itString("Button ");
				strMDisplayPressed += itString(itoa(i, dummy, 10));
				strMDisplayPressed += itString(" ");
			}
		}

		if (m_G && m_G->IsPressed(i))
		{
			if (-1 == i)
			{
				strGDisplayPressed = itString(" True ");
			}
			else
			{
				strGDisplayPressed += itString("Button ");
				strGDisplayPressed += itString(itoa(i, dummy, 10));
				strGDisplayPressed += itString(" ");
			}
		}

		if (m_VJ->IsPressed(i))
		{
			if (-1 == i)
			{
				strVJDisplayPressed = itString(" True ");
			}
			else
			{
				strVJDisplayPressed += inKeys::TerawattKeyStrings[i];
				strVJDisplayPressed += itString(" ");
			}
		}
	}

	for (i = -1; i < inKeys::e_NUMTERAWATTKEYS; ++i)
	{
		if (m_K->IsHeld(static_cast<inKeys::Keys>(i)))
		{
			if (-1 == i)
			{
				strKDisplayHeld = itString(" True ");
			}
			else
			{
				strKDisplayHeld += inKeys::TerawattKeyStrings[i];
				strKDisplayHeld += itString(" ");
			}
		}

		if (m_M->IsHeld(i))
		{
			if (-1 == i)
			{
				strMDisplayHeld = itString(" True ");
			}
			else
			{
				strMDisplayHeld += itString("Button ");
				strMDisplayHeld += itString(itoa(i, dummy, 10));
				strMDisplayHeld += itString(" ");
			}
		}

		if (m_G && m_G->IsHeld(i))
		{
			if (-1 == i)
			{
				strGDisplayHeld = itString(" True ");
			}
			else
			{
				strGDisplayHeld += itString("Button ");
				strGDisplayHeld += itString(itoa(i, dummy, 10));
				strGDisplayHeld += itString(" ");
			}
		}

		if (m_VJ->IsHeld(i))
		{
			if (-1 == i)
			{
				strVJDisplayHeld = itString(" True ");
			}
			else
			{
				strVJDisplayHeld += inKeys::TerawattKeyStrings[i];
				strVJDisplayHeld += itString(" ");
			}
		}
	}

	for (i = -1; i < inKeys::e_NUMTERAWATTKEYS; ++i)
	{
		if (m_K->IsReleased(static_cast<inKeys::Keys>(i)))
		{
			if (-1 == i)
			{
				strKDisplayReleased = itString(" True ");
			}
			else
			{
				strKDisplayReleased += inKeys::TerawattKeyStrings[i];
				strKDisplayReleased += itString(" ");
			}
		}

		if (m_M->IsReleased(i))
		{
			if (-1 == i)
			{
				strMDisplayReleased = itString(" True ");
			}
			else
			{
				strMDisplayReleased += itString("Button ");
				strMDisplayReleased += itString(itoa(i, dummy, 10));
				strMDisplayReleased += itString(" ");
			}
		}

		if (m_G && m_G->IsReleased(i))
		{
			if (-1 == i)
			{
				strGDisplayReleased = itString(" True ");
			}
			else
			{
				strGDisplayReleased += itString("Button ");
				strGDisplayReleased += itString(itoa(i, dummy, 10));
				strGDisplayReleased += itString(" ");
			}
		}

		if (m_VJ->IsReleased(i))
		{
			if (-1 == i)
			{
				strVJDisplayReleased = itString(" True ");
			}
			else
			{
				strVJDisplayReleased += inKeys::TerawattKeyStrings[i];
				strVJDisplayReleased += itString(" ");
			}
		}
	}

	int dX;
	int dY;
	int intdZ;
	float dZ;
	float AnalogDir;
	float Pressure;
	float Twist;
	m_M->GetPosition(dX, dY, dZ);
	strMDisplaydX = itString(itoa(dX, dummy, 10));
	strMDisplaydY += itString(itoa(dY, dummy, 10));
	strMDisplaydZ += itString(itoa(dZ, dummy, 10));

	if (m_G)
	{
		m_G->GetAnalogDir(0, dX, dY, (int&)dZ);
		strGDisplaydX = itString(itoa(dX, dummy, 10));
		strGDisplaydY += itString(itoa(dY, dummy, 10));
		strGDisplaydZ += itString(itoa(dZ, dummy, 10));

		AnalogDir = m_G->GetAnalogDir(0, Pressure, Twist);
		sprintf(dummy, "%f", AnalogDir);
		strGDisplayAnalogDir = itString(dummy);
		sprintf(dummy, "%f", Pressure);
		strGDisplayPressure = itString(dummy);
		sprintf(dummy, "%f", Twist);
		strGDisplayTwist = itString(dummy);
		strGDisplayDigitalDir = DigitalDirStrings[m_G->GetDigitalDir(0) + 1];
	}

	m_VJ->GetAnalogDir(0, dX, dY, intdZ);
	sprintf(dummy, "%d", dX);
	strVJDisplaydX = itString(dummy);
	sprintf(dummy, "%d", dY);
	strVJDisplaydY = itString(dummy);
	sprintf(dummy, "%d", intdZ);
	strVJDisplaydZ = itString(dummy);
	AnalogDir = m_VJ->GetAnalogDir(0, Pressure, Twist);
	sprintf(dummy, "%f", AnalogDir);
	strVJDisplayAnalogDir = itString(dummy);
	sprintf(dummy, "%f", Pressure);
	strVJDisplayPressure = itString(dummy);
	sprintf(dummy, "%f", Twist);
	strVJDisplayTwist = itString(dummy);
	strVJDisplayDigitalDir = DigitalDirStrings[m_VJ->GetDigitalDir(0) + 1];

	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_KCoordX, m_KCoordY - 60, m_Font, strKDisplayPressed, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_MCoordX, m_MCoordY - 60, m_Font, strMDisplayPressed, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_GCoordX, m_GCoordY - 60, m_Font, strGDisplayPressed, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJCoordX, m_VJCoordY - 60, m_Font, strVJDisplayPressed, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_KCoordX, m_KCoordY - 40, m_Font, strKDisplayHeld, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_MCoordX, m_MCoordY - 40, m_Font, strMDisplayHeld, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_GCoordX, m_GCoordY - 40, m_Font, strGDisplayHeld, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJCoordX, m_VJCoordY - 40, m_Font, strVJDisplayHeld, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_KCoordX, m_KCoordY - 20, m_Font, strKDisplayReleased, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_MCoordX, m_MCoordY - 20, m_Font, strMDisplayReleased, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_GCoordX, m_GCoordY - 20, m_Font, strGDisplayReleased, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJCoordX, m_VJCoordY - 20, m_Font, strVJDisplayReleased, g2dRGBColor(0xFF, 0x00, 0xFF));

	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_MCoordX, m_MCoordY, m_Font, strMDisplaydX, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_MCoordX, m_MCoordY + 20, m_Font, strMDisplaydY, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_MCoordX, m_MCoordY + 40, m_Font, strMDisplaydZ, g2dRGBColor(0xFF, 0x00, 0xFF));

	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_GCoordX, m_GCoordY, m_Font, strGDisplaydX, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_GCoordX, m_GCoordY + 20, m_Font, strGDisplaydY, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_GCoordX, m_GCoordY + 40, m_Font, strGDisplaydZ, g2dRGBColor(0xFF, 0x00, 0xFF));

	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_GCoordX, m_GCoordY + 60, m_Font, strGDisplayAnalogDir, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_GCoordX, m_GCoordY + 80, m_Font, strGDisplayPressure, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_GCoordX, m_GCoordY + 100, m_Font, strGDisplayTwist, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_GCoordX, m_GCoordY + 120, m_Font, strGDisplayDigitalDir, g2dRGBColor(0xFF, 0x00, 0xFF));

	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick0CoordX, m_VJStick0CoordY, m_Font, strVJDisplaydX, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick0CoordX, m_VJStick0CoordY + 20, m_Font, strVJDisplaydY, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick0CoordX, m_VJStick0CoordY + 40, m_Font, strVJDisplaydZ, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick0CoordX, m_VJStick0CoordY + 60, m_Font, strVJDisplayAnalogDir, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick0CoordX, m_VJStick0CoordY + 80, m_Font, strVJDisplayPressure, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick0CoordX, m_VJStick0CoordY + 100, m_Font, strVJDisplayTwist, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick0CoordX, m_VJStick0CoordY + 120, m_Font, strVJDisplayDigitalDir, g2dRGBColor(0xFF, 0x00, 0xFF));

	m_VJ->GetAnalogDir(1, dX, dY, intdZ);
	sprintf(dummy, "%d", dX);
	strVJDisplaydX = itString(dummy);
	sprintf(dummy, "%d", dY);
	strVJDisplaydY = itString(dummy);
	sprintf(dummy, "%d", intdZ);
	strVJDisplaydZ = itString(dummy);
	AnalogDir = m_VJ->GetAnalogDir(1, Pressure, Twist);
	sprintf(dummy, "%f", AnalogDir);
	strVJDisplayAnalogDir = itString(dummy);
	sprintf(dummy, "%f", Pressure);
	strVJDisplayPressure = itString(dummy);
	sprintf(dummy, "%f", Twist);
	strVJDisplayTwist = itString(dummy);
	strVJDisplayDigitalDir = DigitalDirStrings[m_VJ->GetDigitalDir(1) + 1];

	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick1CoordX, m_VJStick1CoordY, m_Font, strVJDisplaydX, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick1CoordX, m_VJStick1CoordY + 20, m_Font, strVJDisplaydY, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick1CoordX, m_VJStick1CoordY + 40, m_Font, strVJDisplaydZ, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick1CoordX, m_VJStick1CoordY + 60, m_Font, strVJDisplayAnalogDir, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick1CoordX, m_VJStick1CoordY + 80, m_Font, strVJDisplayPressure, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick1CoordX, m_VJStick1CoordY + 100, m_Font, strVJDisplayTwist, g2dRGBColor(0xFF, 0x00, 0xFF));
	g2dScreenDrawUtil::DrawText( m_ValueOffset + m_VJStick1CoordX, m_VJStick1CoordY + 120, m_Font, strVJDisplayDigitalDir, g2dRGBColor(0xFF, 0x00, 0xFF));

	g2dScreen::EndScene();

	if (m_VJ->IsPressed(inKeys::e_ESC))
	{
		Exit();
	}
}

void MyApp::ReceiveStartEvent(appStartEvent& i_Event)
{
	DBG_LOG0("Start Event");

	inPackage::Init(1000, 0.1f, 0.9f);

	int i;

	g2dScreen::InitializeWindow(1024, 768, 50, 50);
//	g2dScreen::InitializeFullScreen(1024, 768, 16);

	m_VJ = inDeviceMgr::GetPCVJoy();
	m_K = inDeviceMgr::GetKeyboard();
	m_M = inDeviceMgr::GetMouse();
	if (inDeviceMgr::GetNumGamepads())
	{
		m_G = inDeviceMgr::GetGamepad();
	}

	for (i = 0; i < inKeys::e_NUMTERAWATTKEYS; ++i)
	{
		m_VJ->MapButtons(m_K, static_cast<inKeys::Keys>(i), static_cast<inKeys::Keys>(i));
	}

	std::vector<int> TerawattVector;
	std::vector<int> ActualVector;

	TerawattVector.push_back(inKeys::e_A);
	ActualVector.push_back(inKeys::e_B);
	TerawattVector.push_back(inKeys::e_B);
	ActualVector.push_back(inKeys::e_A);
	TerawattVector.push_back(inKeys::e_A);
	ActualVector.push_back(inKeys::e_C);
	TerawattVector.push_back(inKeys::e_Q);
	ActualVector.push_back(inKeys::e_ESC);

	m_VJ->MapButtons(m_K, TerawattVector, ActualVector);
	TerawattVector.clear();
	ActualVector.clear();

	TerawattVector.push_back(inKeys::e_B);
	ActualVector.push_back(0);
	TerawattVector.push_back(inKeys::e_N);
	ActualVector.push_back(1);
	TerawattVector.push_back(inKeys::e_T);
	ActualVector.push_back(2);

	m_VJ->MapButtons(m_M, TerawattVector, ActualVector);
	TerawattVector.clear();
	ActualVector.clear();

	m_VJ->MapDigitalDir(m_K, 0, inKeys::e_W, inKeys::e_D, inKeys::e_S, inKeys::e_A);
	m_VJ->MapDigitalDir(m_K, 1, inKeys::e_UP, inKeys::e_RIGHT, inKeys::e_DOWN, inKeys::e_LEFT);

	m_VJ->MapStick(m_M, e_Stick1,0);

	if (m_G)
	{
		for (i = 0; i < 128; ++i)
		{
			m_VJ->MapButtons(m_G, i, i);
		}

		m_VJ->MapButtons(m_G, inKeys::e_Q, 8);
		m_VJ->InvertXAxis(m_G, e_Stick0, false);
		m_VJ->InvertYAxis(m_G, e_Stick0, false);
		m_VJ->MapStick(m_G, e_Stick0,0);

		m_VJ->MapButtons(m_G, inKeys::e_Q, 8);
		m_VJ->InvertXAxis(m_G, e_Stick1, true);
		m_VJ->InvertYAxis(m_G, e_Stick1, true);
		m_VJ->MapStick(m_G, e_Stick1,1);
	}

	//m_VJ->InvertXAxis(m_K, 0, true);
	//m_VJ->InvertYAxis(m_K, 0, true);

	m_Font = g2dFontUtil::LoadFont(itString("Comic Sans MS"), 16);
	m_BigFont = g2dFontUtil::LoadFont(itString("Comic Sans MS"), 24);
}

void MyApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	DBG_LOG0("Stop Event");

	g2dFontUtil::ReleaseAllFonts();
	g2dScreen::DeInitialize();

	inPackage::CleanUp();
}

void MyApp::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
	DBG_LOG0("Suspend Event");
}

void MyApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
	DBG_LOG0("Resume Event");
}

//====================================================================
//====================================================================
int WINAPI 
WinMain(
		HINSTANCE hInstance,      // handle to current instance
		HINSTANCE hPrevInstance,  // handle to previous instance
		LPSTR lpCmdLine,          // command line
		int nCmdShow)             // show state
{
	appApplicationPAC::SetHINSTANCE(hInstance);

	envPackage::Init();
	dbgPackage::Init();
	itPackage::Init();
	appPackage::Init();
	g2dPackage::Init();

	MyApp app;

//	try
//	{
		app.Run();
//	}
	
	g2dPackage::CleanUp();
	appPackage::CleanUp();
	itPackage::CleanUp();
	dbgPackage::CleanUp();
	envPackage::CleanUp();
	return 0;
}