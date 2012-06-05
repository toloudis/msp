/*****************************************************************************
**	smdlVertexAnimKeys.hpp
**
**		smdlVertexAnimKeys - animation information for vertex baked
**	animation. This file chooses between the static and dynamic variations
**	of vertex animation management.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_VERTEXANIMKEYS_HPP
#error smdlVertexAnimKeys.hpp multiply included
#endif
#define SMDL_VERTEXANIMKEYS_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef VTX_VERTEXANIMBUDGET_HPP
#include "Graphics/vtx/vtxVertexAnimBudget.hpp"
#endif 
#ifndef VTX_VERTEXANIMKEYS_HPP
#include "Graphics/vtx/vtxVertexAnimKeys.hpp"
#endif 


//============================================================================
// New version of these smdl typedefs refer to the base class
// through shared pointers...
//============================================================================
typedef shared_ptr<vtxVertexAnimKeys> smdlVertexAnimKeys;
typedef shared_ptr<vtxVertexFrames> smdlVertexFrames;

// This compiler define is just here so we could switch off the
// the vertex animation swapping quickly if needed.
//
//#define USE_DYNAMIC_VERTEX_ANIM
//
//#ifdef USE_DYNAMIC_VERTEX_ANIM
//
//	// Use vertex animation data that can be swapped in and out of memory
//	#ifndef VTX_VERTEXANIMKEYSDYNAMIC_HPP
//	#include "Graphics/vtx/vtxVertexAnimKeysDynamic.hpp"
//	#endif 
//
//	typedef shared_ptr<vtxVertexAnimKeysDynamic> smdlVertexAnimKeys;
//
//	//class vtxVertexFrames : public vtxVertexFramesDynamic {};
//	typedef vtxVertexFramesDynamic smdlVertexFrames;
//
//#else
//
//	// Use vertex animation data that is completely loaded into memory
//	#ifndef VTX_VERTEXANIMKEYSSTATIC_HPP
//	#include "Graphics/vtx/vtxVertexAnimKeysStatic.hpp"
//	#endif 
//
//
//	typedef shared_ptr<vtxVertexAnimKeysStatic> smdlVertexAnimKeys;
//
//	typedef vtxVertexFramesStatic smdlVertexFrames;
//
//
//#endif

