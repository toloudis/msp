/*****************************************************************************
**  VertexKeys.hpp
**
**   Namespace for baking vertex animations  
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef VERTEX_KEYS_HPP
#error VertexKeys.hpp multiply included
#endif
#define VERTEX_KEYS_HPP

#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSubd.h>

#ifndef VTX_STREAMCOMPRESSION_HPP
#include "Graphics/vtx/vtxStreamCompression.hpp"
#endif 


#include <list>
#include <map>

class chWriter;
class gfFileBin;

namespace VertexKeys
{
//============================================================================
// Supporting data structures
//============================================================================

	struct DelayedMeshAnim
	{
		//MFnMesh m_Mesh;
		MDagPath m_DagPath;
		bool m_bWriteNormals;
		std::map<float, int> m_SeekPos;
		int m_NumVerts;
	};
	//struct DelayedSubdAnim
	//{
	//	MFnSubd m_Subdiv;
	//	std::map<float, int> m_SeekPos;
	//};
	struct DelayedMeshStream
	{
		MDagPath m_DagPath;
		bool m_bWriteNormals;
		int m_NumVerts;
		shared_ptr<vtxStreamCompression> m_CompressStream;
	};

//============================================================================
// Vertex anim baking
//============================================================================

	//========================================================================
	// Gathers vertex animation seek position for this delayed mesh info
	//========================================================================
	void PrepareVertexAnimation(DelayedMeshAnim &io_DelayedMeshInfo,
							   const std::list<float> &i_Keys,
							   chWriter &o_Writer,
							   gfFileBin& io_File);

	//========================================================================
	// Gather vertex animation seek position for this mesh
	//========================================================================
	void PrepareVertexAnimation(MFnMesh &i_Mesh,
							   const std::string &i_MeshName,
							   MDagPath &i_DagPath,
							   std::list<DelayedMeshAnim> &o_DelayedMeshes,
							   const std::list<float> &i_Keys,
							   chWriter &o_Writer,
							   gfFileBin& io_File,
							   bool i_bWriteNormals);
	//void PrepareVertexAnimation(MFnSubd &i_Subdiv,
	//						   std::list<DelayedSubdAnim> &o_DelayedSubdivs,
	//						   const std::list<float> &i_Keys,
	//						   chWriter &o_Writer,
	//						   gfFileBin& io_File);

	//========================================================================
	//	BakeAnimation - write animation based on the given keys
	//		that were gathered earlier. Returns false on error.
	//========================================================================
	bool BakeAnimation(chWriter &o_Writer,
						gfFileBin& io_File,
						const std::list<DelayedMeshAnim> &i_DelayedMeshes, 
						float i_CurrentTime,
						float i_BeginFrame,
						bool i_bWorldSpace);
	//void BakeAnimation(chWriter &o_Writer,
	//					gfFileBin& io_File,
	//					const std::list<DelayedSubdAnim> &i_DelayedSubdivs,
	//					float i_CurrentTime,
	//					float i_BeginFrame);

//============================================================================
// Vertex anim baking through compression streams
//============================================================================

	//========================================================================
	// Create a compression stream for this mesh.
	// Negative tolerance will produce a lossless compression stream.
	//========================================================================
	void CreateAnimationStream(MFnMesh &i_Mesh,
							   const std::string &i_MeshName,
							   MDagPath &i_DagPath,
							   bool i_bWriteNormals,
							   vtxGeometryCacheWriter& io_GeometryCache,
							   std::list<DelayedMeshStream> &o_DelayedMeshes,
							   float i_Tolerance = -1.0f);

	//========================================================================
	//	BakeAnimationStreams - submit current frame of animation data for 
	// each surface to its stream. Returns false on error.
	//========================================================================
	bool BakeAnimationStreams(const std::list<DelayedMeshStream> &i_DelayedMeshes, 
								float i_CurrentTime,
								float i_BeginFrame,
								bool i_bWorldSpace);

	//========================================================================
	//	FinishStreams - write final animation data into the geometry cache.
	//========================================================================
	void FinishStreams(const std::list<DelayedMeshStream> &i_DelayedMeshes);

	//========================================================================
	//	WriteAnimationStreams - write animation frames that reference
	//	into the geometry cache written during earlier calls to
	//  BakeAnimationStreams().
	//========================================================================
	void WriteAnimationStreams(chWriter &o_Writer,
							   const std::list<DelayedMeshStream> &i_DelayedMeshes);
}
