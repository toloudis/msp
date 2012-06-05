/****************************************************************************\
**	visMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/vis/visMgr.hpp"

#include "Support/vis/visVisibleInterest.hpp"

//	library
#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace
{
	std::vector<visVisibleInterest*>	l_VisibleInterestList;
	bool l_bShowIcons = true;
}


//--------------------------------------------------------------------
//	ShowIcons - show or hide icons that are not part of real scene.
//--------------------------------------------------------------------
void visMgr::ShowIcons( bool i_bVisible )
{
	l_bShowIcons = i_bVisible;

	std::vector<visVisibleInterest*>::iterator it, end = l_VisibleInterestList.end();

	//	notify visible interests
	//
	for (it  = l_VisibleInterestList.begin(); it != end ; ++it)
	{
		(*it)->ShowIcons( i_bVisible );
	}
}

//--------------------------------------------------------------------
// Return if icons are current visible
//--------------------------------------------------------------------
bool visMgr::IsShowIcons()
{
	return l_bShowIcons;
}


//--------------------------------------------------------------------
// Make sure that all geometry is visible for rendering
//--------------------------------------------------------------------
void visMgr::ConfirmGeometryVisible()
{
	//	notify visible interests
	//
	std::vector<visVisibleInterest*>::iterator it, end = l_VisibleInterestList.end();
	for (it  = l_VisibleInterestList.begin(); it != end ; ++it)
	{
		(*it)->ConfirmGeometryVisible();
	}
}

//--------------------------------------------------------------------
// Set object active state in the render layer
//--------------------------------------------------------------------
void visMgr::SetActiveInRenderLayer(std::string& i_ObjectName, bool i_bActive)
{
	//	notify visible interests
	//
	std::vector<visVisibleInterest*>::iterator it, end = l_VisibleInterestList.end();
	for (it  = l_VisibleInterestList.begin(); it != end ; ++it)
	{
		(*it)->SetActiveInRenderLayer(i_ObjectName, i_bActive);
	}
}

//--------------------------------------------------------------------
// Set object visible state while editting
//--------------------------------------------------------------------
void visMgr::SetVisibleInEditor(std::string& i_ObjectName, bool i_bVisible)
{
	//	notify visible interests
	//
	std::vector<visVisibleInterest*>::iterator it, end = l_VisibleInterestList.end();
	for (it  = l_VisibleInterestList.begin(); it != end ; ++it)
	{
		(*it)->SetVisibleInEditor(i_ObjectName, i_bVisible);
	}
}


//--------------------------------------------------------------------
// Get object visible state while editting
//--------------------------------------------------------------------
bool visMgr::GetVisibleInEditor(std::string& i_ObjectName)
{
	//	query visible interests
	//
	std::vector<visVisibleInterest*>::iterator it, end = l_VisibleInterestList.end();
	for (it  = l_VisibleInterestList.begin(); it != end ; ++it)
	{
		if( (*it)->InterestHasObject(i_ObjectName) )
		{
			return (*it)->GetVisibleInEditor(i_ObjectName);
		}
	}
	return false;
}

//--------------------------------------------------------------------
//	RegisterVisibleInterest() - add a Visible interest to the system
//--------------------------------------------------------------------
void visMgr::RegisterVisibleInterest( visVisibleInterest* i_pInterest )
{
	DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Visible Interest" );

	l_VisibleInterestList.push_back( i_pInterest );
}

//--------------------------------------------------------------------
//	UnRegisterVisibleInterest() - remove a Visible interest from the system.
//
//	Note: this will NOT delete the Visible interest.  It is up to the
//	registerer.
//--------------------------------------------------------------------
void visMgr::UnRegisterVisibleInterest( visVisibleInterest* i_pInterest )
{
	envSTLHelpers::RemoveOneValue( l_VisibleInterestList, i_pInterest );
}

//----------------------------------------------------------------------------
//	Clear() - clear the list
//----------------------------------------------------------------------------
void visMgr::Clear()
{
	l_VisibleInterestList.clear();
}

