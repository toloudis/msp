/********************************************************************************************\
**	smdlBoneFragment.cpp
**
**		Maintains a shared fragment for representing joints
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Graphics/smdl/private/smdlBoneFragment.hpp"

#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"


//========================================================================
// Local variables and functions
//========================================================================
namespace
{
	g3dFragment* l_ConeFragment = NULL;
} // end of namespace


//------------------------------------------------------------------------
//	Initialize()
//------------------------------------------------------------------------
void	smdlBoneFragment::Initialize()
{
	DBG_ASSERT(l_ConeFragment == NULL, "BoneFragment already created.");
	if (l_ConeFragment != NULL)
		return;
	l_ConeFragment = g3dPrimitiveFragmentUtil::CreateCone(0.1f, 1.0f, 6);
	l_ConeFragment->SetCastsShadow( false );
}


//------------------------------------------------------------------------
//	DeInitialize()
//------------------------------------------------------------------------
void	smdlBoneFragment::DeInitialize()
{
	delete l_ConeFragment;
	l_ConeFragment = NULL;
}

//------------------------------------------------------------------------
//	GetSharedFragment - return pointer to fragment to represent bones
//------------------------------------------------------------------------
g3dFragment*	smdlBoneFragment::GetSharedFragment()
{
	return l_ConeFragment;
}
