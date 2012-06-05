/*****************************************************************************
**	vtxDeltaLosslessEncoder.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxDeltaLosslessEncoder.hpp"

#include "Core/Ma/maConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
vtxDeltaLosslessEncoder::vtxDeltaLosslessEncoder()
:	vtxDeltaEncoder(sizeof(maPoint3d))	// CodeType == maPoint3d, no encoding
{

}

//--------------------------------------------------------------------
// Submit one frame of deltas at current pointer location in buffer
//--------------------------------------------------------------------
bool vtxDeltaLosslessEncoder::SubmitDeltas(const std::vector<maPoint3d> &i_DeltaPositions)
{
	DBG_ASSERT(m_pDeltaBuffer, "Need to call Allocate() before submitting deltas");
	if (!m_pDeltaBuffer)
		return false;

	// See if we are getting any real deltas, (this could be faster)
	if (!m_bAnyDeltas)
	{
		std::vector<maPoint3d>::const_iterator it;
		for (it = i_DeltaPositions.begin(); it != i_DeltaPositions.end(); ++it)
		{
			if (it->LengthSqr() > maConstants::c_fEpsilon)
			{
				m_bAnyDeltas = true;
				break;
			}
		}
	}

	// no encodings here, just submit the vectors directly
	DBG_ASSERT(i_DeltaPositions.size() == m_NumVertices, "Wrong number of vertices submitted to delta code buffer");
	if (i_DeltaPositions.size() != m_NumVertices)
		return false;
	DBG_ASSERT(m_NumFramesSubmitted<m_NumFramesAllocated, "Need to call Reset() after filling the buffer in order to submit more deltas");
	if (m_NumFramesSubmitted>=m_NumFramesAllocated)
		return false;
	unsigned char *pCurPtr = m_pDeltaBuffer + (m_NumFramesSubmitted * m_Stride);
	::memcpy(pCurPtr, &(i_DeltaPositions[0]), m_Stride);
	m_NumFramesSubmitted++;

	return true;
}

//--------------------------------------------------------------------
// Decode a section of the memory buffer into 
//--------------------------------------------------------------------
bool vtxDeltaLosslessEncoder::ReceiveDeltas(int i_FrameIndex,
											std::vector<maPoint3d> &o_DeltaPositions) const
{
	DBG_ASSERT(i_FrameIndex<m_NumFramesSubmitted, "Frame index out of range");	
	if (i_FrameIndex >= m_NumFramesSubmitted)
		return false;

	o_DeltaPositions.resize(m_NumVertices);
	const unsigned char *pCurPtr = m_pDeltaBuffer + (i_FrameIndex * m_Stride);
	::memcpy(&(o_DeltaPositions[0]), pCurPtr, m_Stride);
	return true;
}

