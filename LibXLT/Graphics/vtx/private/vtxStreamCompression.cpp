/*****************************************************************************
**  vtxStreamCompression.cpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxStreamCompression.hpp"

#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/Ma/maFunctions.hpp"
#include "Graphics/vtx/private/vtxDeltaCompression.hpp"
#include "Graphics/vtx/vtxExceptionX.hpp"
#include "Graphics/vtx/vtxGeometryCacheWriter.hpp"


//============================================================================
//============================================================================
namespace
{
	const chDefs::Name c_VTXC = chDefs::MakeName('V', 'T', 'X', 'C');
	const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
	const chDefs::Name c_VCFG = chDefs::MakeName('V', 'C', 'F', 'G');
	const chDefs::Name c_CFRS = chDefs::MakeName('C', 'F', 'R', 'S');
	const chDefs::Name c_CFRM = chDefs::MakeName('C', 'F', 'R', 'M');
}


//--------------------------------------------------------------------
// Constructor requires a geometry cache to which to write data
// when necessary.
// i_SyncInterval defines how often a new reference frame 
//	needs to be written.
//--------------------------------------------------------------------
vtxStreamCompression::vtxStreamCompression( const std::string &i_SurfaceName,
											int i_SyncInterval,
											vtxGeometryCacheWriter &io_GeometryCache,
											const std::string &i_EncoderString,
											shared_ptr<vtxDeltaEncoder> &io_DeltaEncoder,
											shared_ptr<vtxDeltaEncoder> &io_NormalEncoder)
:	m_SurfaceName(i_SurfaceName),
	m_EncoderString(i_EncoderString),
	m_GeometryCache(io_GeometryCache),
	m_DeltaEncoder(io_DeltaEncoder),
	m_NormalEncoder(io_NormalEncoder),
	m_SyncInterval(i_SyncInterval),
	m_FrameCount(0),
	m_bHaveNormals(false)
{
}

//--------------------------------------------------------------------
// Compare this frame with the current stream data and write 
//	animation data to the geometry cache when needed.
//--------------------------------------------------------------------
void vtxStreamCompression::SubmitFrame(float i_Time, const vtxVertexFrame &i_Frame)
{
	const int num_vertices = i_Frame.m_Positions.size();
	const int num_normals = i_Frame.m_Normals.size();

	// The normals have to be unit length for our compression code to work,
	// so, make a copy of the given frame and make sure all normals are
	// unit length.
	m_CurrentFrame = i_Frame;
	for (int n=0; n<num_normals; n++)
	{
		if (!m_CurrentFrame.m_Normals[n].Normalize())
			m_CurrentFrame.m_Normals[n].Set(0,0,1);
	}

	if (m_Deltas.empty())
	{
		// first frame submitted, allocate some memory
		m_Deltas.resize(num_vertices);
		// The number of frames in the delta block is one less than the
		// sync interval
		m_DeltaEncoder->Allocate(m_SyncInterval-1, num_vertices);

		// Also allocate for normals, if they exist
		if (num_normals > 0)
		{
			m_NormalDeltas.resize(num_normals);
			m_NormalEncoder->Allocate(m_SyncInterval-1, num_normals);
			m_bHaveNormals = true;
		}
	}
	else
	{
		// Confirm that we have the correct number of vertices
		if ((num_vertices != m_Deltas.size()) ||
			(num_normals != m_NormalDeltas.size()))
		{
			DBG_ERROR("Number of vertices and normals submitted to compression stream is not allowed to vary.");
			throw vtxCompressErrorX( m_GeometryCache.GetLocator() );
		}
	}

	bool bWriteReferenceFrame = true;
	if (m_FrameCount % m_SyncInterval != 0)
	{
		int prev_frame_index = (m_LastFrameIndex + 1) % 2;
		vtxVertexFrame *pFrame0 = &m_LastFrames[ prev_frame_index ];
		vtxVertexFrame *pFrame1 = &m_LastFrames[ m_LastFrameIndex ];

		// Compute deltas and add them to our encoded buffer
		vtxDeltaCompression::GetExtrapolatedDeltaVectors(*pFrame0, *pFrame1, m_CurrentFrame, m_Deltas);
		if (m_DeltaEncoder->SubmitDeltas(m_Deltas))
		{
			if (m_bHaveNormals)
			{
				vtxDeltaCompression::GetExtrapolatedDeltaNormals(*pFrame0, *pFrame1, m_CurrentFrame, m_NormalDeltas);
				bool bNormalsSuccess = m_NormalEncoder->SubmitDeltas(m_NormalDeltas);
				DBG_ASSERT(bNormalsSuccess, "Need code to unwind delta encoding if normals fail.");
			}

			// If the delta encoder is altering values during its encoding, then the
			// decompression code will be computing extrapolated values from the
			// different reconstructed data. This causes errors to multiply with each frame,
			// so we need to decompress the data ourself here and compute the next set of
			// of deltas from that lossy data.
			if (m_DeltaEncoder->IsLossyCompression())
			{
				// First decode the delta vectors
				int frame_index = m_DeltaEncoder->GetNumFramesSubmitted()-1;
				m_DeltaEncoder->ReceiveDeltas(frame_index, m_CurrentFrame.m_Positions);
				DBG_ASSERT(num_vertices==m_CurrentFrame.m_Positions.size(), "Received deltas vector of the wrong size");

				if (m_bHaveNormals)
				{
					frame_index = m_NormalEncoder->GetNumFramesSubmitted()-1;
					m_NormalEncoder->ReceiveDeltas(frame_index, m_CurrentFrame.m_Normals);
				}
				// Then apply back in the extrapolation
				vtxDeltaCompression::ReconstructFromExtrapolatedDeltas(*pFrame0, *pFrame1, m_CurrentFrame);
			}

			// Fill our last frame buffer with the data from this frame
			m_LastFrameIndex = prev_frame_index;
			m_LastFrames[m_LastFrameIndex] = m_CurrentFrame;

			// Deltas were successfully encoded, so don't have to write reference frame
			bWriteReferenceFrame = false;
			m_FrameCount++;
		}
	}

	// Gather frame information
	sFrameData frame_data;
	frame_data.m_Time = i_Time;

	// Gather bounding box info
	for (int v=0; v<num_vertices; ++v) 
		frame_data.m_BBox.Union( m_CurrentFrame.m_Positions[v] );

	// If bWriteReferenceFrame is true... either it is time to write the reference frames, 
	// or the deltas were too large
	if (bWriteReferenceFrame)
	{
		// Write out any current deltas we might have
		write_deltas();

		// Write the full data for the reference frame
		m_RefFrameOffset = m_GeometryCache.GetCacheOffset();
		m_GeometryCache.WriteReferenceFrame(m_CurrentFrame);

		frame_data.m_ReferenceFrame = m_RefFrameOffset;
		m_FrameInfoList.push_back( frame_data );

		// Reset our "last frame" buffers with the data from this frame
		m_LastFrames[0] = m_CurrentFrame;
		m_LastFrames[1] = m_CurrentFrame;
		m_LastFrameIndex = 0;
		m_FrameCount = 1;
	}
	else
	{
		// Store frame information in deltas list, which waits
		// until the deltas block is actually written out
		frame_data.m_ReferenceFrame = m_RefFrameOffset;
		m_DeltasList.push_back( frame_data );
	}

}

//--------------------------------------------------------------------
// Animation data is finished, flush the current stream of 
//	animation data to the geometry cache.
//--------------------------------------------------------------------
void vtxStreamCompression::Finish()
{
	// Write out any current deltas we might have left over
	write_deltas();
}

//--------------------------------------------------------------------
// Write out frame information, including references into 
// geometry cache.
//--------------------------------------------------------------------
void vtxStreamCompression::WriteCompressedAnimationData(chWriter &o_Writer)
{
	DBG_ASSERT(!m_GeometryCache.IsOpen(), "Have to close geometry cache before writing animation data.");
	if (m_GeometryCache.IsOpen())
		return;

	// Write the vertex animation frames for this surface now,
	// after the goemetry cache is finished.
	o_Writer.WriteChunkHeader(c_VTXC, 0, true);

	o_Writer.WriteChunkHeader(c_NNAM, 0, false);
	chChunkParserUtil::Write(o_Writer, m_SurfaceName);
	o_Writer.FinishChunk();

	o_Writer.WriteChunkHeader(c_VCFG, 0, false);
	envType::Int32 max_frames_per_block = m_SyncInterval-1;
	o_Writer.Write(max_frames_per_block);
	envType::Int32 num_vertices = m_Deltas.size();
	envType::Int32 num_normals = m_NormalDeltas.size();
	o_Writer.Write(num_vertices);
	o_Writer.Write(num_normals);
	chChunkParserUtil::Write(o_Writer, m_EncoderString);
	o_Writer.FinishChunk();

	o_Writer.WriteChunkHeader(c_CFRS, 0, true);
	this->write_frames(o_Writer);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();	// c_VTXC
}

//--------------------------------------------------------------------
// Write out frame information, including references into 
// geometry cache.
//--------------------------------------------------------------------
void vtxStreamCompression::write_frames(chWriter &o_Writer)
{
	std::list<sFrameData>::const_iterator it;
	for (it = m_FrameInfoList.begin(); it != m_FrameInfoList.end(); ++it)
	{
		o_Writer.WriteChunkHeader(c_CFRM, 0, false);

		o_Writer.Write( it->m_Time );
		o_Writer.Write( it->m_ReferenceFrame );
		o_Writer.Write( it->m_DeltaBlock );
		o_Writer.Write( it->m_NormalsBlock );
		o_Writer.Write( it->m_DeltaOffset );

		maPoint3d min_pt(it->m_BBox.GetMinX(), it->m_BBox.GetMinY(), it->m_BBox.GetMinZ());
		maPoint3d max_pt(it->m_BBox.GetMaxX(), it->m_BBox.GetMaxY(), it->m_BBox.GetMaxZ());
		chChunkParserUtil::Write(o_Writer, min_pt);
		chChunkParserUtil::Write(o_Writer, max_pt);

		chChunkParserUtil::Write(o_Writer, it->m_bStatic);

		o_Writer.FinishChunk();
	}
}

//--------------------------------------------------------------------
// Write delta buffer to geometry cache
//--------------------------------------------------------------------
void vtxStreamCompression::write_deltas()
{
	int buffer_size = m_DeltaEncoder->GetFilledBufferSize();
	if (buffer_size > 0)
	{
		// If just a bunch of zeroes in deltas, skip this delta block
		bool bWriteNormals = (m_bHaveNormals && !m_NormalEncoder->GetAllDeltasAreZero());

		if (!m_DeltaEncoder->GetAllDeltasAreZero())
		{
			fsFileStream::FilePosType deltas_cache_offset = m_GeometryCache.GetCacheOffset();
			m_GeometryCache.WriteDeltas(m_DeltaEncoder->GetBuffer(), buffer_size);

			fsFileStream::FilePosType normal_cache_offset = -1;
			if (bWriteNormals)
			{
				int normals_buffer_size = m_NormalEncoder->GetFilledBufferSize();
				if (normals_buffer_size > 0)
				{
					normal_cache_offset = m_GeometryCache.GetCacheOffset();
					m_GeometryCache.WriteDeltas(m_NormalEncoder->GetBuffer(), normals_buffer_size);
				}
			}

			int delta_index = 0;
			std::list<sFrameData>::iterator it;
			for (it = m_DeltasList.begin(); it != m_DeltasList.end(); ++it)
			{
				// Set the cache offset for the delta block now that we have
				// the file position within the goemetry cache.
				it->m_DeltaBlock = deltas_cache_offset;
				it->m_NormalsBlock = normal_cache_offset;
				it->m_DeltaOffset = delta_index++;
				m_FrameInfoList.push_back(*it);
			}
		}
		else
		{
			// There was a large section of frames with no changes,
			// so mark the previous reference frame as "static" so that
			// blending between frames is turned off
			if (!m_FrameInfoList.empty())
				m_FrameInfoList.back().m_bStatic = true;
		}

		m_DeltasList.clear(); // clear list whether we stored the info or not
	}
	m_DeltaEncoder->Reset();
	m_NormalEncoder->Reset();
}

