//****************************************************************************
//	ovrlyObjectColor
//
//	An overlay object for rotation
//
//	StudioGPU
//	Copyright(C) 2010 - All Rights Reserved
//****************************************************************************
#ifdef OVRLY_OBJECTColor_HPP
#error ovrlyObjectColor.hpp multiply included
#endif
#define OVRLY_OBJECTColor_HPP

#ifndef OVRLY_OBJECT_HPP
#include "FCSupport/ovrly/ovrlyObject.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
class ovrlyObjectColor : public ovrlyObject
{
public:
	///-----------------------------------------------------------------------
	/// constructors
	///-----------------------------------------------------------------------
	ovrlyObjectColor(itString& i_FileNormal,
					itString& i_FileHighlight,
					itString& i_FileDisable,
					maFloatRGBA& i_Color);

	///-----------------------------------------------------------------------
	/// destructors
	///-----------------------------------------------------------------------
	virtual ~ovrlyObjectColor();

	//--------------------------------------------------------------------
	///	This overlay object has been picked, do what it needs to do.
	//--------------------------------------------------------------------
	virtual void State_Picked();

	//--------------------------------------------------------------------
	///	Inputs are the mouse movement
	//--------------------------------------------------------------------
	virtual void DoMovement( int i_X, int i_Y, int i_Z );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	maFloatRGBA GetColor() const;

private:
	maFloatRGBA m_Color;
};

