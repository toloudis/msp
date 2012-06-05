/*****************************************************************************
**  cmpsManipObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/cmps/cmpsManipObject.hpp"

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
bool cmpsManipObject::IsOperationEnabled(Operations i_Operation, const maTime& i_Time)
{
	return true;
}
