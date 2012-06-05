/****************************************************************************\
**  g2dResourceCounter.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g2d/g2dResourceCounter.hpp"

#include "Core/dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
g2dResourceCounterImpl* g2dResourceCounter::sm_pImplementation = NULL;


//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounter::GetTotalVideoMemory()
{
	DBG_ASSERT(g2dResourceCounter::sm_pImplementation, "g2dResourceCounter: No implementation");
	if (!g2dResourceCounter::sm_pImplementation)
		return 0.0f;
	return sm_pImplementation->GetTotalVideoMemory();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounter::GetTotalSceneTextureMemory()
{
	DBG_ASSERT(g2dResourceCounter::sm_pImplementation, "g2dResourceCounter: No implementation");
	if (!g2dResourceCounter::sm_pImplementation)
		return 0.0f;
	return sm_pImplementation->GetTotalSceneTextureMemory();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounter::GetNumSceneTextures()
{
	DBG_ASSERT(g2dResourceCounter::sm_pImplementation, "g2dResourceCounter: No implementation");
	if (!g2dResourceCounter::sm_pImplementation)
		return 0;
	return sm_pImplementation->GetNumSceneTextures();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounter::GetTotalFramebufferMemory()
{
	DBG_ASSERT(g2dResourceCounter::sm_pImplementation, "g2dResourceCounter: No implementation");
	if (!g2dResourceCounter::sm_pImplementation)
		return 0.0f;
	return sm_pImplementation->GetTotalFramebufferMemory();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounter::GetNumFramebufferTextures()
{
	DBG_ASSERT(g2dResourceCounter::sm_pImplementation, "g2dResourceCounter: No implementation");
	if (!g2dResourceCounter::sm_pImplementation)
		return 0;
	return sm_pImplementation->GetNumFramebufferTextures();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounter::GetTotalShadowMapMemory()
{
	DBG_ASSERT(g2dResourceCounter::sm_pImplementation, "g2dResourceCounter: No implementation");
	if (!g2dResourceCounter::sm_pImplementation)
		return 0.0f;
	return sm_pImplementation->GetTotalShadowMapMemory();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounter::GetNumShadowMapTextures()
{
	DBG_ASSERT(g2dResourceCounter::sm_pImplementation, "g2dResourceCounter: No implementation");
	if (!g2dResourceCounter::sm_pImplementation)
		return 0;
	return sm_pImplementation->GetNumShadowMapTextures();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounter::GetTotalVertexBufferMemory()
{
	DBG_ASSERT(g2dResourceCounter::sm_pImplementation, "g2dResourceCounter: No implementation");
	if (!g2dResourceCounter::sm_pImplementation)
		return 0.0f;
	return sm_pImplementation->GetTotalVertexBufferMemory();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounter::GetNumVertexBuffers()
{
	DBG_ASSERT(g2dResourceCounter::sm_pImplementation, "g2dResourceCounter: No implementation");
	if (!g2dResourceCounter::sm_pImplementation)
		return 0;
	return sm_pImplementation->GetNumVertexBuffers();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounter::GetTotalIndexBufferMemory()
{
	DBG_ASSERT(g2dResourceCounter::sm_pImplementation, "g2dResourceCounter: No implementation");
	if (!g2dResourceCounter::sm_pImplementation)
		return 0.0f;
	return sm_pImplementation->GetTotalIndexBufferMemory();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounter::GetNumIndexBuffers()
{
	DBG_ASSERT(g2dResourceCounter::sm_pImplementation, "g2dResourceCounter: No implementation");
	if (!g2dResourceCounter::sm_pImplementation)
		return 0;
	return sm_pImplementation->GetNumIndexBuffers();
}

//--------------------------------------------------------------------
// Set new implementation method, returns pointer to last one
// that was being used.  Both can be NULL.
// Ownership for the pointer remains with the caller.
//--------------------------------------------------------------------
//static
g2dResourceCounterImpl* g2dResourceCounter::SetImplementation(g2dResourceCounterImpl* i_Creator)
{
	g2dResourceCounterImpl* old_impl = sm_pImplementation;
	sm_pImplementation = i_Creator;
	return old_impl;
}

