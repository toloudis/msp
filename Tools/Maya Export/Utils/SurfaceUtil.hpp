/*****************************************************************************
**  SurfaceUtil.hpp
**
**   Namespace for gathering Maya mesh into LibXLT surface info.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef SURFACEUTIL_HPP
#error SurfaceUtil.hpp multiply included
#endif
#define SURFACEUTIL_HPP

#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 

#include <vector>

class MFnMesh;
class maMatrix4x4;
class mdlNodeInfo;

namespace SurfaceUtil
{
	//========================================================================
	// Gather information about mesh into a node info structure.
	//========================================================================
	bool GatherMeshInfo(MFnMesh &mesh, 
						mdlNodeInfo& o_NodeForShape, 
						mdlMatInfoTable& io_MaterialTable,
						const std::vector<MString>& i_Joints,
						const maMatrix4x4& i_BindPose,
						bool i_bReversePolygonOrder,
						MString &o_Message);

	//========================================================================
	// Gather information about mesh into a node info structure.
	//	This variation will write the mesh into world space and will
	//	not look for skinning or blend shape deformers.
	//========================================================================
	bool GatherWorldSpaceMeshInfo(MFnMesh &mesh, 
						const std::string &i_SurfaceName,
						bool i_bPrepareForVertexAnim,
						bool i_bReversePolygonOrder,
						mdlNodeInfo& o_NodeForShape, 
						mdlMatInfoTable& io_MaterialTable,
						MString &o_Message);

}