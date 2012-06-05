/*****************************************************************************
**	vtxDeltaLosslessEncoder.hpp
**
**		Converts sequential frames of delta animation data into
**	a large encoded memory buffer ready for zip compression.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_DELTALOSSLESSENCODER_HPP
#error vtxDeltaEncoder.hpp multiply included
#endif
#define VTX_DELTALOSSLESSENCODER_HPP

#ifndef VTX_DELTAENCODER_HPP
#include "Graphics/vtx/vtxDeltaEncoder.hpp"
#endif 


//============================================================================
// Encoder that keeps deltas as maPoint3d vectors so that they
// are deflated and inflated without loss.
//============================================================================
class vtxDeltaLosslessEncoder : public vtxDeltaEncoder
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	vtxDeltaLosslessEncoder();

	//--------------------------------------------------------------------
	// Returns true if the compression is lossy so that the compression
	// stream knows to doing extrapolations on the lossy data, not on 
	// the original data.
	//--------------------------------------------------------------------
	virtual bool IsLossyCompression() const { return false; }

	//--------------------------------------------------------------------
	// Submit one frame of deltas at current pointer location in buffer
	//--------------------------------------------------------------------
	bool SubmitDeltas(const std::vector<maPoint3d> &i_DeltaPositions);

	//--------------------------------------------------------------------
	// Decode a section of the memory buffer into 
	//--------------------------------------------------------------------
	bool ReceiveDeltas(int i_FrameIndex,
					   std::vector<maPoint3d> &o_DeltaPositions) const;
};

