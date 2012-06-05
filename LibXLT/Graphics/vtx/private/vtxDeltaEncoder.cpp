/*****************************************************************************
**	vtxDeltaEncoder.cpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxDeltaEncoder.hpp"

#include "Core/Ma/maConstants.hpp"


//--------------------------------------------------------------------
// Derived classes should give this class the size in bytes of 
// a single vertex takes in the packed buffer.
// HeaderSize sets an extra amount of memory to allocate per frame.
//--------------------------------------------------------------------
vtxDeltaEncoder::vtxDeltaEncoder(int i_VertexStride, int i_HeaderSize)
:	m_bAnyDeltas(false),
	m_pDeltaBuffer(NULL),
	m_NumFramesAllocated(0),
	m_NumFramesSubmitted(0),
	m_NumVertices(0),
	m_Stride(0),
	m_VertexStride(i_VertexStride),
	m_HeaderSize(i_HeaderSize)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
vtxDeltaEncoder::~vtxDeltaEncoder()
{
	if (m_pDeltaBuffer)
		delete [] m_pDeltaBuffer;
}

//--------------------------------------------------------------------
// Allocate memory for the buffer of encoded data based on the
// given number of frame and vertices.
//--------------------------------------------------------------------
void vtxDeltaEncoder::Allocate(int i_NumFrames, int i_NumVertices)
{
	m_NumFramesAllocated = i_NumFrames;
	m_NumVertices = i_NumVertices;
	m_NumFramesSubmitted = 0;
	m_Stride = (i_NumVertices * m_VertexStride) + m_HeaderSize; 

	if (m_pDeltaBuffer)
	{
		delete m_pDeltaBuffer;
		m_pDeltaBuffer = NULL;
	}
	if (i_NumFrames>0 || m_Stride>0)
		m_pDeltaBuffer = new unsigned char[m_Stride * i_NumFrames];
}

//--------------------------------------------------------------------
// Reset to start of buffer, ready to receive more frames
//--------------------------------------------------------------------
void vtxDeltaEncoder::Reset()
{
	m_NumFramesSubmitted = 0;
	m_bAnyDeltas = false;
}

//--------------------------------------------------------------------
// Return true if all deltas submitted so far are zeros.
// This means the delta buffer doesn't need to be written.
//--------------------------------------------------------------------
bool vtxDeltaEncoder::GetAllDeltasAreZero() const
{
	return (!m_bAnyDeltas);
}

//--------------------------------------------------------------------
// Access to buffer
//--------------------------------------------------------------------
const void* vtxDeltaEncoder::GetBuffer() const 
{ 
	DBG_ASSERT(m_pDeltaBuffer, "Need to call Allocate() before getting buffer");
	return m_pDeltaBuffer;
}
int vtxDeltaEncoder::GetFilledBufferSize() const 
{ 
	return m_NumFramesSubmitted * m_Stride;
}
//--------------------------------------------------------------------
// Access to buffer for writing
//--------------------------------------------------------------------
void* vtxDeltaEncoder::Buffer()
{
	DBG_ASSERT(m_pDeltaBuffer, "Need to call Allocate() before getting buffer");
	return m_pDeltaBuffer;
}
int vtxDeltaEncoder::GetFullBufferSize() const
{
	return m_NumFramesAllocated * m_Stride;
}

//--------------------------------------------------------------------
// After you put data into the buffer, call this function to
// say how much data you have filled. This will be converted
// into a number of frames.
//--------------------------------------------------------------------
void vtxDeltaEncoder::SetFilledBufferSize(int i_Size)
{
	// could assert a zero remainder here also?
	if (m_Stride > 0)
		m_NumFramesSubmitted = i_Size / m_Stride;
	else 
		m_NumFramesSubmitted = 0;
}

//--------------------------------------------------------------------
// Get number of frames submitted based on a previous call to 
// SetFilledBufferSize().
//--------------------------------------------------------------------
int vtxDeltaEncoder::GetNumFramesSubmitted() const
{
	return m_NumFramesSubmitted;
}

