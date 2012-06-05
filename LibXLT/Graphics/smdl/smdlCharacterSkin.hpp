/*****************************************************************************
**  smdlCharacterSkin.hpp
**
**      smdlCharacterSkin.hpp defines a grouping of structures that define
**	one mesh that makes up part of a character object.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef SMDL_CHARACTERSKIN_HPP
#error smdlCharacterSkin.hpp multiply included
#endif
#define SMDL_CHARACTERSKIN_HPP

#ifndef SMDL_BONEVERTEX_HPP
#include "Graphics/smdl/smdlBoneVertex.hpp"
#endif
#ifndef SMDL_MORPHTARGET_HPP
#include "Graphics/smdl/smdlMorphTarget.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/Ma/maMatrix4x4.hpp"
#endif 


//============================================================================
//============================================================================
struct smdlCharacterSkin
{
	std::vector<smdlBoneVertex> m_BoneVertices;
	std::vector< shared_ptr<smdlMorphTarget> > m_MorphTargets;
	maMatrix4x4 m_BindPose; 
};
