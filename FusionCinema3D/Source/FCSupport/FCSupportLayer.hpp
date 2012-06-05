/*****************************************************************************
**  FCSupportLayer.hpp
**
**      FCSupportLayer contains the initialization functions
**	for the all packages within the Fusion Cinema Support Layer.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef FCSUPPORT_LAYER_HPP
#error FCSupportLayer.hpp multiply included
#endif
#define FCSUPPORT_LAYER_HPP

//============================================================================
//============================================================================
class FCSupportLayer
{
	public:

		//--------------------------------------------------------------------
		//	Init
		//--------------------------------------------------------------------
		static void Init();

		//--------------------------------------------------------------------
		//	CleanUp
		//--------------------------------------------------------------------
		static void CleanUp() throw();
};
