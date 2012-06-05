/*****************************************************************************
**	cmraDriverFramedArcInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverFramedArcInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFramedArcInfo::cmraDriverFramedArcInfo(chDefs::Name i_ChunkName)
:	cmraDriverFramedBaseInfo( i_ChunkName ),
	m_fRevolutions( 1.0f ),
	m_bClockwise( true )
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFramedArcInfo::~cmraDriverFramedArcInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverFramedArcInfo::Clone()
{
	return new cmraDriverFramedArcInfo(*this);
}
