/*****************************************************************************
**	vtxVertexFrame.hpp
**
**		vtxVertexFrame - structure for a single frame of animation data
**	on a single surface.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_VERTEXFRAME_HPP
#error vtxVertexFrame.hpp multiply included
#endif
#define VTX_VERTEXFRAME_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
struct vtxVertexFrame
{
	typedef std::vector<maPoint3d> VertContainerT;
	typedef std::vector<maVector3d> NormContainerT;
	//positions of all original vertices
	//in the mesh, (ie not the unique vertices)
	VertContainerT m_Positions;
	//normals of the mesh 
	//(not the normals in the unique vertices)
	NormContainerT m_Normals;
};

