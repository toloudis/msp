/*****************************************************************************
**	vtxVertexAnimKeysStatic.cpp
**
**		vtxVertexAnimKeysStatic - animation information for vertex baked
**	animation.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxVertexAnimKeysStatic.hpp"

#include "Core/Env/envSTLHelpers.hpp"


//--------------------------------------------------------------------
//	Default constructor.
//--------------------------------------------------------------------
vtxVertexAnimKeysStatic::vtxVertexAnimKeysStatic()
{
}

//--------------------------------------------------------------------
//	GetAnimLength returns the time value for the last key
//--------------------------------------------------------------------
float vtxVertexAnimKeysStatic::GetAnimLength() const
{
	return m_Frames.GetLength();
}

//--------------------------------------------------------------------
// Get number of vertices in a frame of animation data.
// Used to make sure the vertex data is compatible with the
// geometry.
//--------------------------------------------------------------------
void vtxVertexAnimKeysStatic::GetNumVertices(int &o_NumVertices, int &o_NumNormals)
{
	o_NumVertices = o_NumNormals = 0;

	// Check all frames, or just the first frame?
	const int num_frames = m_Frames.GetNumKeys();
	float time = 0;
	vtxVertexFrame *pFrame = NULL;
	for (int f=0; f<num_frames; ++f)
	{
		m_Frames.GetKeyData(f, time, pFrame);
		if (pFrame)
		{
			o_NumVertices = pFrame->m_Positions.size();
			o_NumNormals = pFrame->m_Normals.size();
			return;
		}
	}
}

//--------------------------------------------------------------------
//	Mutators.  The vtxVertexAnimKeysStatic makes a copy of the anKeyData
//	part of the animation, but does not own the vtxVertexFrame data
//	being pointed at. This allows sharing of vertex data between
//	animations.
//--------------------------------------------------------------------
//void vtxVertexAnimKeysStatic::SetFrames(anKeyDataBase<vtxVertexFrame*>& i_Anim)
//{
//	m_Frames = i_Anim;
//}

//--------------------------------------------------------------------
// GetBracketingFrames() - get vertex anim frame before and after 
// given time. Also returns alpha value for frame between
// the bracketing frames to be used for blending.
//--------------------------------------------------------------------
void vtxVertexAnimKeysStatic::GetBracketingFrames(float i_Frame,
													vtxVertexFrame* &o_pFrame0,
													vtxVertexFrame* &o_pFrame1,
													float &o_Alpha) const
{
	// Get frames to interpolate between. Should we clamp to one frame or the other?
	int key0, key1;
	m_Frames.GetBracketingKeyData(i_Frame, key0, key1);
	float time0 = 0, time1 = 0;
	o_pFrame0 = o_pFrame1  = NULL;
	m_Frames.GetKeyData(key0, time0, o_pFrame0);
	m_Frames.GetKeyData(key1, time1, o_pFrame1);	
	
	// Coordinate the pointers so that we know that pFrame0 is valid and 
	// pFrame1 is non-NULL only if different than pFrame0
	if (!o_pFrame0) 
		o_pFrame0 = o_pFrame1;
	if (o_pFrame0 == o_pFrame1)
		o_pFrame1 = NULL;

	o_Alpha = 0.0f;
	if (o_pFrame1 != NULL && time1 != time0)
		o_Alpha = (i_Frame - time0) / (time1 - time0);

}

//--------------------------------------------------------------------
// Get animated bounding box at given frame
//--------------------------------------------------------------------
maAxisBox vtxVertexAnimKeysStatic::GetBBox(float i_Frame)
{
	DBG_ASSERT(false, "BBOX animation to implemented in vertex anim static keys.");
	return maAxisBox();
}

//--------------------------------------------------------------------
// Delete the vertex frames in the container as well
//--------------------------------------------------------------------

vtxVertexFramesStatic::~vtxVertexFramesStatic()
{
	envSTLHelpers::DeleteContainer(m_VertexFrames);
}


//--------------------------------------------------------------------
// Add the vertex frame to the container
//--------------------------------------------------------------------
void  vtxVertexFramesStatic::AddVertexFrame(vtxVertexFrame* i_pFrame)
{
	m_VertexFrames.push_back(i_pFrame);
}

