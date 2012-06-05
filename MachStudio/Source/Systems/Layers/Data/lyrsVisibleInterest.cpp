/****************************************************************************\
**	lyrsVisibleInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Layers/Data/lyrsVisibleInterest.hpp"

#include "Support/lyer/lyerLayerMgr.hpp"



//--------------------------------------------------------------------
//--------------------------------------------------------------------
lyrsVisibleInterest::lyrsVisibleInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
lyrsVisibleInterest::~lyrsVisibleInterest()
{
}


//--------------------------------------------------------------------
//	ShowIcons - show or hide icons that are not part of real scene.
//--------------------------------------------------------------------
void lyrsVisibleInterest::ShowIcons( bool i_bVisible )
{
	// nothing to do here
}

//--------------------------------------------------------------------
// Make sure that all geometry is visible for rendering
//--------------------------------------------------------------------
//virtual 
void lyrsVisibleInterest::ConfirmGeometryVisible()
{
	// Make sure all layers are visible and solid
	lyerLayerMgr::AllLayersVisible();
}
