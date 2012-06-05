/*****************************************************************************
**  cptrMotionSampler.cpp
**
**      see .h
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrMotionSampler.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrMotionSampler::cptrMotionSampler(int nSamplesPerFrame)
:	m_nSamplesPerFrame(nSamplesPerFrame),
	m_nSamplesCollected(0)
{
}


