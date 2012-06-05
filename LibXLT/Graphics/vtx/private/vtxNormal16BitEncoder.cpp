/*****************************************************************************
**	vtxNormal16BitEncoder.cpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxNormal16BitEncoder.hpp"

#include "Core/Ma/maConstants.hpp"
#include "Graphics/vtx/private/vtxDeltaCompression.hpp"


//============================================================================
//============================================================================
namespace
{
	// Each buffer is the encoded data plus 4 floats for the tolerance and bbox offset,
	// each position is encoded as one 32-bit code (X,Y,Z) [2 * 16-bit]
	int c_VertexStride = 2 * sizeof(vtxDeltaCompression::CodeType);
	int c_HeaderSize = 4 * sizeof(envType::Float32);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
vtxNormal16BitEncoder::vtxNormal16BitEncoder()
:	vtxDeltaEncoder(c_VertexStride, c_HeaderSize)
{
}

//--------------------------------------------------------------------
// Submit one frame of deltas at current pointer location in buffer
//--------------------------------------------------------------------
bool vtxNormal16BitEncoder::SubmitDeltas(const std::vector<maPoint3d> &i_DeltaPositions)
{
	DBG_ASSERT(m_pDeltaBuffer, "Need to call Allocate() before submitting deltas");
	if (!m_pDeltaBuffer)
		return false;

	// See if we are getting any real deltas, (this could be faster)
	std::vector<maPoint3d>::const_iterator it;
	if (!m_bAnyDeltas)
	{
		for (it = i_DeltaPositions.begin(); it != i_DeltaPositions.end(); ++it)
		{
			// normals have deltas only in X and Y components,
			// Z value is sign.
			if ((it->m_X*it->m_X + it->m_Y*it->m_Y) > maConstants::c_fEpsilon)
			{
				m_bAnyDeltas = true;
				break;
			}
		}
	}

	DBG_ASSERT(i_DeltaPositions.size() == m_NumVertices, "Wrong number of vertices submitted to delta code buffer");
	if (i_DeltaPositions.size() != m_NumVertices)
		return false;
	DBG_ASSERT(m_NumFramesSubmitted<m_NumFramesAllocated, "Need to call Reset() after filling the buffer in order to submit more deltas");
	if (m_NumFramesSubmitted >= m_NumFramesAllocated)
		return false;
	unsigned char *pTopPtr = m_pDeltaBuffer + (m_NumFramesSubmitted * m_Stride);
	unsigned char *pCurPtr = pTopPtr + c_HeaderSize; // skip over tolerance and bbox offset

	maVector3d min_bounds;
	float quant = 0;
	vtxDeltaCompression::CodeType *pCodePtr = reinterpret_cast<vtxDeltaCompression::CodeType*>(pCurPtr);
	vtxDeltaCompression::EncodeNormalVectors(i_DeltaPositions, pCodePtr, min_bounds, quant);

	// Pack the header data
	envType::Float32 *pFloats = reinterpret_cast<envType::Float32*>(pTopPtr);
	pFloats[0] = quant;
	pFloats[1] = min_bounds.GetX();
	pFloats[2] = min_bounds.GetY();
	pFloats[3] = min_bounds.GetZ();
	m_NumFramesSubmitted++;
	return true;
}

//--------------------------------------------------------------------
// Decode a section of the memory buffer into 
//--------------------------------------------------------------------
bool vtxNormal16BitEncoder::ReceiveDeltas(int i_FrameIndex,
											std::vector<maPoint3d> &o_DeltaPositions) const
{

	DBG_ASSERT(i_FrameIndex<m_NumFramesSubmitted, "Frame index out of range");	
	if (i_FrameIndex >= m_NumFramesSubmitted)
		return false;
	o_DeltaPositions.resize(m_NumVertices);

	const unsigned char *pTopPtr = m_pDeltaBuffer + (i_FrameIndex * m_Stride);
	const unsigned char *pCurPtr = pTopPtr + c_HeaderSize; // skip over tolerance and bbox offset
	
	const envType::Float32 *pFloats = reinterpret_cast<const envType::Float32*>(pTopPtr);
	float quant = pFloats[0];
	maVector3d min_bounds( pFloats[1], pFloats[2], pFloats[3] );
	const vtxDeltaCompression::CodeType *pCodePtr = 
		reinterpret_cast<const vtxDeltaCompression::CodeType*>(pCurPtr);

	vtxDeltaCompression::DecodeNormalVectors(pCodePtr, quant, min_bounds, o_DeltaPositions);
	return true;
}
