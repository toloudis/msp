/*****************************************************************************
**  cmmVisibleInterestTemplate.hpp
**
**      Visible interest for systems with lists of objects
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef CMM_VISIBLEINTERESTTEMPLATE_HPP
#error cmmVisibleInterestTemplate.hpp multiply included
#endif
#define CMM_VISIBLEINTERESTTEMPLATE_HPP

#ifndef VIS_VISIBLEINTEREST_HPP
#include "Support/vis/visVisibleInterest.hpp"
#endif

//============================================================================
//============================================================================
template<class xxxObjectMgr>
class cmmVisibleInterestTemplate : public visVisibleInterest
{
public:
	//--------------------------------------------------------------------
	//	ShowIcons - show or hide icons that are not part of real scene.
	//--------------------------------------------------------------------
	virtual void ShowIcons( bool i_bVisible )
	{
		xxxObjectMgr::ShowIcons(i_bVisible);
	}
	
	//--------------------------------------------------------------------
	// Set object active state in the render layer
	//--------------------------------------------------------------------
	virtual void SetActiveInRenderLayer(std::string& i_ObjectName, bool i_bActive)
	{
		nameString objname(i_ObjectName);
		int index = xxxObjectMgr::GetIndexForObject(objname);
		if (index >= 0)
		{
			xxxObjectMgr::SetActiveRenderLayer(index, i_bActive);
		}
	};

	//--------------------------------------------------------------------
	// Set object visible state while editting
	//--------------------------------------------------------------------
	virtual void SetVisibleInEditor(std::string& i_ObjectName, bool i_bVisible)
	{
		nameString objname(i_ObjectName);
		int index = xxxObjectMgr::GetIndexForObject(objname);
		if (index >= 0)
		{
			xxxObjectMgr::SetEditorVisible(index, i_bVisible);
		}
	};
	
	//--------------------------------------------------------------------
	// Get object visible state while editting
	//--------------------------------------------------------------------
	virtual bool GetVisibleInEditor(std::string& i_ObjectName)
	{
		nameString objname(i_ObjectName);
		int index = xxxObjectMgr::GetIndexForObject(objname);
		if (index >= 0)
		{
			return xxxObjectMgr::GetEditorVisible(index);
		}

		return false;
	};

	//--------------------------------------------------------------------
	// Get whether or not this interest has the object
	//--------------------------------------------------------------------
	virtual bool InterestHasObject(std::string& i_ObjectName)
	{
		nameString objname(i_ObjectName);
		int index = xxxObjectMgr::GetIndexForObject(objname);
		if (index >= 0)
		{
			return true;
		}

		return false;
	};
};

//============================================================================
// Variation of the Visible PickInterest used for systems where
//	the objects maintain geometry that needs to be rendered.
//============================================================================
template<class xxxObjectMgr>
class cmmVisibleInterestGeomTemplate : public cmmVisibleInterestTemplate<xxxObjectMgr>
{
public:
	//--------------------------------------------------------------------
	// Make sure that all geometry is visible for rendering
	//--------------------------------------------------------------------
	virtual void ConfirmGeometryVisible()
	{
		xxxObjectMgr::ConfirmGeometryVisible();
	}
};
