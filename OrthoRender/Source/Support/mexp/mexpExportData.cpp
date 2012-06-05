/****************************************************************************\
**  mexpExportData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Support/mexp/mexpExportData.hpp"

#include "Graphics/g3d/g3dConstants.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mexpExportData::mexpExportData()
:	m_bExportAnimation(true), 
	m_SimulationFrameRate(g3dConstants::c_fDefaultFrameRate), 
	m_bExportJoints(false)
{

}