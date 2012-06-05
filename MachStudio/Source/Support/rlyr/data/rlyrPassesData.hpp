/********************************************************************************************\
** rlyrPassesData.hpp
**
**		Data relating to the render passes of each individual render layer
**
**  studio|gpu
\********************************************************************************************/
#pragma once

#ifdef RLYR_PASSESDATA_HPP
#error rlyrPassesData.hpp multiply included
#endif
#define RLYR_PASSESDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif

class rlyrPassesData
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	rlyrPassesData();

	prtyBoolean m_Beauty;
	prtyBoolean m_AOOnly;
	prtyBoolean m_Depth;
	prtyBoolean m_ShadowMask;
	prtyBoolean m_IlluminationOnly;
	prtyBoolean m_Normals;
	prtyBoolean m_DirtyMatte;
	prtyBoolean m_Wireframe;
	prtyBoolean m_Materials;
	prtyBoolean m_ReflectionsOnly;
	prtyBoolean m_Velocity;
	prtyBoolean m_Diffuse;
	prtyBoolean m_Specular;
	prtyBoolean m_Bloom;
	prtyBoolean m_Star;
	prtyBoolean m_CameraDOF;
	prtyBoolean m_Preview;
	prtyBoolean m_Emissive;
	prtyBoolean m_SpecEnv;
	prtyBoolean m_SpecLit;
	prtyBoolean m_DiffEnv;
	prtyBoolean m_DiffLit;
	prtyBoolean m_GI;
	prtyBoolean m_Glow;
	prtyBoolean m_RmanColorBleed;
	prtyBoolean m_MrayFinalGather;

};
