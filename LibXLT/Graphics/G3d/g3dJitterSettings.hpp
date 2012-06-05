/****************************************************************************\
**	g3dJitterSettings.hpp
**
**		The g3dJitterSettings configures the use of multiple passes
**	per light to make softer lighting effects and shadows.
**	These passes are combined using an additive blend with reduced
**	intensity lights.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_JITTERSETTINGS_HPP
#error g3dJitterSettings.hpp multiply included
#endif
#define G3D_JITTERSETTINGS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
namespace g3dJitterSettings
{
	//------------------------------------------------------------------------
	// If true, activates jittering for shadow lights.
	// This is false by default.
	//------------------------------------------------------------------------
	bool GetDoJitter();
	void SetDoJitter(bool i_Val);

	//------------------------------------------------------------------------
	// Number of passes per light
	//------------------------------------------------------------------------
	int GetNumJitterPasses();
	void SetNumJitterPasses(int i_Val);

	//------------------------------------------------------------------------
	// JitterScale, set separately for point and directional lights
	//------------------------------------------------------------------------
	float GetJitterPointScale();
	void SetJitterPointScale(float i_Val);
	float GetJitterDirScale();
	void SetJitterDirScale(float i_Val);

	//------------------------------------------------------------------------
	// GetJitterNoise - returns vector of noise with each component
	//	ranging from -1 to 1.  Each call with the same index value will
	//  return the same jitter vector.  This vector is not scaled, the
	//	caller should do the jitter scaling.
	//------------------------------------------------------------------------
	maVector3d GetJitterNoise(int i_Index);
}


