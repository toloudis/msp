/*****************************************************************************
**	vtxVertexFramesCompressed.hpp
**
**		vtxVertexFramesCompressed - animation information for vertex baked
**	animation that is formed from reference frames and compressed delta blocks.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_VERTEXFRAMESCOMPRESSED_HPP
#error vtxVertexFramesCompressed.hpp multiply included
#endif
#define VTX_VERTEXFRAMESCOMPRESSED_HPP

#ifndef CH_DEFS_HPP
#include "Core/Ch/chDefs.hpp"
#endif 
#ifndef ENV_THREAD_HPP
#include "Core/env/envThread.hpp"
#endif
#ifndef FS_FILESTREAM_HPP
#include "Core/Fs/fsFileStream.hpp"
#endif 
#ifndef VTX_VERTEXANIMBUDGET_HPP
#include "Graphics/vtx/vtxVertexAnimBudget.hpp"
#endif 
#ifndef VTX_VERTEXANIMKEYS_HPP
#include "Graphics/vtx/vtxVertexAnimKeys.hpp"
#endif 
#ifndef VTX_VERTEXFRAME_HPP
#include "Graphics/vtx/vtxVertexFrame.hpp"
#endif 


//============================================================================
//============================================================================
class fsLocator;
class vtxDeltaEncoder;
class vtxDynamicVertexSet;
class vtxVertexFileSeeker;


//============================================================================
// Tracks a set of compressed delta information between two reference frames
//============================================================================
class vtxCompressedDeltasSet : public vtxVertexAnimBudgetItem
{
public:
	//--------------------------------------------------------------------
	// Constructor needs to know the maximum number of frames in a
	// delta block in order to allocate the temporary buffer.
	//--------------------------------------------------------------------
	vtxCompressedDeltasSet(shared_ptr<vtxDynamicVertexSet>& i_ReferenceFrames,
						   shared_ptr<vtxDeltaEncoder>& i_Encoder,
						   shared_ptr<vtxDeltaEncoder>& i_NormalEncoder,
						   int i_MaxNumFramesPerBlock,
						   int i_NumVertices, 
						   int i_NumNormals,
						   shared_ptr<vtxVertexFileSeeker> i_FileSeeker);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~vtxCompressedDeltasSet();

	//--------------------------------------------------------------------
	// Compute memory needs for the delta blocks and add this
	// item to the vertex animation budgeting based on that size.
	// This may cause all data to be loaded.
	//--------------------------------------------------------------------
	void SubmitForBudgeting();

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
	// This class tracks file positions for each set of deltas data.
	// This functions adds the file position for the next frame of data.
	// i_ReferenceFrameIndex sets the index into the vtxDynamicVertexSet
	// to find the reference frame that these deltas work from.
	// Returns index to use as the i_DeltaBlockIndex in a 
	// later call to GetDeltas.
	//--------------------------------------------------------------------
	int AddFilePos(fsFileStream::FilePosType i_DeltasFilePos,
				   fsFileStream::FilePosType i_NormalsFilePos,
				   int i_ReferenceFrameIndex);

	//--------------------------------------------------------------------
	// Get animation delta data, loading from disk if necessary.
	// It is only possible to ask for two frames from the same delta 
	// block. If you are only interested in one frame, then set
	// i_Frame0=i_Frame1
	//--------------------------------------------------------------------
	void GetDeltas(int i_DeltaBlockIndex, 
				   int i_Frame0, 
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
	// 	typedef for a delta block
	//--------------------------------------------------------------------
	typedef std::vector<vtxVertexFrame> DeltaBlock;

	//--------------------------------------------------------------------
	// private functions
	//--------------------------------------------------------------------
	void allocate_temp_block();
	void load_deltas(int i_DeltaBlockIndex, DeltaBlock &o_DeltaBlock);

	vtxVertexAnimBudgetItem::BudgetSize m_Size;
	bool m_bBudgeting;
	shared_ptr<vtxVertexFileSeeker> m_FileSeeker;
	std::vector<fsFileStream::FilePosType> m_DeltasFilePositions;
	std::vector<fsFileStream::FilePosType> m_NormalsFilePositions;
	std::vector<int> m_ReferenceFrameIndices;
	shared_ptr<vtxDynamicVertexSet> m_ReferenceFrames;
	shared_ptr<vtxDeltaEncoder> m_Encoder;
	shared_ptr<vtxDeltaEncoder> m_NormalEncoder;

	int m_MaxNumFramesPerBlock;
	int m_NumVertices, m_NumNormals;
	// Buffer for loading temporary frames just in time
	DeltaBlock m_TempBlock;
	int m_LastLoaded;

	// Buffer for when the data is fully loaded
	bool m_bFullyLoaded;
	std::vector<DeltaBlock*> m_FullBuffer;
};


//============================================================================
// Tracks all of the animation sets within a given file.
//============================================================================
class vtxVertexFramesCompressed : public vtxVertexFrames
{
public:
	//--------------------------------------------------------------------
	// shared pointer handles cleanup, but need to implement 
	// virtual destructor
	//--------------------------------------------------------------------
	~vtxVertexFramesCompressed() {}

	//--------------------------------------------------------------------
	// Create a new set of reference vertex animation frames to management
	//--------------------------------------------------------------------
	shared_ptr<vtxDynamicVertexSet> CreateVertexSet(const fsLocator& i_Locator,
													 chDefs::Size i_Size);

	//--------------------------------------------------------------------
	// Create a new set of compressed deltas blocks to management
	//--------------------------------------------------------------------
	shared_ptr<vtxCompressedDeltasSet> CreateDeltasSet(const fsLocator& i_Locator,
													   shared_ptr<vtxDynamicVertexSet>& i_ReferenceFrames,
													   shared_ptr<vtxDeltaEncoder>& i_Encoder,
													   shared_ptr<vtxDeltaEncoder>& i_NormalEncoder,
													   int i_MaxNumFramesPerBlock,
													   int i_NumVertices, 
													   int i_NumNormals);

private:
	shared_ptr<vtxVertexFileSeeker> m_FileSeeker;
	std::vector< shared_ptr<vtxDynamicVertexSet> > m_VertexSets;
	std::vector< shared_ptr<vtxCompressedDeltasSet> > m_DeltasSets;
};


