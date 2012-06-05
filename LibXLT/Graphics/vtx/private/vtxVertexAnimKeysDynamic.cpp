/*****************************************************************************
**	vtxVertexAnimKeysDynamic.cpp
**
**		vtxVertexAnimKeysDynamic - animation information for vertex baked
**	animation.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxVertexAnimKeysDynamic.hpp"


//============================================================================
//============================================================================
namespace
{
	const float c_FrameClamp = 1.0f / 6000.0f;
}


//--------------------------------------------------------------------
//	Default constructor.
//--------------------------------------------------------------------
vtxVertexAnimKeysDynamic::vtxVertexAnimKeysDynamic(const shared_ptr<vtxDynamicVertexSet>& i_VertexSet)
:	m_VertexSet(i_VertexSet),
	m_Frames(0)
{
}

//--------------------------------------------------------------------
//	GetAnimLength returns the time value for the last key
//--------------------------------------------------------------------
float vtxVertexAnimKeysDynamic::GetAnimLength() const
{
	return m_Frames.GetLength();
}

//--------------------------------------------------------------------
// Get number of vertices in a frame of animation data.
// Used to make sure the vertex data is compatible with the
// geometry.
//--------------------------------------------------------------------
void vtxVertexAnimKeysDynamic::GetNumVertices(int &o_NumVertices, int &o_NumNormals)
{
	m_VertexSet->GetNumVertices(o_NumVertices, o_NumNormals);
}

//--------------------------------------------------------------------
//	Mutators.  The vtxVertexAnimKeysDynamic makes a copy of the anKeyData
//	part of the animation, but does not own the vtxVertexFrame data
//	being pointed at. This allows sharing of vertex data between
//	animations.
//--------------------------------------------------------------------
//void vtxVertexAnimKeysDynamic::SetFrames(anKeyDataBase<int>& i_Anim)
//{
//	m_Frames = i_Anim;
//}

//--------------------------------------------------------------------
// GetBracketingFrames() - get vertex anim frame before and after 
// given time. Also returns alpha value for frame between
// the bracketing frames to be used for blending.
//--------------------------------------------------------------------
void vtxVertexAnimKeysDynamic::GetBracketingFrames(float i_Frame,
													vtxVertexFrame* &o_pFrame0,
													vtxVertexFrame* &o_pFrame1,
													float &o_Alpha) const
{

	// Get frames to interpolate between. Should we clamp to one frame or the other?
	int key0, key1;
	m_Frames.GetBracketingKeyData(i_Frame, key0, key1);

	float time0 = 0, time1 = 0;
	int frame0 = 0, frame1 = 0;
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

	// This call to m_VertexSet may cause a read from disk
	o_pFrame0 = o_pFrame1  = NULL;
	m_VertexSet->GetVertexFrames(frame0, frame1, o_pFrame0, o_pFrame1);

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
maAxisBox vtxVertexAnimKeysDynamic::GetBBox(float i_Frame)
{	
	int key1, key2;
	m_BBoxAnim.GetBracketingKeyData(i_Frame, key1, key2);

	float t1 = 0, t2 = 0;
	maAxisBox v1, v2;
	m_BBoxAnim.GetKeyData(key1, t1, v1);
	m_BBoxAnim.GetKeyData(key2, t2, v2);	

	if ( t1 == t2 )
		return v1;

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

