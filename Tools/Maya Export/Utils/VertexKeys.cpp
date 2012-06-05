/*****************************************************************************
**  VertexKeys.cpp
**
**   Namespace for baking vertex animations  
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <VertexKeys.hpp>
#include <VertexFuncs.hpp>

#include <maya/MFloatVectorArray.h>

#include "Graphics/vtx/vtxCompressionUtil.hpp"


namespace VertexKeys
{
	//========================================================================
	// Fills in the DelayedMeshAnim structure for later preparation
	//========================================================================
	//void GetDelayedMeshInfo(MFnMesh &i_Mesh,
	//						   MDagPath &i_DagPath,
	//						   bool i_bWriteNormals,
	//						   DelayedMeshAnim &o_DelayedMeshInfo)
	//{
	//	o_DelayedMeshInfo.m_DagPath = i_DagPath;
	//	o_DelayedMeshInfo.m_bWriteNormals = i_bWriteNormals;
	//	o_DelayedMeshInfo.m_NumVerts = i_Mesh.numVertices();
	//}

	//========================================================================
	// Gathers vertex animation seek position for this delayed mesh info
	//========================================================================
	void PrepareVertexAnimation(DelayedMeshAnim &io_DelayedMeshInfo,
							   const std::list<float> &i_Keys,
							   chWriter &o_Writer,
							   gfFileBin& io_File)
	{
		MFnMesh mesh(io_DelayedMeshInfo.m_DagPath);
		MString name = MayaUtil::PrepareName(mesh.name());

		// This function will allocate space in the file for the animation data
		// to go later
		VertexFuncs::PrepareVertexAnimation(mesh, name.asUTF8(), o_Writer, io_File, i_Keys, 
			io_DelayedMeshInfo.m_SeekPos, io_DelayedMeshInfo.m_bWriteNormals);
	}
								

	//========================================================================
	// Gathers vertex animation seek position for this mesh
	//========================================================================
	void PrepareVertexAnimation(MFnMesh &i_Mesh,
							   const std::string &i_MeshName,
							   MDagPath &i_DagPath,
							   std::list<DelayedMeshAnim> &o_DelayedMeshes,
							   const std::list<float> &i_Keys,
							   chWriter &o_Writer,
							   gfFileBin& io_File,
							   bool i_bWriteNormals)
	{
		DelayedMeshAnim delay_info;
		//delay_info.m_Mesh.setObject(i_Mesh.object());
		delay_info.m_DagPath = i_DagPath;
		delay_info.m_bWriteNormals = i_bWriteNormals;
		delay_info.m_NumVerts = i_Mesh.numVertices();
		
		// This function will allocate space in the file for the animation data
		// to go later
		VertexFuncs::PrepareVertexAnimation(i_Mesh, i_MeshName, o_Writer, io_File, i_Keys, 
			delay_info.m_SeekPos, i_bWriteNormals);

		o_DelayedMeshes.push_back( delay_info );
	}
	//void PrepareVertexAnimation(MFnSubd &i_Subdiv,
	//						   std::list<DelayedSubdAnim> &o_DelayedSubdivs,
	//						   const std::list<float> &i_Keys,
	//						   chWriter &o_Writer,
	//						   gfFileBin& io_File)
	//{
	//	DelayedSubdAnim delay_info;
	//	delay_info.m_Subdiv.setObject(i_Subdiv.object());
	//	
	//	// This function will allocate space in the file for the animation data
	//	// to go later
	//	VertexFuncs::PrepareVertexAnimation(i_Subdiv, o_Writer, io_File, i_Keys, 
	//		delay_info.m_SeekPos);

	//	o_DelayedSubdivs.push_back( delay_info );

	//}

	//========================================================================
	//	BakeAnimation - write animation based on the given keys
	//		that were gathered earlier. Returns false on error.
	//========================================================================
	bool BakeAnimation(chWriter &o_Writer,
						gfFileBin& io_File,
						const std::list<DelayedMeshAnim> &i_DelayedMeshes, 
						float i_CurrentTime,
						float i_BeginFrame,
						bool i_bWorldSpace)
	{

		// Bake current position of vertex animation
		std::list<DelayedMeshAnim>::const_iterator it;
		for (it = i_DelayedMeshes.begin(); it != i_DelayedMeshes.end(); ++it)
		{
			// Write animation data back into correct position in file.
			MFnMesh mesh(it->m_DagPath);
			if (mesh.numVertices() != it->m_NumVerts)
				return false;

			VertexFuncs::WriteVertexAnimationFrame(mesh, o_Writer, io_File, 
				i_CurrentTime, i_BeginFrame, it->m_SeekPos, it->m_bWriteNormals, i_bWorldSpace);
		}
		return true;
	}
	//void BakeAnimation(chWriter &o_Writer,
	//					gfFileBin& io_File,
	//					const std::list<DelayedSubdAnim> &i_DelayedSubdivs,
	//					float i_CurrentTime,
	//					float i_BeginFrame)
	//{

	//	// Bake current position of vertex animation
	//	std::list<DelayedSubdAnim>::const_iterator it;
	//	for (it = i_DelayedSubdivs.begin(); it != i_DelayedSubdivs.end(); ++it)
	//	{
	//		// Write animation data back into correct position in file.
	//		VertexFuncs::WriteVertexAnimationFrame(it->m_Subdiv, o_Writer, io_File, 
	//			i_CurrentTime, i_BeginFrame, it->m_SeekPos);
	//	}
	//}

	
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
							   float i_Tolerance)
	{
		DelayedMeshStream delay_info;
		delay_info.m_DagPath = i_DagPath;
		delay_info.m_bWriteNormals = i_bWriteNormals;
		delay_info.m_NumVerts = i_Mesh.numVertices();

		// Negative tolerance will produce a lossless compression stream
		if (i_Tolerance < 0)
			delay_info.m_CompressStream = 
				vtxCompressionUtil::CreateLosslessCompressStream(i_MeshName, io_GeometryCache);
		else
			delay_info.m_CompressStream = 
			vtxCompressionUtil::CreateToleranceCompressStream(i_Tolerance, i_MeshName, io_GeometryCache);

		o_DelayedMeshes.push_back( delay_info );
	}

	//========================================================================
	//	BakeAnimationStreams - submit current frame of animation data for 
	// each surface to its stream. Returns false on error.
	//========================================================================
	bool BakeAnimationStreams(const std::list<DelayedMeshStream> &i_DelayedMeshes, 
								float i_CurrentTime,
								float i_BeginFrame,
								bool i_bWorldSpace)
	{
		MStatus status;
		// Bake current position of vertex animation
		std::list<DelayedMeshStream>::const_iterator it;
		for (it = i_DelayedMeshes.begin(); it != i_DelayedMeshes.end(); ++it)
		{
			// Write animation data back into correct position in file.
			MFnMesh mesh(it->m_DagPath);
			int nVerts = mesh.numVertices(&status);
			if (mesh.numVertices() != it->m_NumVerts)
				return false;

			vtxVertexFrame vertex_frame;

			// Positions
			vertex_frame.m_Positions.resize(nVerts);
			std::vector<maPoint3d>::iterator vit = vertex_frame.m_Positions.begin();
			MPoint point;
			MSpace::Space export_space = (i_bWorldSpace) ? MSpace::kWorld : MSpace::kObject;
			for(int i = 0; i < nVerts; ++i, ++vit) 
			{
				mesh.getPoint(i, point, export_space);
				vit->Set(float(point[0]),float(point[1]),float(point[2]));
			}

			if (it->m_bWriteNormals)
			{
				// Normals
				int nNormals = mesh.numNormals(&status);

				vertex_frame.m_Normals.resize(nNormals);

				MFloatVectorArray normalArray;
				MFloatVector normal;
				mesh.getNormals(normalArray, export_space);
				std::vector<maPoint3d>::iterator nit = vertex_frame.m_Normals.begin();
				for (int i = 0; i < nNormals; ++i, ++nit) 
				{
					normal = normalArray[i];
					nit->Set(float(normal[0]),float(normal[1]),float(normal[2]));
				}
			}

			it->m_CompressStream->SubmitFrame(i_CurrentTime-i_BeginFrame, vertex_frame);
		}
		return true;
	}

	//========================================================================
	//	FinishStreams - write final animation data into the geometry cache.
	//========================================================================
	void FinishStreams(const std::list<DelayedMeshStream> &i_DelayedMeshes)
	{
		std::list<DelayedMeshStream>::const_iterator it;
		for (it = i_DelayedMeshes.begin(); it != i_DelayedMeshes.end(); ++it)
		{
			it->m_CompressStream->Finish();
		}
	}
	
	//========================================================================
	//	WriteAnimationStreams - write animation frames that reference
	//	into the geometry cache written during earlier calls to
	//  BakeAnimationStreams().
	//========================================================================
	void WriteAnimationStreams(chWriter &o_Writer,
							   const std::list<DelayedMeshStream> &i_DelayedMeshes)
	{
		std::list<DelayedMeshStream>::const_iterator it;
		for (it = i_DelayedMeshes.begin(); it != i_DelayedMeshes.end(); ++it)
		{
			it->m_CompressStream->WriteCompressedAnimationData(o_Writer);
		}
	}
}