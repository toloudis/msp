/*****************************************************************************
**	ovrlyObjectColor.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/ovrly/ovrlyObjectColor.hpp"

#include "Core/it/itString.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"


///-----------------------------------------------------------------------
/// constructors
///-----------------------------------------------------------------------
ovrlyObjectColor::ovrlyObjectColor(itString& i_FileNormal,
									itString& i_FileHighlight,
									itString& i_FileDisable,
									maFloatRGBA& i_Color)
:	ovrlyObject(i_FileNormal, i_FileHighlight, i_FileDisable)
{
	m_bIsPickable = true;
	m_Color = i_Color;
}

///-----------------------------------------------------------------------
/// destructors
///-----------------------------------------------------------------------
ovrlyObjectColor::~ovrlyObjectColor()
{
}

//--------------------------------------------------------------------
///	This overlay object has been picked, do what it needs to do.
//--------------------------------------------------------------------
//virtual 
void ovrlyObjectColor::State_Picked()
{
	ovrlyObject::State_Picked();

	//	Do something
}

//--------------------------------------------------------------------
///	Inputs are the mouse movement
//--------------------------------------------------------------------
//virtual 
void ovrlyObjectColor::DoMovement( int i_X, int i_Y, int i_Z )
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maFloatRGBA ovrlyObjectColor::GetColor() const
{
	return m_Color;
}

