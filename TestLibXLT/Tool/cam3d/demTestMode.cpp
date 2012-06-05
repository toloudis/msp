/*****************************************************************************
**  demTestMode.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demTestMode.hpp"

#include "Core/app/appCharEvent.hpp"
#include "Core/app/appSimTime.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Core/ma/maConstants.hpp"

//====================================================================
//====================================================================
demTestMode::demTestMode()
:	m_QuitSignaled(false)
{
	this->SetEnableCharEvents(false);
}

//====================================================================
//====================================================================
demTestMode::~demTestMode()
{
}

//====================================================================
//====================================================================
void demTestMode::Initialize()
{
	this->SetEnableCharEvents(true);

	m_QuitSignaled = false;
}

//====================================================================
//	Think
//====================================================================
void demTestMode::Think()
{
	appSimTime::IncrementTime();
}

//====================================================================
//====================================================================
void demTestMode::DeInitialize()
{

	this->SetEnableCharEvents(false);
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demTestMode::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType('q'):
		case itString::CharType(' '):
			m_QuitSignaled = true;
		break;

	}
}

//====================================================================
//	GetQuitSignaled returns true if the user is done with this mode 
//	(pressed the space bar).
//====================================================================
bool demTestMode::GetQuitSignaled() const
{
	return m_QuitSignaled;
}
