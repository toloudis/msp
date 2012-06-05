/*****************************************************************************
**	ovrlyObjectRotate.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/ovrly/ovrlyObjectRotate.hpp"

#include "Core/it/itString.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"


///-----------------------------------------------------------------------
/// constructors
///-----------------------------------------------------------------------
ovrlyObjectRotate::ovrlyObjectRotate(itString& i_FileNormal,
									itString& i_FileHighlight,
									itString& i_FileDisable)
:	ovrlyObject(i_FileNormal, i_FileHighlight, i_FileDisable)
{
	m_bIsPickable = true;
}

///-----------------------------------------------------------------------
/// destructors
///-----------------------------------------------------------------------
ovrlyObjectRotate::~ovrlyObjectRotate()
{
}

//--------------------------------------------------------------------
///	This overlay object has been picked, do what it needs to do.
//--------------------------------------------------------------------
//virtual 
void ovrlyObjectRotate::State_Picked()
{
	ovrlyObject::State_Picked();

	//	Do something
}

//--------------------------------------------------------------------
///	Inputs are the mouse movement
//--------------------------------------------------------------------
//virtual 
void ovrlyObjectRotate::DoMovement( int i_X, int i_Y, int i_Z )
{
	//DBG_TRACE(" Mouse Move " << i_X << " " << i_Y << " " << i_Z);

	maPoint3d campos, camtar;
	campos = cam3dMgr::GetCamera().GetPosition();
	camtar = cam3dMgr::GetCamera().GetTarget();
	float yaw, pitch;
	maVector3d radius = camtar - campos;
	maVector3d camdir = cam3dMgr::GetCamera().GetDirection();
	maFunctions::GetYawPitch(camdir , yaw, pitch);

	// base modifiers on screen size
	float y_modifier = 0.010f;
	yaw = yaw + (-y_modifier * i_X);
	//cam3dMgr::SetYaw

	float x = float(sin(yaw) * cos(pitch));
	float y = float(sin(pitch));
	float z = float(cos(yaw) * cos(pitch));

	maVector3d dir(-x, -y, -z);
	maPoint3d pos = camtar - dir * radius.Length();
//	maAxisBox box = cam3dMgr::GetCamera().GetBox();

//	cam3dMgr::GetCamera().LookAt(pos, camtar, maVector3d(0,1,0));

	cam3dMgr::SetManipDepth( (pos - camtar).Length() );
//	cam3dMgr::SetManipBox( box );
	//if(dZ != 0)
	//{
	//	o_CamManip.m_bScrollWheel = true;
	//	o_CamManip.m_CamManipMode = e_ZOOM;
	//}
}

