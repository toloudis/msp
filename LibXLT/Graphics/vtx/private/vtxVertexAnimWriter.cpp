/****************************************************************************\
**	vtxVertexAnimWriter.hpp
**
**		vtxVertexAnimWriter.hpp supplies functions used to export vertex anim data into
**	our animation file formats	
**
**	StudioGPU
**	Copyright(C) 2003-8 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxVertexAnimWriter.hpp"

#include "Core/Ch/chBinWriter.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/vtx/private/vtxZLibCompress.hpp"
#include "Graphics/vtx/vtxVertexAnimKeysStatic.hpp"

#include <vector>


//============================================================================
//	Any of these vtxVertexAnimWriter functions might throw one of the fs exceptions.
//============================================================================
namespace vtxVertexAnimWriter
{
	//=============================================================================
	//	Chunk types
	//=============================================================================
	namespace 
	{
		const chDefs::Name c_VTXA = chDefs::MakeName('V', 'T', 'X', 'A');
		const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
		const chDefs::Name c_FRAM = chDefs::MakeName('F', 'R', 'A', 'M');
		const chDefs::Name c_TIME = chDefs::MakeName('T', 'I', 'M', 'E');
		const chDefs::Name c_GVER = chDefs::MakeName('G', 'V', 'E', 'R');
		const chDefs::Name c_NVER = chDefs::MakeName('N', 'V', 'E', 'R');
		//const chDefs::Name c_NCMP = chDefs::MakeName('N', 'C', 'M', 'P');
		const chDefs::Name c_BBOX = chDefs::MakeName('B', 'B', 'O', 'X');
		const chDefs::Name c_VDLT = chDefs::MakeName('V', 'D', 'L', 'T');
		const chDefs::Name c_FRDT = chDefs::MakeName('F', 'R', 'D', 'T');
		
		//------------------------------------------------------------------------
		// Common code for writing a frame of vertex animation data without
		// any specific chunk name so that it can be used in FRDT and FRAM chunks.
		//o_Writer is the binary chunk writer
		//i_FrameData is the animation data
		//------------------------------------------------------------------------	
		void write_vertex_frame_data(  chWriter &io_Writer, 
									   const vtxVertexFrame& i_FrameData)
									// bool i_bTwoComponentNormals)
		{	
			const vtxVertexFrame::VertContainerT &positions = i_FrameData.m_Positions;
			const vtxVertexFrame::NormContainerT &normals = i_FrameData.m_Normals;
			int nVerts = positions.size();
			int nNormals = normals.size();
		
			//vertex array chunk
			io_Writer.WriteChunkHeader(c_GVER, 1, false);
			io_Writer.Write(envType::Int32(nVerts));
			if ( nVerts > 0)
			{
				io_Writer.Write(&positions[0], nVerts*sizeof(maPoint3d));			
			}
			io_Writer.FinishChunk();
			
			if ( nNormals > 0)
			{
				//if (i_bTwoComponentNormals)
				//{
				//	// Write just the X and Y values of the normals
				//	io_Writer.WriteChunkHeader(c_NCMP, 0, false);
				//	io_Writer.Write(envType::Int32(nNormals));
				//	for (int i=0; i<nNormals; ++i)
				//	{
				//		io_Writer.Write(normals[i].m_X);
				//		io_Writer.Write(normals[i].m_Y);
				//	}
				//	io_Writer.FinishChunk();

				//}
				//else
				//{
					//normal array chunk
					io_Writer.WriteChunkHeader(c_NVER, 1, false);
					io_Writer.Write(envType::Int32(nNormals));
					io_Writer.Write(&normals[0], nNormals*sizeof(maVector3d));
					io_Writer.FinishChunk();
				//}
			}
		}
	}

	//------------------------------------------------------------------------
	// Write the geoemtry data for a single vertex frame.
	//o_Writer is the binary chunk writer
	//i_FrameData is the animation data
	//------------------------------------------------------------------------	
	void WriteVertexFrameData( chWriter &io_Writer, 
							   const vtxVertexFrame& i_FrameData)
							   //bool i_bTwoComponentNormals)
	{	
		// Use FRDT (FRame DaTa) in order to mark that this has two chunks
		// and is different than the FRAM which has time and BBox in it.
		io_Writer.WriteChunkHeader(c_FRDT, 0, true);
				
		write_vertex_frame_data(io_Writer, i_FrameData);

		io_Writer.FinishChunk(); // FRDT
	}

	//------------------------------------------------------------------------
	//write the vertex animation data
	//o_Writer is the binary chunk writer
	//i_Callback, is the callback struct we can call  on each vertexframe
	//i_MeshName, is the name of the mesh that we are animating
	//i_VetexAnimKeys is the animation data
	//------------------------------------------------------------------------	
	void WriteVertexAnimForMesh( chWriter &io_Writer, 
								 WriteVertexAnimForMeshCallback &i_Callback,
								 const std::string &i_MeshName,
								 shared_ptr< vtxVertexAnimKeysStatic > &i_VertexAnimKeys)
	{	
		const anKeyDataBase<vtxVertexFrame*> &frames = i_VertexAnimKeys->GetFrames();
		int nKeys = frames.GetNumKeys();
		if ( nKeys > 0 )
		{
			//vertex animation chunk
			io_Writer.WriteChunkHeader(c_VTXA, 0, true);
			// Name of mesh
			io_Writer.WriteChunkHeader(c_NNAM, 0, false);
			io_Writer.Write(i_MeshName); 
			io_Writer.FinishChunk();
			
			for ( int k=0; k < nKeys; ++k)
			{
				vtxVertexFrame *pThisFrame=NULL;
				float thisTime =0;
				frames.GetKeyData( k, thisTime, pThisFrame);

				const vtxVertexFrame::VertContainerT &positions = pThisFrame->m_Positions;
				int nVerts = positions.size();
				
				//call back with this vertex frame
				i_Callback( pThisFrame );
				//frame chunk
				io_Writer.WriteChunkHeader(c_FRAM, 0, true);
				//time chunk
				io_Writer.WriteChunkHeader(c_TIME, 0, false);
				io_Writer.Write( thisTime ); 
				io_Writer.FinishChunk();

				// write positions and normals to file
				write_vertex_frame_data(io_Writer, *pThisFrame);

				// Compute bounding box for this vertex frame
				maAxisBox bbox;
				for (int i=0; i < nVerts; ++i )
				{
					bbox.Union( positions[i] );
				}
				if (!bbox.IsEmpty())
				{
					//bounding box chunk
					io_Writer.WriteChunkHeader(c_BBOX, 0, false);
					maPoint3d min_pt( bbox.GetMinX(), bbox.GetMinY(), bbox.GetMinZ() );
					maPoint3d max_pt( bbox.GetMaxX(), bbox.GetMaxY(), bbox.GetMaxZ() );
					chChunkParserUtil::Write( io_Writer, min_pt );
					chChunkParserUtil::Write( io_Writer, max_pt );
					io_Writer.FinishChunk();
				}				
				//frame chunk is finished
				io_Writer.FinishChunk();
			}
			//vrtex animation chunk
			io_Writer.FinishChunk();
		}
			
	}

	//------------------------------------------------------------------------
	// Write encoded deltas as a large buffer of memory,
	// compressed with ZLib compression.
	//------------------------------------------------------------------------
	void WriteDeltas(chWriter &o_Writer,
					 const void* i_Data, 
					 int i_NumBytes)
	{
		o_Writer.WriteChunkHeader(c_VDLT, 0, false);
		vtxZLibCompress::WriteCompressed(o_Writer, i_Data, i_NumBytes);
		o_Writer.FinishChunk();
	}
}

