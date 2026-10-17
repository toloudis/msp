/****************************************************************************\
**	g3dMeshInfo.hpp
**
**		g3dType.hpp defines public geometry data types. They should be platform
**	independent.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_MESHINFO_HPP
#error g3dType.hpp multiply included
#endif
#define G3D_MESHINFO_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class matMaterial;


//============================================================================
//============================================================================
struct g3dMeshInfo
{
	std::vector<uint32_t> m_Indices;
	matMaterial* m_pMaterial;
};


