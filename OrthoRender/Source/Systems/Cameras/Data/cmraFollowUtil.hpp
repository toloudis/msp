/*****************************************************************************
**	cmraFollowUtil.hpp
**
**	Namespace with routines for matching editor and scripted cameras
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_FOLLOWUTIL_HPP
#error cmraFollowUtil.hpp multiply included
#endif
#define CMRA_FOLLOWUTIL_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

//============================================================================
//============================================================================
class cmraCameraData;
class camCamera;


//============================================================================
//============================================================================
namespace cmraFollowUtil
{
	//--------------------------------------------------------------------
	// ResetManipCamera() - reset the camera if it is controlled by a
	//	manip
	//--------------------------------------------------------------------
	void ResetManipCamera();

	//--------------------------------------------------------------------
	// Get data from the current camera view
	//--------------------------------------------------------------------
	void GetCurrentCamera(cmraCameraData& o_CamData);

	//--------------------------------------------------------------------
	// Get position and target of current camera view
	//--------------------------------------------------------------------
	void GetCurrentCamera(maPoint3d &o_Position, maPoint3d &o_Target);

}	// end of namespace
