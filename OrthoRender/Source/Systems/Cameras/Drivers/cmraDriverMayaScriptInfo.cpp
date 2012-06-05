/*****************************************************************************
**	cmraDriverMayaScriptInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverMayaScriptInfo.hpp"

#include "Graphics/g3d/g3dConstants.hpp"
#include "Tool/api3d/api3dScale.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
	const float c_InitialCutThreshold = 1.0f;
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
	m_CutThreshold(c_InitialCutThreshold) // better not to use the scale - it's not dependable
	//m_CutThreshold(api3dScale::Scale(c_InitialCutThreshold)) // default value based on global scale
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
