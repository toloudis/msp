/****************************************************************************\
**	g3dJitterSettings.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dJitterSettings.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace g3dJitterSettings
{
namespace
{
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool l_bDoJitter = false;
int l_NumJitterPasses = 10;
float l_JitterPointScale = 1.0f;
float l_JitterDirScale = 1.0f;

std::vector<maVector3d> l_Jitters;

//----------------------------------------------------------------------------
// Fill jitter array
//----------------------------------------------------------------------------
void init_jitter_array(int i_Num)
{
	l_Jitters.resize(i_Num);
	l_Jitters[0].Set(0.0f, 0.0f, 0.0f);

	//double angle_inc = (maConstants::c_dPI * 5.37) / double(i_Num);
	double angle_inc = (maConstants::c_dPI * 2) / double(i_Num);
	double angle = 0.0;
	double ht_inc = 2.0 / double(i_Num);
	double ht = -1.0f;
	for (int i=1; i<i_Num; i++, angle+=angle_inc, ht+=ht_inc)
	{
		l_Jitters[i].Set(cos(angle), ht, sin(angle));
		//DBG_LOG4("Jitter(%d): %f, %f, %f", i, l_Jitters[i].m_X, l_Jitters[i].m_Y, l_Jitters[i].m_Z);
	}
}

}

//------------------------------------------------------------------------
// If true, activates jittering for shadow lights.
// This is false by default.
//------------------------------------------------------------------------
bool GetDoJitter()
{
	return l_bDoJitter;
}
void SetDoJitter(bool i_Val)
{
	l_bDoJitter = i_Val;

	// Fill jitter array
	init_jitter_array(l_NumJitterPasses);
}

//------------------------------------------------------------------------
// Number of passes per light
//------------------------------------------------------------------------
int GetNumJitterPasses()
{
	return l_NumJitterPasses;
}
void SetNumJitterPasses(int i_Val)
{
	l_NumJitterPasses = i_Val;

	// Fill jitter array
	init_jitter_array(l_NumJitterPasses);
}

//------------------------------------------------------------------------
// JitterScale, set separately for point and directional lights
//------------------------------------------------------------------------
float GetJitterPointScale()
{
	return l_JitterPointScale;
}
void SetJitterPointScale(float i_Val)
{
	l_JitterPointScale = i_Val;
}
float GetJitterDirScale()
{
	return l_JitterDirScale;
}
void SetJitterDirScale(float i_Val)
{
	l_JitterDirScale = i_Val;
}

//------------------------------------------------------------------------
// GetJitterNoise - returns vector of noise with each component
//	ranging from -1 to 1.  Each call with the same index value will
//  return the same jitter vector.  This vector is not scaled, the
//	caller should do the jitter scaling.
//------------------------------------------------------------------------
maVector3d GetJitterNoise(int i_Index)
{
	return l_Jitters[i_Index % l_Jitters.size()];
}

}
