/*****************************************************************************
**	sbrdAdapterGetPosition.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/Timeline/sbrdAdapterGetPosition.hpp"

#include "Tool/api3d/api3dObject.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Graphics/ent/entEntity.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
sbrdAdapterGetPosition::sbrdAdapterGetPosition( api3dObject* i_pObject )
: m_pObject(i_pObject)
{
}


//--------------------------------------------------------------------
//  Get position of object, for localizing sounds
//--------------------------------------------------------------------
maPoint3d  sbrdAdapterGetPosition::GetPosition() const
{
	return m_pObject->GetPosition();
}
