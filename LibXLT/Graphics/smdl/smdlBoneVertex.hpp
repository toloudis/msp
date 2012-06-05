/*****************************************************************************
**	smdlBoneVertex.hpp
**
**		smdlBoneVertex.hpp defines the BoneVertex struct, which is used
**	by the object to do skinning computations,
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_BONEVERTEX_HPP
#error smdlBoneVertex.hpp multiply included
#endif
#define SMDL_BONEVERTEX_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef MA_POINT4D_HPP
#include "Core/ma/maPoint4d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
struct scBoneInfluence
{
	float m_fWeight;
	envType::UInt32 m_BoneIndex;
};


//============================================================================
//============================================================================
struct smdlBoneVertex
{
	//maPoint3d m_Position;
	//maVector3d m_Normal;
	std::vector<scBoneInfluence> m_Influences;
};
