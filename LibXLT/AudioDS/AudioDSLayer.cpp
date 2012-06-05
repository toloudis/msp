/*****************************************************************************
**  AudioDSLayer.cpp
**
**      AudioDSLayer contains the initialization functions
**	for the all packages within the Audio Layer.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "AudioDS/AudioDSLayer.hpp"

//#include "AudioDS/mu/muPackage.hpp"
#include "AudioDS/sn/snPackage.hpp"

namespace
{

int l_RefCount = 0;

}

//============================================================================
//	Init 
//============================================================================
void AudioDSLayer::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages in the layer
		snPackage::Init();
		//muPackage::Init();
	}

	//	increment the ref count
	l_RefCount++;
}

//============================================================================
//	CleanUp 
//============================================================================
void AudioDSLayer::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		//muPackage::CleanUp();
		snPackage::CleanUp();

	}
}
