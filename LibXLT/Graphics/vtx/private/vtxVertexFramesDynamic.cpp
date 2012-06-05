/*****************************************************************************
**	vtxVertexFramesDynamic.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxVertexFramesDynamic.hpp"

#include "Core/Ch/chBinReader.hpp"
#include "Core/Env/envSTLHelpers.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "Graphics/vtx/private/vtxVertexFileSeeker.hpp"
#include "Graphics/vtx/vtxVertexAnimImport.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
vtxDynamicVertexSet::vtxDynamicVertexSet(shared_ptr<vtxVertexFileSeeker> i_FileSeeker,
										 chDefs::Size i_Size)
:	m_FileSeeker(i_FileSeeker), 
	m_LastUsed(0), 
	m_bFullyLoaded(false), 
	m_bBudgeting(false), 
	m_Size(i_Size)
{
	m_Loaded[0] = m_Loaded[1] = -1;

	// Special meaning for size==0 means we don't know the size yet,
	// so wait for a later call to SubmitForBudgeting()
	if (m_Size > 0)
	{
		m_bFullyLoaded = vtxVertexAnimBudget::AddItem(this);
		m_bBudgeting = true;
	}

	// Only attach to the file seeker if we are not fully loaded.
	if (!m_bFullyLoaded)
		m_FileSeeker->Attach();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
vtxDynamicVertexSet::~vtxDynamicVertexSet()
{
	// Free the full buffer, if we used it
	envSTLHelpers::DeleteContainer(m_FullBuffer);

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
void vtxDynamicVertexSet::SubmitForBudgeting(int i_NumVertices, int i_NumNormals)
{
	DBG_ASSERT(!m_bBudgeting, "Can only submit for budgeting once.");
	if (m_bBudgeting)
		return;

	// Compute the size we need if fully loaded, overestimate
	// assumes all blocks will be full to maximum.
	// 12 bytes per maVector3d vector (3 32 bit floats)
	int num_frames = m_FilePositions.size();
	m_Size = (i_NumVertices * 12 + i_NumNormals * 12) * num_frames;

	bool bLoadAllData = vtxVertexAnimBudget::AddItem(this);
	m_bBudgeting = true;

	// If the budget manager says we can load all data, do it now
	if (bLoadAllData)
		this->FullyLoadData();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool vtxDynamicVertexSet::IsFullyLoaded() const
{
	return m_bFullyLoaded;
}

//--------------------------------------------------------------------
// Get number of vertices in a frame of animation data.
// Used to make sure the vertex data is compatible with the
// geometry.
//--------------------------------------------------------------------
void vtxDynamicVertexSet::GetNumVertices(int &o_NumVertices, int &o_NumNormals)
{
	if (m_FullBuffer.empty() || (m_FullBuffer[0] == NULL))
	{
		o_NumVertices = m_Frame[0].m_Positions.size();
		o_NumNormals = m_Frame[0].m_Normals.size();
	}
	else
	{
		o_NumVertices = m_FullBuffer[0]->m_Positions.size();
		o_NumNormals = m_FullBuffer[0]->m_Normals.size();
	}
}

//--------------------------------------------------------------------
// Adds vertex frame - only used to fill the full buffer
// on initial file load when IsFullyLoaded() is true
//--------------------------------------------------------------------
void vtxDynamicVertexSet::AddVertexFrame(vtxVertexFrame* i_pFrame)
{
	m_FullBuffer.push_back(i_pFrame);
}

//--------------------------------------------------------------------
// Return pointer to buffer for holding first frame of animation
// data. Even when paging anim data, the first frame should be
// loaded in order to know how many vertices and normals  
// will be in each frame of data.
//--------------------------------------------------------------------
vtxVertexFrame* vtxDynamicVertexSet::FirstFrame()
{
	// Allow the caller to fill the first frame of data into 
	// our paging buffer.
	m_Loaded[0] = 0;
	m_LastUsed = 0;
	return (&m_Frame[0]);
}

//--------------------------------------------------------------------
// This should be called after the first frame of data is loaded
// using a call to "FirstFrame()". This will make sure enough
// memory is allocated in the second frame so that we can page
// vertex animation memory without needing to allocate the second
// frame later. 
//--------------------------------------------------------------------
void vtxDynamicVertexSet::AllocateSecondFrame()
{
	m_Frame[1].m_Positions.reserve( m_Frame[0].m_Positions.size() );
	m_Frame[1].m_Normals.reserve( m_Frame[0].m_Normals.size() );
}

//--------------------------------------------------------------------
// This class tracks file positions for each frame of vertex data.
// This functions adds the file position for the next frame of data.
//--------------------------------------------------------------------
int vtxDynamicVertexSet::AddFilePos(fsFileStream::FilePosType i_FilePos)
{
	int index = m_FilePositions.size();
	m_FilePositions.push_back(i_FilePos);
	return index;
}

//--------------------------------------------------------------------
// Get vertex frame data, loading from disk if necessary.
//--------------------------------------------------------------------
void vtxDynamicVertexSet::GetVertexFrames(int i_Frame0, 
										   int i_Frame1, 
										   vtxVertexFrame* &o_pFrame0, 
										   vtxVertexFrame* &o_pFrame1)
{
	// If fully loaded, jsut return the frames
	if (m_bFullyLoaded)
	{
		DBG_ASSERT(i_Frame0<m_FullBuffer.size(), "Frame index out of range");
		if (i_Frame0<m_FullBuffer.size())
			o_pFrame0 = m_FullBuffer[i_Frame0];

		DBG_ASSERT(i_Frame1<m_FullBuffer.size(), "Frame index out of range");
		if (i_Frame1<m_FullBuffer.size())
			o_pFrame1 = m_FullBuffer[i_Frame1];
	}
	else
	{
		// Otherwise, look for frames in the couple of frames
		// we have in temporary memory buffer.
		bool bHaveFrame0 = false, bHaveFrame1 = false;

		// Look for frames in the data we have already loaded
		if (test_slot(0, i_Frame0, o_pFrame0))
			bHaveFrame0 = true;
		else if (test_slot(1, i_Frame0, o_pFrame0))
			bHaveFrame0 = true;

		// Look for the other frame also first before
		// loading so that we don't overwrite it if
		// we had it already.
		if (test_slot(0, i_Frame1, o_pFrame1))
			bHaveFrame1 = true;
		else if (test_slot(1, i_Frame1, o_pFrame1))
			bHaveFrame1 = true;

		// Load frames from disk that we did not already find
		if (!bHaveFrame0)
			load_frame(i_Frame0, o_pFrame0);
		if (!bHaveFrame1)
		{
			// Check to see if we just loaded frame1 also when we loaded frame0
			if (i_Frame1 == i_Frame0)
				o_pFrame1 = o_pFrame0;
			else
				load_frame(i_Frame1, o_pFrame1);
		}
	}
		
}

//--------------------------------------------------------------------
// Note: this is a call from the budget manager and signifies
//	that we have been moved from swapping to a fully loaded state.
// Load in all frames of animation from the file and close
// our file handle.
//--------------------------------------------------------------------
void vtxDynamicVertexSet::FullyLoadData()
{
	if (!m_bFullyLoaded)
	{
		envScopedLock read_lock(m_FileSeeker->GetMutex());

		DBG_ASSERT(m_FullBuffer.empty(), "Buffer should be fully empty before loading, no partial states yet");
		if (!m_FullBuffer.empty())
			return;

		const int num_frames = m_FilePositions.size();
		m_FullBuffer.resize( num_frames, NULL );
		for (int i=0; i<num_frames; i++)
		{
			m_FileSeeker->SetFilePos(m_FilePositions[i]);

			// Read the frame data
			std::auto_ptr<vtxVertexFrame> pVertexFrame(new vtxVertexFrame);
			vtxVertexAnimImport::ReadSingleVertexFrameData(m_FileSeeker->GetReader(), *pVertexFrame);
			m_FullBuffer[i] = pVertexFrame.release();
		}

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
void vtxDynamicVertexSet::FreeDataAndStartSwapping()
{
	if (m_bFullyLoaded)
	{
		// If we had a full buffer and are now switching to
		// paging, set up the pages to the correct size
		bool bNeedPageData = false;
		vtxVertexFrame *pFirstPage = NULL;
		if (m_Frame[0].m_Positions.empty() && !m_FullBuffer.empty())
		{
			// Steal the pointer to the first frame of 
			// data for our use in the pages.
			bNeedPageData = true;
			pFirstPage = m_FullBuffer[0];
			m_FullBuffer[0] = NULL;
		}

		m_bFullyLoaded = false;
		envSTLHelpers::DeleteContainer(m_FullBuffer);

		if (bNeedPageData)
		{
			// We are doing this after freeing the other frames because
			// this function might have been called when system memory is tight.
			// So it is better to free what memory we can first and then
			// allocate the pages.
			m_Frame[0] = (*pFirstPage);	
			m_Frame[1].m_Positions.reserve( m_Frame[0].m_Positions.size() );
			m_Frame[1].m_Normals.reserve( m_Frame[0].m_Normals.size() );
			delete pFirstPage;
			m_Loaded[0] = 0;
			m_LastUsed = 0;
		}

		// Attach to the file seeker in order to load frames as needed
		m_FileSeeker->Attach();
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool vtxDynamicVertexSet::test_slot(int i_Slot, int i_Frame, vtxVertexFrame* &o_pFrame)
{
	if (m_Loaded[i_Slot] == i_Frame)
	{
		o_pFrame = &m_Frame[i_Slot];
		m_LastUsed = i_Slot;
		return true;
	}
	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void vtxDynamicVertexSet::load_frame(int i_Frame, vtxVertexFrame* &o_pFrame)
{
	// Move to slot that was not used last
	m_LastUsed = (m_LastUsed+1) % 2; // (!m_LastUsed) ?

	// Load frame into this slot
	o_pFrame  = &m_Frame[m_LastUsed];
	m_Loaded[m_LastUsed] = i_Frame;

	// Lock the file seeker while we move the file position and read our data
	envScopedLock read_lock(m_FileSeeker->GetMutex());

	// Move the file position
	DBG_ASSERT(i_Frame<m_FilePositions.size(), "Frame index out of range");
	if (i_Frame<m_FilePositions.size())
		m_FileSeeker->SetFilePos(m_FilePositions[i_Frame]);

	// Read the frame data
	vtxVertexAnimImport::ReadSingleVertexFrameData(m_FileSeeker->GetReader(), *o_pFrame);
}

//--------------------------------------------------------------------
// Create a new set of vertex animation frames to management
//--------------------------------------------------------------------
shared_ptr<vtxDynamicVertexSet> vtxVertexFramesDynamic::CreateVertexSet(const fsLocator &i_Locator,
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
