/*****************************************************************************
**	tmlnTimelineMgr.hpp
**
**	A TimelineMap is list of script objects in order to update time
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_TIMELINEMGR_HPP
#error tmlnTimelineMgr.hpp multiply included
#endif
#define TMLN_TIMELINEMGR_HPP

#include <vector>

//============================================================================
//	forward references
//============================================================================
class maTime;
class sel3dObject;
class tmlnScriptObject;


//============================================================================
//============================================================================
namespace tmlnTimelineMgr
{
	//--------------------------------------------------------------------
	//	Deinitialize/Initialize
	//--------------------------------------------------------------------
	void Initialize();
	void DeInitialize();

	//--------------------------------------------------------------------
	//	Update()
	//--------------------------------------------------------------------
	void Update(const maTime& i_Time);

	//--------------------------------------------------------------------
	//	AddObject() - not owned
	//--------------------------------------------------------------------
	void AddObject( tmlnScriptObject * i_pObject, const char* i_Category );

	//--------------------------------------------------------------------
	//	RemoveObject()
	//--------------------------------------------------------------------
	void RemoveObject( tmlnScriptObject * i_pObject );

	//--------------------------------------------------------------------
	//	GetObjects() - get list fo all objects
	//--------------------------------------------------------------------
	void GetObjects( std::vector<tmlnScriptObject*> &o_Objects, const char* i_Category = "" );

	//--------------------------------------------------------------------
	//	GetObjectCategory() - get category for this object
	//--------------------------------------------------------------------
	const char* GetObjectCategory( tmlnScriptObject* i_pObject);

	//--------------------------------------------------------------------
	// Get the time bounds of all drivers in the scene. Could be used
	//	to set the main timeline's bounds. Returns false if there
	//	are no drivers in the scene.
	//--------------------------------------------------------------------
	bool GetDriverBounds(maTime &o_MinTime, maTime &o_MaxTime);
};
