/*****************************************************************************
**	envtDocumentChunk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Data/envtDocumentChunk.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual 
const char* envtDocumentChunk::GetChunkDesc() const
{
	return "Environment Lights";
}
