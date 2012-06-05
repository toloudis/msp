/*****************************************************************************
**	vtxVertexAnimKeysCompressed.cpp
**
**		vtxVertexAnimKeysCompressed - animation information for vertex baked
**	animation.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxVertexAnimKeysCompressed.hpp"

#include "Graphics/vtx/vtxVertexFramesDynamic.hpp"


//============================================================================
//============================================================================
namespace
{
	const float c_FrameClamp = 1.0f / 6000.0f;
}

//--------------------------------------------------------------------
//	Default constructor.
//--------------------------------------------------------------------
vtxVertexAnimKeysCompressed::vtxVertexAnimKeysCompressed(const shared_ptr<vtxDynamicVertexSet> i_VertexSet,
														 const shared_ptr<vtxCompressedDeltasSet> i_DeltasSet)
:	m_VertexSet(i_VertexSet),
	m_DeltasSet(i_DeltasSet),
	m_Frames(sFrameData())
{
}

//--------------------------------------------------------------------
//	GetAnimLength returns the time value for the last key
//--------------------------------------------------------------------
float vtxVertexAnimKeysCompressed::GetAnimLength() const
{
	return m_Frames.GetLength();
}

//--------------------------------------------------------------------
// Get number of vertices in a frame of animation data.
// Used to make sure the vertex data is compatible with the
// geometry.
//--------------------------------------------------------------------
void vtxVertexAnimKeysCompressed::GetNumVertices(int &o_NumVertices, int &o_NumNormals)
{
	m_DeltasSet->GetNumVertices(o_NumVertices, o_NumNormals);
}

//--------------------------------------------------------------------
// GetBracketingFrames() - get vertex anim frame before and after 
// given time. Also returns alpha value for frame between
// the bracketing frames to be used for blending.
//--------------------------------------------------------------------
void vtxVertexAnimKeysCompressed::GetBracketingFrames(float i_Frame,
													vtxVertexFrame* &o_pFrame0,
													vtxVertexFrame* &o_pFrame1,
													float &o_Alpha) const
{

	// Get frames to interpolate between. Should we clamp to one frame or the other?
	int key0, key1;
	m_Frames.GetBracketingKeyData(i_Frame, key0, key1);

	float time0 = 0, time1 = 0;
	sFrameData frame0, frame1;
	m_Frames.GetKeyData(key0, time0, frame0);
	m_Frames.GetKeyData(key1, time1, frame1);	
	
	float alpha = 0.0f;
	if (time1 != time0)
		alpha = (i_Frame - time0) / (time1 - time0);

	// See if we can clamp to exactly one frame or the other
	if (alpha < c_FrameClamp)
		frame1 = frame0; // fetch only frame 0
	else if (alpha > 1.0f - c_FrameClamp)
		frame0 = frame1; // fetch only frame 0

	// If frame0 is static, then don't blend between it and the next frame
	if (frame0.m_bStatic)
		frame1 = frame0;

	bool bFrame0HasDeltas = (frame0.m_DeltaBlock >= 0);
	bool bFrame1HasDeltas = (frame1.m_DeltaBlock >= 0);

	// Handle multiple blending cases based on whether the neighboring frames
	// use delta blocks or not...
	o_pFrame0 = o_pFrame1  = NULL;
	if (bFrame0HasDeltas && bFrame1HasDeltas)
	{
		// You can't have two neghboring frames with different delta blocks, there
		// had to have been a reference frame between. Check for this and then fallback on
		// just frame0 if that is the case
		if (frame1.m_DeltaBlock != frame0.m_DeltaBlock)
			frame1 = frame0;

		// Fetch both frames from the same delta block, this call may cause a read from disk
		m_DeltasSet->GetDeltas(frame0.m_DeltaBlock, frame0.m_DeltaOffset, frame1.m_DeltaOffset,  o_pFrame0, o_pFrame1);
	}
	else if (!bFrame0HasDeltas && !bFrame1HasDeltas)
	{
		// Both are using reference frames, this call may cause a read from disk
		m_VertexSet->GetVertexFrames(frame0.m_ReferenceFrame, frame1.m_ReferenceFrame, o_pFrame0, o_pFrame1);
	}
	else if (bFrame1HasDeltas)
	{
		// Blending between a reference frame in frame0 and a delta block in frame1
		vtxVertexFrame* duplicate_frame = NULL;
		m_VertexSet->GetVertexFrames(frame0.m_ReferenceFrame, frame0.m_ReferenceFrame, o_pFrame0, duplicate_frame);
		m_DeltasSet->GetDeltas(frame1.m_DeltaBlock, frame1.m_DeltaOffset, frame1.m_DeltaOffset,  o_pFrame1, duplicate_frame);
	}
	else if (bFrame0HasDeltas)
	{
		// Blending between a delta block in frame0 and a reference frame in frame1
		vtxVertexFrame* duplicate_frame = NULL;
		m_DeltasSet->GetDeltas(frame0.m_DeltaBlock, frame0.m_DeltaOffset, frame0.m_DeltaOffset,  o_pFrame0, duplicate_frame);
		m_VertexSet->GetVertexFrames(frame1.m_ReferenceFrame, frame1.m_ReferenceFrame, o_pFrame1, duplicate_frame);
	}

	// Coordinate the pointers so that we know that pFrame0 is valid and 
	// pFrame1 is non-NULL only if different than pFrame0
	if (!o_pFrame0) 
		o_pFrame0 = o_pFrame1;
	if (o_pFrame0 == o_pFrame1)
		o_pFrame1 = NULL;

	// If we only have one frame, then alpha needs to be 0.0
	o_Alpha = (o_pFrame1 == NULL) ? 0.0f : alpha;

}

//--------------------------------------------------------------------
// Get animated bounding box at given frame
//--------------------------------------------------------------------
maAxisBox vtxVertexAnimKeysCompressed::GetBBox(float i_Frame)
{	
	int key1, key2;
	m_Frames.GetBracketingKeyData(i_Frame, key1, key2);

	float t1 = 0, t2 = 0;
	sFrameData frame0, frame1;
	m_Frames.GetKeyData(key1, t1, frame0);
	m_Frames.GetKeyData(key2, t2, frame1);	

	if( t1 == t2 )
		return frame0.m_BBox;

	const maAxisBox &v1 = frame0.m_BBox;
	const maAxisBox &v2 = frame1.m_BBox;

	// do a linear interpolation
	float alpha = (i_Frame - t1) / (t2 - t1);

	maVector3d v1_min(v1.GetMinX(), v1.GetMinY(), v1.GetMinZ());
	maVector3d v1_max(v1.GetMaxX(), v1.GetMaxY(), v1.GetMaxZ());
	maVector3d v2_min(v2.GetMinX(), v2.GetMinY(), v2.GetMinZ());
	maVector3d v2_max(v2.GetMaxX(), v2.GetMaxY(), v2.GetMaxZ());

	maVector3d min_pt( v1_min * (1 - alpha) + v2_min * alpha );
	maVector3d max_pt( v1_max * (1 - alpha) + v2_max * alpha );

	return maAxisBox(min_pt, max_pt);
}

