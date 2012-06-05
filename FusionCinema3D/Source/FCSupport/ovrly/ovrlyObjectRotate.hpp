//****************************************************************************
//	ovrlyObjectRotate
//
//	An overlay object for rotation
//
//	StudioGPU
//	Copyright(C) 2010 - All Rights Reserved
//****************************************************************************
#ifdef OVRLY_OBJECTROTATE_HPP
#error ovrlyObjectRotate.hpp multiply included
#endif
#define OVRLY_OBJECTROTATE_HPP

#ifndef OVRLY_OBJECT_HPP
#include "FCSupport/ovrly/ovrlyObject.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
class ovrlyObjectRotate : public ovrlyObject
{
public:
	///-----------------------------------------------------------------------
	/// constructors
	///-----------------------------------------------------------------------
	ovrlyObjectRotate(itString& i_FileNormal,
					itString& i_FileHighlight,
					itString& i_FileDisable);

	///-----------------------------------------------------------------------
	/// destructors
	///-----------------------------------------------------------------------
	virtual ~ovrlyObjectRotate();

	//--------------------------------------------------------------------
	///	This overlay object has been picked, do what it needs to do.
	//--------------------------------------------------------------------
	virtual void State_Picked();

	//--------------------------------------------------------------------
	///	Inputs are the mouse movement
	//--------------------------------------------------------------------
	virtual void DoMovement( int i_X, int i_Y, int i_Z );
};

