/*****************************************************************************
**  VertexFuncs.hpp
**
**   Namespace for vertex animation related functions.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef VERTEXFUNCS_HPP
#error VertexFuncs.hpp multiply included
#endif
#define VERTEXFUNCS_HPP

class MDagPath;
class MFnMesh;
class MFnParticleSystem;
class MFnSubd;
class MMatrix;
class chWriter;
class gfFileBin;

#include <list>
#include <map>

namespace VertexFuncs
{
	//========================================================================
	//	PrepareVertexAnimation - allocate enough space in file in order
	//		to later write frame data into it. Create a management
	//		data structure that tracks the location of each frame
	//		within the file so that we can seek to those points later.
	//========================================================================
	void PrepareVertexAnimation(MFnMesh &mesh, 
							  const std::string &i_MeshName,
							  chWriter &o_Writer,
							  const gfFileBin& i_File,
							  const std::list<float> &i_Keys,
							  std::map<float, int> &o_SeekPos,
							  bool i_bWriteNormals = true);
	//void PrepareVertexAnimation(MFnSubd &subdiv, chWriter &o_Writer,
	//						  const gfFileBin& i_File,
	//						  const std::list<float> &i_Keys,
	//						  std::map<float, int> &o_SeekPos);

	//========================================================================
	//	WriteVertexAnimationFrame - assuming Maya timeline is at the correct
	//		time, seek to position in file and write out animation data
	//		for this mesh at this time.
	//========================================================================
	void WriteVertexAnimationFrame( const MFnMesh &mesh, 
								     chWriter &o_Writer,
								     gfFileBin& io_File,
								     float i_Frame,
								     float i_TimeOffset,
								     const std::map<float, int> &i_SeekPos,
								     bool i_bWriteNormals,
									 bool i_bWorldSpace );
	//void WriteVertexAnimationFrame( const MFnSubd &subdiv, 
	//							     chWriter &o_Writer,
	//							     gfFileBin& io_File,
	//							     float i_Frame,
	//							     float i_TimeOffset,
	//							     const std::map<float, int> &i_SeekPos );

	//========================================================================
	//	MeshHashVertexAnimation - return true if there is 
	//	some vertex animation on the given mesh.
	//========================================================================
	bool MeshHashVertexAnimation( MFnMesh& mesh );

	//========================================================================
	//	WriteVertexAnimation - write animation of vertices to file.
	//========================================================================
	void WriteMeshVertexAnimation(MDagPath& dagPath, 
								MString mesh_name,
								chWriter &o_Writer,
								const std::list<float> &i_Keys,
								float i_TimeOffset,
								bool i_bWriteNormals = true,
								bool i_bWorldSpace = true);
	void WriteMeshVertexAnimation(MDagPath& dagPath, 
							 chWriter &o_Writer,
							 double i_MinTime, 
							 double i_MaxTime);
	//void WriteVertexAnimation(MFnSubd &subd, chWriter &o_Writer,
	//						  const std::list<float> &i_Keys,
	//						  float i_TimeOffset);
	//void WriteVertexAnimation(MFnSubd &subdiv, chWriter &o_Writer);
	void WriteVertexAnimation(MFnParticleSystem &particle, 
							  chWriter &o_Writer,
							  const std::list<float> &i_Keys,
							  float i_TimeOffset);

	//========================================================================
	// Write subdivision animation as baked mesh vertex animation
	//  VERY SLOW!
	//========================================================================
	void BakeVertexAnimation(MFnSubd &subd, int depth,
							 chWriter &o_Writer,
						     const std::list<float> &i_Keys,
						     MMatrix &matrix,
							 float i_TimeOffset);

}


