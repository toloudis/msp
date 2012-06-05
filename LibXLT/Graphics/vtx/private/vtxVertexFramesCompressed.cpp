/*****************************************************************************
**  vtxVertexFramesCompressed.cpp
**
**		see.hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxVertexFramesCompressed.hpp"

#include "Core/Ch/chBinReader.hpp"
#include "Core/Env/envSTLHelpers.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "Graphics/vtx/private/vtxDeltaCompression.hpp"
#include "Graphics/vtx/private/vtxVertexFileSeeker.hpp"
#include "Graphics/vtx/vtxDeltaEncoder.hpp"
#include "Graphics/vtx/vtxVertexAnimImport.hpp"
#include "Graphics/vtx/vtxVertexFramesDynamic.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
vtxCompressedDeltasSet::vtxCompressedDeltasSet(shared_ptr<vtxDynamicVertexSet>& i_ReferenceFrames,
											   shared_ptr<vtxDeltaEncoder>& i_Encoder,
											   shared_ptr<vtxDeltaEncoder>& i_NormalEncoder,
											   int i_MaxNumFramesPerBlock,
											   int i_NumVertices, 
											   int i_NumNormals,
											   shared_ptr<vtxVertexFileSeeker> i_FileSeeker)
:	m_FileSeeker(i_FileSeeker),
	m_ReferenceFrames(i_ReferenceFrames),
	m_Encoder(i_Encoder),
	m_NormalEncoder(i_NormalEncoder),
	m_MaxNumFramesPerBlock(i_MaxNumFramesPerBlock), 
	m_NumVertices(i_NumVertices), 
	m_NumNormals(i_NumNormals), 
	m_bFullyLoaded(false), 
	m_bBudgeting(false), 
	m_LastLoaded(0), 
	m_Size(0)
{
	//m_bFullyLoaded = vtxVertexAnimBudget::AddItem(this);

	// Only attach to the file seeker if we are not fully loaded.
	if (!m_bFullyLoaded)
	{
		// Allocate memory in the temporary delta block
		allocate_temp_block();

		m_FileSeeker->Attach();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
vtxCompressedDeltasSet::~vtxCompressedDeltasSet()
{
	// Free the full buffer, if we used it
	envSTLHelpers::DeleteContainer(m_FullBuffer);

	// free up the memory from the temporary deltas block
	m_TempBlock.clear();

	// Detach from the file seeker if we were using it.
	if (!m_bFullyLoaded)
		m_FileSeeker->Detach();

	// Notify manager that we are not taking up memory anymore
	if (m_bBudgeting)
		vtxVertexAnimBudget::RemoveItem(this);
}

//--------------------------------------------------------------------
// Compute memory needs for the delta blocks and add this
// item to the vertex animation budgeting based on that size.
// This may cause all data to be loaded.
//--------------------------------------------------------------------
void vtxCompressedDeltasSet::SubmitForBudgeting()
{
	// Compute the size we need if fully loaded, overestimate
	// assumes all blocks will be full to maximum.
	// 12 bytes per maVector3d vector (3 32 bit floats)
	int num_blocks = m_DeltasFilePositions.size();
	m_Size = (m_NumVertices * 12 + m_NumNormals * 12) * m_MaxNumFramesPerBlock * num_blocks;

	bool bLoadAllData = vtxVertexAnimBudget::AddItem(this);
	m_bBudgeting = true;

	// If the budget manager says we can load all data, do it now
	if (bLoadAllData)
		this->FullyLoadData();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool vtxCompressedDeltasSet::IsFullyLoaded() const
{
	return m_bFullyLoaded;
}

//--------------------------------------------------------------------
// Get number of vertices in a frame of animation data.
// Used to make sure the vertex data is compatible with the
// geometry.
//--------------------------------------------------------------------
void vtxCompressedDeltasSet::GetNumVertices(int &o_NumVertices, int &o_NumNormals)
{
	o_NumVertices = m_NumVertices;
	o_NumNormals = m_NumNormals;
}

//--------------------------------------------------------------------
// This class tracks file positions for each frame of vertex data.
// This functions adds the file position for the next frame of data.
//--------------------------------------------------------------------
int vtxCompressedDeltasSet::AddFilePos(fsFileStream::FilePosType i_DeltasFilePos,
									   fsFileStream::FilePosType i_NormalsFilePos,
									   int i_ReferenceFrameIndex)
{
	int index = m_DeltasFilePositions.size();
	m_DeltasFilePositions.push_back(i_DeltasFilePos);
	m_NormalsFilePositions.push_back(i_NormalsFilePos);
	m_ReferenceFrameIndices.push_back(i_ReferenceFrameIndex);
	return index;
}

//--------------------------------------------------------------------
// Get animation delta data, loading from disk if necessary.
// It is only possible to ask for two frames from the same delta 
// block. If you are only interested in one frame, then set
// i_Frame0=i_Frame1
//--------------------------------------------------------------------
void vtxCompressedDeltasSet::GetDeltas(int i_DeltaBlockIndex, 
									   int i_Frame0, 
									   int i_Frame1, 
									   vtxVertexFrame* &o_pFrame0, 
									   vtxVertexFrame* &o_pFrame1)
{
	DeltaBlock *pBlock = NULL;

	// If fully loaded, just return the frames
	if (m_bFullyLoaded)
	{
		DBG_ASSERT(i_DeltaBlockIndex<m_FullBuffer.size(), "Delta Block index out of range");
		if (i_DeltaBlockIndex < m_FullBuffer.size())
			pBlock = m_FullBuffer[i_DeltaBlockIndex];
	}
	else
	{
		// Otherwise, look for frames in the delta block
		// we have in temporary memory buffer.
		if (i_DeltaBlockIndex != m_LastLoaded)
		{
			load_deltas(i_DeltaBlockIndex, m_TempBlock);
		}
		pBlock = (&m_TempBlock);
	}	

	if (pBlock)
	{
		DBG_ASSERT(i_Frame0<pBlock->size(), "Frame index out of range");
		DBG_ASSERT(i_Frame1<pBlock->size(), "Frame index out of range");
		if (i_Frame0<pBlock->size() && i_Frame1<pBlock->size())
		{
			o_pFrame1 = &(pBlock->at(i_Frame1));
			o_pFrame0 = &(pBlock->at(i_Frame0));
		}
		else
		{
			// Return null pointers
			o_pFrame1 = o_pFrame0 = NULL;
		}
	}
	else
	{
		// Return null pointers
		o_pFrame1 = o_pFrame0 = NULL;
	}
}

//--------------------------------------------------------------------
// Note: this is a call from the budget manager and signifies
//	that we have been moved from swapping to a fully loaded state.
// Load in all frames of animation from the file and close
// our file handle.
//--------------------------------------------------------------------
void vtxCompressedDeltasSet::FullyLoadData()
{
	if (!m_bFullyLoaded)
	{
		DBG_ASSERT(m_FullBuffer.empty(), "Buffer should be fully empty before loading, no partial states yet");

		const int num_frames = m_DeltasFilePositions.size();
		m_FullBuffer.resize( num_frames, NULL );
		for (int i=0; i<num_frames; i++)
		{
			std::auto_ptr<DeltaBlock> new_deltas(new DeltaBlock());
			load_deltas(i, *new_deltas);
			m_FullBuffer[i] = new_deltas.release();
		}
		
		// free up the memory from the temporary deltas block
		m_TempBlock.clear();

		// Don't need the file seeker anymore
		m_FileSeeker->Detach();

		// Set flag to mark that we are now fully loaded
		m_bFullyLoaded = true;
	}
}

//--------------------------------------------------------------------
// Note: this is a call from the budget manager and signifies
//	that we have been moved from a fully loaded to a swapping state.
// Load in all frames of animation from the file and close
// our file handle.
//--------------------------------------------------------------------
void vtxCompressedDeltasSet::FreeDataAndStartSwapping()
{
	if (m_bFullyLoaded)
	{
		// If we had a full buffer and are now switching to
		// paging, set up the pages to the correct size
		bool bHavePageData = false;
		DeltaBlock *pFirstPage = NULL;
		if (m_TempBlock.empty() && !m_FullBuffer.empty())
		{
			// Steal the pointer to the first frame of 
			// data for our use in the pages.
			bHavePageData = true;
			pFirstPage = m_FullBuffer[0];
			m_FullBuffer[0] = NULL;
		}

		m_bFullyLoaded = false;
		envSTLHelpers::DeleteContainer(m_FullBuffer);

		// We are doing this after freeing the other frames because
		// this function might have been called when system memory is tight.
		// So it is better to free what memory we can first and then
		// allocate the pages.
		allocate_temp_block(); // make sure temp block is full size even if first block is smaller

		if (bHavePageData)
		{
			// Move deltas from first block of full buffer into temp block
			for (int i=0; i<pFirstPage->size(); ++i)
			{
				m_TempBlock[i] = (*pFirstPage)[i];
			}
			delete pFirstPage;
			m_LastLoaded = 0; // set loaded index to 0 because we took the first block
		}

		// Attach to the file seeker in order to load frames as needed
		m_FileSeeker->Attach();
	}
}

//--------------------------------------------------------------------
// Allocate memory in the temporary delta block
//--------------------------------------------------------------------
void vtxCompressedDeltasSet::allocate_temp_block()
{
	// If no data is loaded, then make sure the loaded index is invalid
	// by setting it to -1
	m_LastLoaded = -1;

	// Allocate vector large enough for the max number of frames per block
	m_TempBlock.resize(m_MaxNumFramesPerBlock);
	for (int i=0; i<m_MaxNumFramesPerBlock; ++i)
	{
		// Allocating the vertex and normal arrays within the temp block 
		// completely so that we don't have to allocate memory when animating
		m_TempBlock[i].m_Positions.reserve( m_NumVertices );
		m_TempBlock[i].m_Normals.reserve( m_NumNormals );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void vtxCompressedDeltasSet::load_deltas(int i_DeltaBlockIndex, 
										 DeltaBlock &o_DeltaBlock)
{
	// First get the reference frame
	DBG_ASSERT(i_DeltaBlockIndex<m_DeltasFilePositions.size(), "Reference Frame index out of range");
	if (i_DeltaBlockIndex >= m_DeltasFilePositions.size())
		return;
	int referenceFrameIndex = m_ReferenceFrameIndices[i_DeltaBlockIndex];
	// Using same frame index will return the same frame twice, 
	// could add special single frame function
	// This call to m_VertexSet may cause a read from disk,
	vtxVertexFrame* pFrame = NULL, *pFrame1  = NULL;
	m_ReferenceFrames->GetVertexFrames(referenceFrameIndex, referenceFrameIndex, pFrame, pFrame1);

	// mutex scope
	{
		// Lock the file seeker while we move the file position and read our data
		envScopedLock read_lock(m_FileSeeker->GetMutex());

		// Move the file position
		DBG_ASSERT(i_DeltaBlockIndex<m_DeltasFilePositions.size(), "Delta Block index out of range");
		m_FileSeeker->SetFilePos(m_DeltasFilePositions[i_DeltaBlockIndex]);

		// Read the compressed deltas data
		int amount_read = 0;
		vtxVertexAnimImport::ReadDeltas(m_FileSeeker->GetReader(), m_Encoder->Buffer(), m_Encoder->GetFullBufferSize(), amount_read);
		m_Encoder->SetFilledBufferSize(amount_read); // tell encoder how much data we just put into the buffer

		// Move the file position
		DBG_ASSERT(i_DeltaBlockIndex<m_NormalsFilePositions.size(), "Delta Block index out of range");
		fsFileStream::FilePosType normals_fpos = m_NormalsFilePositions[i_DeltaBlockIndex];
		amount_read = 0;
		if (normals_fpos >= 0)
		{
			m_FileSeeker->SetFilePos(normals_fpos);
			vtxVertexAnimImport::ReadDeltas(m_FileSeeker->GetReader(), m_NormalEncoder->Buffer(), m_NormalEncoder->GetFullBufferSize(), amount_read);
		}
		m_NormalEncoder->SetFilledBufferSize(amount_read); // tell encoder how much data we just put into the buffer
	}

	//DBG_LOG("Loaded compressed deltas: num frames " << m_Encoder->GetNumFramesSubmitted());
	//DBG_LOG("Loaded compressed normals: num frames " << m_NormalEncoder->GetNumFramesSubmitted());

	int num_frames = m_Encoder->GetNumFramesSubmitted();
	if (o_DeltaBlock.size() < num_frames)
		o_DeltaBlock.resize(num_frames);
	bool bHaveNormals = (num_frames == m_NormalEncoder->GetNumFramesSubmitted());
	for (int f=0; f<num_frames; f++)
	{
		m_Encoder->ReceiveDeltas(f, o_DeltaBlock[f].m_Positions);
		DBG_ASSERT(m_NumVertices==o_DeltaBlock[f].m_Positions.size(), "Received deltas vector of the wrong size");
		if (m_NumVertices!=o_DeltaBlock[f].m_Positions.size())
			continue;

		if (bHaveNormals)
			m_NormalEncoder->ReceiveDeltas(f, o_DeltaBlock[f].m_Normals);
		else
			o_DeltaBlock[f].m_Normals = pFrame->m_Normals; // no deltas, so use reference frame's normals
	}
	// Since these are extrapolated deltas, we need to sum these deltas with the 
	// deltas from the previous frames
	for (int f=0; f<num_frames; f++)
	{
		vtxVertexFrame & cur = o_DeltaBlock[f];
		// first two frames are just deltas from the reference frame
		if (f == 0)
		{
			for (int v=0; v<m_NumVertices; v++)
				cur.m_Positions[v] += pFrame->m_Positions[v]; 

			if (bHaveNormals)
			{
				for (int n=0; n<m_NumNormals; n++)
				{
					cur.m_Normals[n].m_X += pFrame->m_Normals[n].m_X; 
					cur.m_Normals[n].m_Y += pFrame->m_Normals[n].m_Y; 
				}
			}
		}
		else if (f == 1)
		{
			vtxVertexFrame & prev1 = o_DeltaBlock[f-1];
			vtxDeltaCompression::ReconstructFromExtrapolatedDeltas(*pFrame, prev1, cur, bHaveNormals);
		}
		else
		{
			// The rest of the frames are deltas from an
			// extrapolated position
			vtxVertexFrame & prev1 = o_DeltaBlock[f-1];
			vtxVertexFrame & prev2 = o_DeltaBlock[f-2];
			vtxDeltaCompression::ReconstructFromExtrapolatedDeltas(prev2, prev1, cur, bHaveNormals);
		}
	
		// Normal compression is only setting the X and Y values,
		// so we have to go back and compute the Z value.
		if (bHaveNormals)
		{
			float length_sqr;
			for (int n=0; n<m_NumNormals; n++)
			{
				maVector3d &normal = cur.m_Normals[n]; 
				length_sqr = (normal.m_X*normal.m_X + normal.m_Y*normal.m_Y);
				if (length_sqr > 1)
					normal.m_Z = 0;
				else
					normal.m_Z *= ::sqrtf(1.0f - length_sqr); // Z component was either 1 or -1 before
				normal.Normalize();
			}
		}
	}

	m_LastLoaded = i_DeltaBlockIndex;
}

//--------------------------------------------------------------------
// Create a new set of reference vertex animation frames to management
//--------------------------------------------------------------------
shared_ptr<vtxDynamicVertexSet> vtxVertexFramesCompressed::CreateVertexSet(const fsLocator &i_Locator,
																		  chDefs::Size i_Size)
{
	if (!m_FileSeeker.get())
	{
		m_FileSeeker.reset( new vtxVertexFileSeeker(i_Locator) );
	}

	shared_ptr<vtxDynamicVertexSet> new_vertex_set(new vtxDynamicVertexSet(m_FileSeeker, i_Size));
	m_VertexSets.push_back( new_vertex_set );
	return new_vertex_set;
}

//--------------------------------------------------------------------
// Create a new set of compressed deltas blocks to management
//--------------------------------------------------------------------
shared_ptr<vtxCompressedDeltasSet> vtxVertexFramesCompressed::CreateDeltasSet(const fsLocator &i_Locator,
																		shared_ptr<vtxDynamicVertexSet>& i_ReferenceFrames,
																		shared_ptr<vtxDeltaEncoder>& i_Encoder,
																		shared_ptr<vtxDeltaEncoder>& i_NormalEncoder,
																		int i_MaxNumFramesPerBlock,
																		int i_NumVertices, 
																		int i_NumNormals)
{
	if (!m_FileSeeker.get())
	{
		m_FileSeeker.reset( new vtxVertexFileSeeker(i_Locator) );
	}

	shared_ptr<vtxCompressedDeltasSet> new_vertex_set(
		new vtxCompressedDeltasSet(i_ReferenceFrames, i_Encoder, i_NormalEncoder, i_MaxNumFramesPerBlock, 
								   i_NumVertices, i_NumNormals, m_FileSeeker));
	m_DeltasSets.push_back( new_vertex_set );
	return new_vertex_set;
}
