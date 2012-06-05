/*****************************************************************************
**  entModelInstance.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/ent/entModelInstance.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTextureMgr.hpp"


//--------------------------------------------------------------------
//	Constructor
//	i_PathIndex is the Model index for this entity template, under this
// directory the Textures, Models, Sounds and Data directories can be found.
//--------------------------------------------------------------------
entModelInstance::entModelInstance()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
entModelInstance::~entModelInstance()
{
	envSTLHelpers::DeleteContainer(m_Materials);
	envSTLHelpers::DeleteContainer(m_Fragments);
	envSTLHelpers::ForAll(m_Textures, matTextureMgr::ReleaseTexture );
}
