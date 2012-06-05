/*****************************************************************************
**	vtxVertexFramesDynamic.hpp
**
**		vtxVertexFramesDynamic - animation information for vertex baked
**	animation that is being swapped in and out of memory.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_VERTEXFRAMESDYNAMIC_HPP
#error vtxVertexFramesDynamic.hpp multiply included
#endif
#define VTX_VERTEXFRAMESDYNAMIC_HPP

#ifndef CH_DEFS_HPP
#include "Core/Ch/chDefs.hpp"
#endif 
#ifndef ENV_THREAD_HPP
#include "Core/env/envThread.hpp"
#endif
#ifndef FS_FILESTREAM_HPP
#include "Core/Fs/fsFileStream.hpp"
#endif 
#ifndef VTX_VERTEXANIMKEYS_HPP
#include "Graphics/vtx/vtxVertexAnimKeys.hpp"
#endif 
#ifndef VTX_VERTEXANIMBUDGET_HPP
#include "Graphics/vtx/vtxVertexAnimBudget.hpp"
#endif 
#ifndef VTX_VERTEXFRAME_HPP
#include "Graphics/vtx/vtxVertexFrame.hpp"
#endif 


//============================================================================
//============================================================================
class fsLocator;
class vtxVertexFileSeeker;


//============================================================================
// Tracks a set of animation frames for a single surface - may not all
// be in memory at any given moment.
//============================================================================
class vtxDynamicVertexSet : public vtxVertexAnimBudgetItem
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	vtxDynamicVertexSet(shared_ptr<vtxVertexFileSeeker> i_FileSeeker,
						 chDefs::Size i_Size);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~vtxDynamicVertexSet();

	//--------------------------------------------------------------------
	// Compute memory needs for the delta blocks and add this
	// item to the vertex animation budgeting based on that size.
	// This may cause all data to be loaded.
	//--------------------------------------------------------------------
	void SubmitForBudgeting(int i_NumVertices, int i_NumNormals);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool IsFullyLoaded() const;

	//--------------------------------------------------------------------
	// Get number of vertices in a frame of animation data.
	// Used to make sure the vertex data is compatible with the
	// geometry.
	//--------------------------------------------------------------------
	void GetNumVertices(int &o_NumVertices, int &o_NumNormals);

	//--------------------------------------------------------------------
	// Adds vertex frame - only used to fill the full buffer
	// on initial file load when IsFullyLoaded() is true
	//--------------------------------------------------------------------
	void AddVertexFrame(vtxVertexFrame* i_pFrame);

	//--------------------------------------------------------------------
	// Return pointer to buffer for holding first frame of animation
	// data. Even when paging anim data, the first frame should be
	// loaded in order to know how many vertices and normals  
	// will be in each frame of data.
	//--------------------------------------------------------------------
	vtxVertexFrame* FirstFrame();

	//--------------------------------------------------------------------
	// This should be called after the first frame of data is loaded
	// using a call to "FirstFrame()". This will make sure enough
	// memory is allocated in the second frame so that we can page
	// vertex animation memory without needing to allocate the second
	// frame later. 
	//--------------------------------------------------------------------
	void AllocateSecondFrame();

	//--------------------------------------------------------------------
	// This class tracks file positions for each frame of vertex data.
	// This functions adds the file position for the next frame of data.
	//--------------------------------------------------------------------
	int AddFilePos(fsFileStream::FilePosType i_FilePos);

	//--------------------------------------------------------------------
	// Get vertex frame data, loading from disk if necessary.
	//--------------------------------------------------------------------
	void GetVertexFrames(int i_Frame0, 
						 int i_Frame1, 
						 vtxVertexFrame* &o_pFrame0, 
						 vtxVertexFrame* &o_pFrame1);

	//--------------------------------------------------------------------
	// vtxVertexAnimBudgetItem virtual overrides
	//--------------------------------------------------------------------
	virtual vtxVertexAnimBudgetItem::BudgetSize GetSize() const { return m_Size; }
	virtual void FullyLoadData();
	virtual void FreeDataAndStartSwapping();

private:
	//--------------------------------------------------------------------
	// private functions
	//--------------------------------------------------------------------
	bool test_slot(int i_Slot, int i_Frame, vtxVertexFrame* &o_pFrame);
	void load_frame(int i_Frame, vtxVertexFrame* &o_pFrame);

	vtxVertexAnimBudgetItem::BudgetSize m_Size;
	bool m_bBudgeting;
	shared_ptr<vtxVertexFileSeeker> m_FileSeeker;
	std::vector<fsFileStream::FilePosType> m_FilePositions;

	// Buffer for loading temporary frames just in time
	int m_Loaded[2];
	vtxVertexFrame m_Frame[2];
	int m_LastUsed;

	// Buffer for when the data is fully loaded
	bool m_bFullyLoaded;
	std::vector<vtxVertexFrame*> m_FullBuffer;
};


//============================================================================
// Tracks all of the animation sets within a given file.
//============================================================================
class vtxVertexFramesDynamic : public vtxVertexFrames
{
public:
	//--------------------------------------------------------------------
	// shared pointer handles cleanup, but need to implement 
	// virtual destructor
	//--------------------------------------------------------------------
	~vtxVertexFramesDynamic() {}

	//--------------------------------------------------------------------
	// Create a new set of vertex animation frames to management
	//--------------------------------------------------------------------
	shared_ptr<vtxDynamicVertexSet> CreateVertexSet(const fsLocator& i_Locator,
													 chDefs::Size i_Size);

private:
	shared_ptr<vtxVertexFileSeeker> m_FileSeeker;
	std::vector< shared_ptr<vtxDynamicVertexSet> > m_VertexSets;
};

