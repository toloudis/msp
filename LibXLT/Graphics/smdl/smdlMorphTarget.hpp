/*****************************************************************************
**	smdlMorphTarget.hpp
**
**		smdlMorphTarget.hpp defines the structures used in morphing
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_MORPHTARGET_HPP
#error smdlMorphTarget.hpp multiply included
#endif
#define SMDL_MORPHTARGET_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//============================================================================
//struct smdlMorphVertex
//{
//	maPoint3d m_Position;
//	maVector3d m_Normal;
//};


//============================================================================
//============================================================================
struct smdlMorphTarget
{
	std::string m_Name;
	std::string m_Alias;	// Name in the Maya UI for this blend shape
	//std::vector<smdlMorphVertex> m_Offsets;
	std::vector<maVector3d> m_Offsets;
	bool m_bOffsetsAreDeltas;  // values in m_Offsets could be deltas or absolute positions
};
