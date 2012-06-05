/*****************************************************************************
**	vtxVertexAnimKeys.hpp
**
**		vtxVertexAnimKeys - base class for vertex animations in
**	various paging and compressed formats.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_VERTEXANIMKEYS_HPP
#error vtxVertexAnimKeys.hpp multiply included
#endif
#define VTX_VERTEXANIMKEYS_HPP

#ifndef AN_KEYDATA_HPP
#include "Graphics/an/anKeyData.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif
#ifndef VTX_VERTEXFRAME_HPP
#include "Graphics/vtx/vtxVertexFrame.hpp"
#endif


//============================================================================
//============================================================================
class vtxVertexAnimKeys
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~vtxVertexAnimKeys() = 0  {} ;

	//--------------------------------------------------------------------
	//	HasAnimation - returns true if some channel has animation
	//--------------------------------------------------------------------
	virtual bool HasAnimation() const = 0;

	//--------------------------------------------------------------------
	//	GetAnimLength returns the time value for the last key
	//--------------------------------------------------------------------
	virtual float GetAnimLength() const = 0;

	//--------------------------------------------------------------------
	// Get number of vertices in a frame of animation data.
	// Used to make sure the vertex data is compatible with the
	// geometry.
	//--------------------------------------------------------------------
	virtual void GetNumVertices(int &o_NumVertices, int &o_NumNormals) = 0;

	//--------------------------------------------------------------------
	// GetBracketingFrames() - get vertex anim frame before and after 
	// given time. Also returns alpha value for frame between
	// the bracketing frames to be used for blending.
	//--------------------------------------------------------------------
	virtual void GetBracketingFrames(float i_Frame,
							 vtxVertexFrame* &o_pFrame0,
							 vtxVertexFrame* &o_pFrame1,
							 float &o_Alpha) const = 0;

	//--------------------------------------------------------------------
	// Bounding box animation
	//--------------------------------------------------------------------
	virtual bool	HasBBoxAnimation() const = 0;

	//--------------------------------------------------------------------
	// Get animated bounding box at given frame
	//--------------------------------------------------------------------
	virtual maAxisBox GetBBox(float i_Frame) = 0;

};


//============================================================================
// Base class for holding all the memory for the frames
// we have loaded.
//============================================================================
class vtxVertexFrames
{
public:
	virtual ~vtxVertexFrames() = 0  {};
};

