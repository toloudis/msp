/*****************************************************************************
**	cmraDriverMayaScriptInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverMayaScriptInfo.hpp"

#include "Graphics/g3d/g3dConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
	const float c_InitialCutThreshold = 100.0f;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverMayaScriptInfo::cmraDriverMayaScriptInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName),
	m_FrameRate(g3dConstants::c_fDefaultFrameRate),
	m_bDriverToAnimLength(true),
	m_bUseAnimStart(false),
	m_StartFrame(-1.0f),
	m_EndFrame(-1.0f),
	m_bLooping(false),
	m_Offset(0,0,0),
	m_CutThreshold(c_InitialCutThreshold),
	m_bHasStereoChannels(false),
	m_bAlterPosition(true),
	m_bAlterTarget(true),
	m_bAlterFov(true),
	m_bAlterTilt(true),
	m_bAlterFocalLength(true),
	m_bAlterHorizontalAperture(true),
	m_bAlterStereoFD(true),
	m_bAlterStereoIOD(true)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverMayaScriptInfo::~cmraDriverMayaScriptInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverMayaScriptInfo::Clone()
{
	return new cmraDriverMayaScriptInfo(*this);
}
