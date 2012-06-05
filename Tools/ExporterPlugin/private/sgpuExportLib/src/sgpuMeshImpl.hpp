/****************************************************************************\
**  sgpuMeshImpl.hpp
**
**      sgpuMeshImpl.hpp defines private implementation for sgpuMesh.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_MESHIMPL_HPP
#define SGPU_MESHIMPL_HPP

#ifndef MDL_FRAGINFO_HPP
#include "Graphics/mdl/mdlFragInfo.hpp"
#endif 

#include <map>
#include <string>


//============================================================================
//============================================================================
struct sgpuMeshImpl
{

	shared_ptr<mdlFragInfo> m_Mesh;
	bool operator ==( const sgpuMeshImpl &other ) const
	{
		return m_Mesh == other.m_Mesh;
	}
};

#endif // #ifndef SGPU_MESHIMPL_HPP
