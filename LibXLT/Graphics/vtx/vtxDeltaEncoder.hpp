/*****************************************************************************
**	vtxDeltaEncoder.hpp
**
**		Converts sequential frames of delta animation data into
**	a large encoded memory buffer ready for zip compression.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_DELTAENCODER_HPP
#error vtxDeltaEncoder.hpp multiply included
#endif
#define VTX_DELTAENCODER_HPP

#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif 

#include <vector>


//============================================================================
// Base class for encoders
//============================================================================
class vtxDeltaEncoder
{
public:
	//--------------------------------------------------------------------
	// Derived classes should give this class the size in bytes of 
	// a single vertex takes in the packed buffer.
	// HeaderSize sets an extra amount of memory to allocate per frame.
	//--------------------------------------------------------------------
	vtxDeltaEncoder(int i_VertexStride, int i_HeaderSize = 0);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~vtxDeltaEncoder();
	
	//--------------------------------------------------------------------
	// Returns true if the compression is lossy so that the compression
	// stream knows to doing extrapolations on the lossy data, not on 
	// the original data.
	//--------------------------------------------------------------------
	virtual bool IsLossyCompression() const = 0;

	//--------------------------------------------------------------------
	// Allocate memory for the buffer of encoded data based on the
	// given number of frame and vertices. This needs to be called
	// before encoding or decoding.
	//--------------------------------------------------------------------
	virtual void Allocate(int i_NumFrames, int i_NumVertices);
 
//------------------------------------------------------------------------
// Functions for encoding deltas into a memory block
//------------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Submit one frame of deltas at current pointer location in buffer.
	// Returns false if the deltas can not be encoded, which means write
	// full frame data.
	//--------------------------------------------------------------------
	virtual bool SubmitDeltas(const std::vector<maPoint3d> &i_DeltaPositions) = 0;

	//--------------------------------------------------------------------
	// Reset to start of buffer, ready to receive more frames
	//--------------------------------------------------------------------
	virtual void Reset();

	//--------------------------------------------------------------------
	// Return true if all deltas submitted so far are zeros.
	// This means the delta buffer doesn't need to be written.
	//--------------------------------------------------------------------
	virtual bool GetAllDeltasAreZero() const;

	//--------------------------------------------------------------------
	// Access to buffer for reading
	//--------------------------------------------------------------------
	virtual const void* GetBuffer() const;
	virtual int GetFilledBufferSize() const;

//------------------------------------------------------------------------
// Functions for decoding deltas from a memory block
//------------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Access to buffer for writing
	//--------------------------------------------------------------------
	virtual void* Buffer();
	virtual int GetFullBufferSize() const;

	//--------------------------------------------------------------------
	// After you put data into the buffer, call this function to
	// say how much data you have filled. This will be converted
	// into a number of frames.
	//--------------------------------------------------------------------
	virtual void SetFilledBufferSize(int i_Size);

	//--------------------------------------------------------------------
	// Get number of frames submitted based on a previous call to 
	// SetFilledBufferSize().
	//--------------------------------------------------------------------
	virtual int GetNumFramesSubmitted() const;

	//--------------------------------------------------------------------
	// Decode a section of the memory buffer into 
	//--------------------------------------------------------------------
	virtual bool ReceiveDeltas(int i_FrameIndex,
							   std::vector<maPoint3d> &o_DeltaPositions) const = 0;

protected:
	unsigned char *m_pDeltaBuffer;
	int m_NumFramesAllocated;
	int m_NumFramesSubmitted;
	int m_NumVertices;
	int m_Stride;
	int m_VertexStride;
	int m_HeaderSize;
	bool m_bAnyDeltas;
};
