/*****************************************************************************
**  inDevice.hpp
**
**      inDevice acts as a base class from which all other device classes are
**		to be derived.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_DEVICE_HPP
#error inDevice.hpp multiply included
#endif
#define IN_DEVICE_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

class inDevice
{
public:
	//========================================================================
	//	Constructor
	//========================================================================
	inDevice();

	//========================================================================
	//	Destructor
	//========================================================================
	virtual ~inDevice();

	//========================================================================
	//	Think gives the device a chance to update its state once per frame
	//========================================================================
	virtual void Think() = 0;

	//========================================================================
	//	Enable will enable the device
	//========================================================================
	void Enable();

	//========================================================================
	//	Disable will disable the device
	//========================================================================
	void Disable();

	//========================================================================
	//	IsEnabled returns true if the device is currently enabled
	//========================================================================
	bool IsEnabled() const;

	//========================================================================
	//	GetDeviceName returns the name of the device, for example, 
	//	"Microsoft SideWinder Pro"
	//========================================================================
	const itString& GetDeviceName() const;

	//========================================================================
	//	GetDeviceName returns the name of the device, for example, 
	//	"Microsoft SideWinder Pro"
	//========================================================================
	void SetDeviceName(itString& i_DeviceName);

private:
	bool m_Enabled;
	itString m_DeviceName;
};