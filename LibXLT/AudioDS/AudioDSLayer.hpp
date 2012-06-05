/*****************************************************************************
**  AudioDSLayer.hpp
**
**      AudioDSLayer contains the initialization functions
**	for the all packages within the Audio Layer.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef AUDIO_DS_LAYER_HPP
#error AudioDSLayer.hpp multiply included
#endif
#define AUDIO_DS_LAYER_HPP

class AudioDSLayer
{
	public:

		//========================================================================
		//	Init 
		//========================================================================
		static void Init();

		//========================================================================
		//	CleanUp 
		//========================================================================
		static void CleanUp() throw();
};
