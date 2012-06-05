/****************************************************************************\
**	mdlNodeInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlNodeInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mdlNodeInfo::mdlNodeInfo()
:	m_bIsJoint(false),
	m_JointOrientation(0,0,0),
	m_JointScaleOrientation(0,0,0)
{
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mdlNodeInfo::mdlNodeInfo(bool i_bIsJoint)
:	m_bIsJoint(i_bIsJoint),
	m_JointOrientation(0,0,0),
	m_JointScaleOrientation(0,0,0)
{
}
