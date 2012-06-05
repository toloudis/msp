/*****************************************************************************
**	chtrAdapterGetPosition.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Character/Timeline/chtrAdapterGetPosition.hpp"

#include "Tool/api3d/api3dObject.hpp"
#include "Graphics/ent/entEntity.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrAdapterGetPosition::chtrAdapterGetPosition( api3dObject* i_pObject )
: m_pCharacter(i_pObject)
{
}


//--------------------------------------------------------------------
//  Get position of object, for localizing sounds
//--------------------------------------------------------------------
maPoint3d  chtrAdapterGetPosition::GetPosition() const
{
	return m_pCharacter->GetPosition();
}
