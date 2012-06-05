/*****************************************************************************
**	tmlnTimelineMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Support/tmln/tmlnTimelineMgr.hpp"

// tool
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"

// library
//#include "Core/dbg/dbgAssert.hpp"
#include "Core/env/envSTLHelpers.hpp"

//
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
void tmlnTimelineMgr::Update(float i_Time)
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

	std::list<tmlnDriver*>::iterator drv_it	= delayed_drivers.begin();
	std::list<tmlnDriver*>::iterator drv_end = delayed_drivers.end();
	for ( ; drv_it != drv_end; ++drv_it )
	{
		(*drv_it)->ClearDirty();
		(*drv_it)->Operate(i_Time);
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
bool tmlnTimelineMgr::GetDriverBounds(float &o_MinTime, float &o_MaxTime)
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
				if (driver.GetEndTime() < o_MaxTime)
					o_MaxTime = driver.GetEndTime();
			}
		}
	}

	return found_drivers;
}

