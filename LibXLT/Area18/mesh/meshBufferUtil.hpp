/****************************************************************************\
**	meshBufferUtil.hpp
**
**	Utilities for constructing vertex and index buffer for use
**	with a meshTriMeshBumpFrag.
**
** Area17
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MESH_BUFFERUTIL_HPP
#error meshBufferUtil.hpp multiply included
#endif
#define MESH_BUFFERUTIL_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif 
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <vector>

//------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------
class oglDevice;
class meshVertexBuffer;
class meshIndexBuffer;

//------------------------------------------------------------------------
//------------------------------------------------------------------------
namespace meshBufferUtil
{
	//--------------------------------------------------------------------
	//	CreateVertexBuffer - create vertex buffer from 
	//		arrays of vertex data.
	//	Indices are needed for generating basis vectors when not
	//	included in i_Ss and i_Ts.
	//--------------------------------------------------------------------
	shared_ptr<meshVertexBuffer> CreateVertexBuffer(oglDevice* i_pDevice,
		const maPoint3d* i_Vertices,
		const maPoint3d* i_Normals,
		const maPoint2d* i_UVs,
		const maPoint3d* i_Ss,
		const maPoint3d* i_Ts,
		int	i_nVertices,
		const g3dIndexPtr i_Indices,
		int i_nNonShadowIndices,
		bool i_bMorphable = false,
		bool i_bComponentSort = false,
		bool i_bComputeBasisVectors = true);

	//--------------------------------------------------------------------
	//	CreateIndexBuffer - create index buffer from 
	//		array of index data.
	//	i_NumVertices is needed in order to know if 16-bit 
	//		or 32-bit indices are needed.
	//--------------------------------------------------------------------
	shared_ptr<meshIndexBuffer> CreateIndexBuffer(oglDevice* i_pDevice,
		const g3dIndexPtr i_Indices,
		int	i_nIndices,
		int i_nNonShadowIndices,
		int i_NumVertices,
		bool i_bComponentSort = false );

	//--------------------------------------------------------------------
	//	CreateVelocityBuffer - create velocity buffer from 
	//		vertex positions in i_pVertexBuffer
	//--------------------------------------------------------------------
	void CreateVelocityBuffer(oglDevice* i_pDevice,
		const shared_ptr<meshVertexBuffer> &i_pVertexBuffer,
		shared_ptr<meshVertexBuffer> &o_pVertexBuffer_Old,
		bool i_bMorphable);

	//--------------------------------------------------------------------
	//  BackupVertexBuffer - make copy of positions from i_pCurBuffer
	//		into i_pOldBuffer.
	//--------------------------------------------------------------------
	void BackupVertexBuffer( meshVertexBuffer * i_pOldBuffer, 
							 meshVertexBuffer * i_pCurBuffer );

};
