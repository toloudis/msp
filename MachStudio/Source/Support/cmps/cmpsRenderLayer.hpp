/*****************************************************************************
**  cmpsRenderLayer.hpp
**
**      Defines render layers for compass placement.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_RENDERLAYER_HPP
#error cmpsRenderLayer.hpp multiply included
#endif
#define CMPS_RENDERLAYER_HPP



//----------------------------------------------------------------------------
// enumeration for possible render layer placements,
// actually moved to icon layers within viewer now.
//----------------------------------------------------------------------------
namespace cmpsRenderLayer
{
	enum RenderLayer
	{
		e_World = 0,
		//e_Camera,
		e_Manipulators
	};

}


