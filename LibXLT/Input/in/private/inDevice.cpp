/*****************************************************************************
**  inDevice.cpp
**
**      inDevice acts as a base class from which all other device classes are
**		to be derived.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Input/in/inDevice.hpp"

//========================================================================
//	Constructor
//========================================================================
inDevice::inDevice()
{
	m_Enabled = true;
}

//========================================================================
//	Destructor
//========================================================================
inDevice::~inDevice()
{
	
}

//========================================================================
//	Enable will enable the device
//========================================================================
void inDevice::Enable()
{
	m_Enabled = true;
}

//========================================================================
//	Disable will disable the device
//========================================================================
void inDevice::Disable()
{
	m_Enabled = false;
}

//========================================================================
//	IsEnabled returns true if the device is currently enabled
//========================================================================
bool inDevice::IsEnabled() const
{
	return m_Enabled;
}

//========================================================================
//	GetDeviceName returns the name of the device, for example, 
//	"Microsoft SideWinder Pro"
//========================================================================
const itString& inDevice::GetDeviceName() const
{
	return m_DeviceName;
}

//========================================================================
//	GetDeviceName returns the name of the device, for example, 
//	"Microsoft SideWinder Pro"
//========================================================================
void inDevice::SetDeviceName(itString& i_DeviceName)
{
	m_DeviceName = i_DeviceName;
}