/********************************************************************************************\
**  mcpBoneFragment.cpp
**
**      Maintains a shared fragment for representing joints
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "mcpBoneFragment.hpp"

#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dFragment.hpp"

//========================================================================
// Local variables and functions
//========================================================================
namespace
{
	g3dFragment* l_ConeFragment = NULL;

} // end of namespace

//========================================================================
//	Initialize()
//========================================================================
void	mcpBoneFragment::Initialize()
{
	DBG_ASSERT0(l_ConeFragment == NULL, "BoneFragment already created.");
	l_ConeFragment = g3dPrimitiveFragmentUtil::CreateCone(0.1f, 1.0f, 6);
}


//========================================================================
//	DeInitialize()
//========================================================================
void	mcpBoneFragment::DeInitialize()
{
	delete l_ConeFragment;
	l_ConeFragment = NULL;
}

//========================================================================
//	GetSharedFragment - return pointer to fragment to represent bones
//========================================================================
g3dFragment*	mcpBoneFragment::GetSharedFragment()
{
	return l_ConeFragment;
}
