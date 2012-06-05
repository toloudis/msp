/*****************************************************************************
**	propAdapterGetPosition.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Props/Timeline/propAdapterGetPosition.hpp"

#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Graphics/ent/entEntity.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
propAdapterGetPosition::propAdapterGetPosition( api3dObjectEntity* i_pObject )
: m_pProp(i_pObject)
{
}


//--------------------------------------------------------------------
//  Get position of object, for localizing sounds
//--------------------------------------------------------------------
maPoint3d  propAdapterGetPosition::GetPosition() const
{
	return m_pProp->GetEntity()->GetPosition();
}
