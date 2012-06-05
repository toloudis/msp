/*****************************************************************************
**	tmlnTimelineMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"

#include "Support/xfrm/xfrmTransformMgr.hpp"

// library
//#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/G3d/g3dThreadControl.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gpx/gpxProxyMgr.hpp"

//
#include <iterator>
#include <list>


//============================================================================
//============================================================================
namespace timelinemap
{
	std::vector<std::string> l_ScriptObjectCategories;			//  or system
	std::vector<tmlnScriptObject*> l_ScriptObjects;
}


//--------------------------------------------------------------------
//	Deinitialize/Initialize
//--------------------------------------------------------------------
void tmlnTimelineMgr::Initialize()
{
	timelinemap::l_ScriptObjects.clear();
	timelinemap::l_ScriptObjectCategories.clear();
}
void tmlnTimelineMgr::DeInitialize()
{
	timelinemap::l_ScriptObjects.clear();
	timelinemap::l_ScriptObjectCategories.clear();
}

//--------------------------------------------------------------------
//	Update()
//--------------------------------------------------------------------
void tmlnTimelineMgr::Update(const maTime& i_Time)
{
	// some drivers, such as attachment, need to be delayed to make 
	// sure others, such as animation, have set the final position
	// 
	std::list<tmlnDriver*> delayed_drivers;

	std::vector<tmlnScriptObject*>::iterator obj_it	= timelinemap::l_ScriptObjects.begin();
	std::vector<tmlnScriptObject*>::iterator obj_end	= timelinemap::l_ScriptObjects.end();
	for ( ; obj_it != obj_end; ++obj_it )
	{
		(*obj_it)->Update(i_Time, delayed_drivers);
	}

	if (!delayed_drivers.empty())
	{
		// Because of the way attachments work, we need to flush the transformations
		// from the drivers that just finished updating into the scene graph.
		// But, we don´t want to interrupt a render for this - just when there is no
		// render thread already running. This will make it so that capture will 
		// be correct, but interactive modes will catch up the next frame.
		if (!g3dThreadControl::IsRenderThreadActive())
		{
			// Push through hierarchical transformations into proxies
			xfrmTransformMgr::UpdateTransforms();

			// Push through all of the buffered changes in the graphical proxies,
			// mark that a new render is needed if there were changes.
			if (gpxProxyMgr::Update())
				gpxRenderControl::SetNeedsNewRender();
		}

		// Now update the drivers that need to be delayed
		// (the ones that are attached to properties that were animated above)
		std::list<tmlnDriver*>::iterator drv_it	= delayed_drivers.begin();
		std::list<tmlnDriver*>::iterator drv_end = delayed_drivers.end();
		for ( ; drv_it != drv_end; ++drv_it )
		{
			(*drv_it)->ClearDirty();
			(*drv_it)->Operate(i_Time);
		}
	}
}

//--------------------------------------------------------------------
//	AddObject() - not owned
//--------------------------------------------------------------------
void tmlnTimelineMgr::AddObject( tmlnScriptObject * i_pObject, const char* i_Category )
{
	timelinemap::l_ScriptObjects.push_back(i_pObject);
	std::string cat(i_Category);
	timelinemap::l_ScriptObjectCategories.push_back( cat );
}

//--------------------------------------------------------------------
//	RemoveObject()
//--------------------------------------------------------------------
void tmlnTimelineMgr::RemoveObject( tmlnScriptObject * i_pObject )
{
	std::vector<std::string>::iterator cat_it			= timelinemap::l_ScriptObjectCategories.begin();
	std::vector<tmlnScriptObject*>::iterator obj_it	= timelinemap::l_ScriptObjects.begin();
	std::vector<tmlnScriptObject*>::iterator obj_end	= timelinemap::l_ScriptObjects.end();
	for( ; obj_it != obj_end; ++obj_it )
	{
		if ((*obj_it) == i_pObject)
		{
			timelinemap::l_ScriptObjects.erase(obj_it);
			timelinemap::l_ScriptObjectCategories.erase(cat_it);
			return;
		}
		++cat_it;
	}
}

//--------------------------------------------------------------------
//	GetObjects() - get list fo all objects
//--------------------------------------------------------------------
void tmlnTimelineMgr::GetObjects( std::vector<tmlnScriptObject*> &o_Objects, const char* i_Category )
{
	if ( strlen(i_Category) == 0 )
	{
		std::copy(timelinemap::l_ScriptObjects.begin(), timelinemap::l_ScriptObjects.end(), std::back_inserter(o_Objects));
	}
	else
	{
		std::vector<std::string>::iterator cat_it			= timelinemap::l_ScriptObjectCategories.begin();
		std::vector<tmlnScriptObject*>::iterator obj_it	= timelinemap::l_ScriptObjects.begin();
		std::vector<tmlnScriptObject*>::iterator obj_end	= timelinemap::l_ScriptObjects.end();
		for( ; obj_it != obj_end; ++obj_it )
		{
			if ( strcmp((*cat_it).c_str(), i_Category) == 0)
			{
				o_Objects.push_back( (*obj_it) );
			}
			++cat_it;
		}
	}
}

//--------------------------------------------------------------------
//	GetObjectCategory() - get category for this object
//--------------------------------------------------------------------
const char* tmlnTimelineMgr::GetObjectCategory( tmlnScriptObject* i_pObject)
{
	if ( !i_pObject) return "";

	std::vector<std::string>::iterator cat_it			= timelinemap::l_ScriptObjectCategories.begin();
	std::vector<tmlnScriptObject*>::iterator obj_it	= timelinemap::l_ScriptObjects.begin();
	std::vector<tmlnScriptObject*>::iterator obj_end	= timelinemap::l_ScriptObjects.end();
	for( ; obj_it != obj_end; ++obj_it )
	{
		if ((*obj_it) == i_pObject)
		{
			return (*cat_it).c_str();
		}
		++cat_it;
	}
	return "";
}

//--------------------------------------------------------------------
// Get the time bounds of all drivers in the scene. Could be used
//	to set the main timeline's bounds. Returns false if there
//	are no drivers in the scene.
//--------------------------------------------------------------------
bool tmlnTimelineMgr::GetDriverBounds(maTime &o_MinTime, maTime &o_MaxTime)
{
	bool found_drivers = false;

	std::vector<tmlnScriptObject*>::iterator obj_it	= timelinemap::l_ScriptObjects.begin();
	std::vector<tmlnScriptObject*>::iterator obj_end	= timelinemap::l_ScriptObjects.end();
	for ( ; obj_it != obj_end; ++obj_it )
	{
		tmlnScriptObject *scr_obj = (*obj_it);
		const int num_drivers = scr_obj->GetNumDrivers();
		for (int d=0; d<num_drivers; ++d)
		{
			const tmlnDriver &driver = scr_obj->GetDriver(d);
			if (!found_drivers)
			{
				found_drivers = true;
				o_MinTime = driver.GetBeginTime();
				o_MaxTime = driver.GetEndTime();
			}
			else
			{
				if (driver.GetBeginTime() < o_MinTime)
					o_MinTime = driver.GetBeginTime();
				if (driver.GetEndTime() > o_MaxTime)
					o_MaxTime = driver.GetEndTime();
			}
		}
	}

	return found_drivers;
}

