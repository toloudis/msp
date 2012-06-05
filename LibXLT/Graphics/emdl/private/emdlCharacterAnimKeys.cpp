/*****************************************************************************
**  emdlCharacterAnimKeys.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/emdl/emdlCharacterAnimKeys.hpp"

#include "Core/env/envSTLHelpers.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
emdlCharacterAnimKeys::emdlCharacterAnimKeys()
:	m_bAttachToRootJoint(false)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
emdlCharacterAnimKeys::~emdlCharacterAnimKeys()
{
	envSTLHelpers::DeleteContainer(m_MorphChannels);
}

//--------------------------------------------------------------------
// Old animation files need to attach to the root joint
// in order to mimic previous behavior correctly.
//--------------------------------------------------------------------
void emdlCharacterAnimKeys::SetAttachToRootJoint(bool i_bAttach)
{
	m_bAttachToRootJoint = i_bAttach;
}
