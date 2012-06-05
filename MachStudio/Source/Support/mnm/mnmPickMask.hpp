/*****************************************************************************
**  mnmPickMask.hpp
**
**      Defines categories of objects that can be used in 
**	filtering during GPU picking.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef MNM_PICKMASK_HPP
#error mnmPickMask.hpp multiply included
#endif
#define MNM_PICKMASK_HPP

#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
#endif 

//============================================================================
// Error codes returned as return value of program itself.

//============================================================================
namespace mnmPickMask
{
	static const envType::UInt32 c_NoFilter	= 0x00;	// 0 means do not filter

	static const envType::UInt32 c_Geometry	= 0x01;	// Pick geometry
	static const envType::UInt32 c_Light	= 0x02;	// Pick lights
	static const envType::UInt32 c_Camera	= 0x04;	// Pick cameras

	static const envType::UInt32 c_Material	= 0x10;	// Pick surfaces with selectable material
	static const envType::UInt32 c_Surface	= 0x20;	// Pick selectable surfaces

}
