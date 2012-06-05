/*****************************************************************************
**  cmpsManipObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/cmps/cmpsManipObject.hpp"

#include "Support/cmps/cmpsReference.hpp"

#include "Tool/api3d/api3dScene.hpp"
#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmpsManipObject::cmpsManipObject()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmpsManipObject::~cmpsManipObject()
{
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the cmpsManipObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
//virtual 
bool cmpsManipObject::RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T)
{
	return false;
}

//----------------------------------------------------------------------------
//	GetDefaultTerrainOffset is the desired offset from
//	the terrain for this object.  This can be altered
//	by the user during placement
//----------------------------------------------------------------------------
//virtual 
float cmpsManipObject::GetDefaultTerrainOffset() const
{
	return 0.0f;
}

//----------------------------------------------------------------------------
// Flags for how object can be modified
//----------------------------------------------------------------------------
//virtual 
cmpsManipObject::RotateFlags	cmpsManipObject::GetRotateFlags()
{
	return cmpsManipObject::e_RotateNone;
}
//virtual 
cmpsManipObject::ScaleFlags	cmpsManipObject::GetScaleFlags()
{
	return cmpsManipObject::e_ScaleNone;
}
//virtual 
cmpsManipObject::TranslateFlags	cmpsManipObject::GetTranslateFlags()
{
	return cmpsManipObject::e_TranslateAll;
}

//--------------------------------------------------------------------
// For polling if an manipulation operation is currently enabled
//--------------------------------------------------------------------
//virtual 
bool cmpsManipObject::IsOperationEnabled(Operations i_Operation, float i_Time)
{
	return true;
}
