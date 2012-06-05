/*****************************************************************************
**	vtxDeltaToleranceEncoder.hpp
**
**		Converts sequential frames of delta animation data into
**	a large encoded memory buffer ready for zip compression.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_DELTATOLERANCEENCODER_HPP
#error vtxDeltaEncoder.hpp multiply included
#endif
#define VTX_DELTATOLERANCEENCODER_HPP

#ifndef VTX_DELTAENCODER_HPP
#include "Graphics/vtx/vtxDeltaEncoder.hpp"
#endif 


//============================================================================
// Encoder that quantizes the delta vectors 
//============================================================================
class vtxDeltaToleranceEncoder : public vtxDeltaEncoder
{
public:
	//--------------------------------------------------------------------
	// Compresses deltas by quantizing them to a 16-bit code using the
	// given tolerance.
	//--------------------------------------------------------------------
	vtxDeltaToleranceEncoder(float i_Tolerance);

	//--------------------------------------------------------------------
	// Returns true if the compression is lossy so that the compression
	// stream knows to doing extrapolations on the lossy data, not on 
	// the original data.
	//--------------------------------------------------------------------
	virtual bool IsLossyCompression() const { return true; }

	//--------------------------------------------------------------------
	// Submit one frame of deltas at current pointer location in buffer.
	// Returns false if the deltas can not be encoded, which means write
	// full frame data.
	//--------------------------------------------------------------------
	bool SubmitDeltas(const std::vector<maPoint3d> &i_DeltaPositions);

	//--------------------------------------------------------------------
	// Decode a section of the memory buffer into 
	//--------------------------------------------------------------------
	bool ReceiveDeltas(int i_FrameIndex,
					   std::vector<maPoint3d> &o_DeltaPositions) const;

private:
	float m_Tolerance;
};
