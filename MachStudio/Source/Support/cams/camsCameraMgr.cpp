/*****************************************************************************
**	camsCameraMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/cams/camsCameraMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Tool/gpx/gpxCamera.hpp"

#include <iterator>

//============================================================================
//============================================================================
namespace camsCameraMgr
{
	namespace
	{
		std::vector<nameString> l_Names;
		std::vector<std::string> l_Descs;
		std::vector< shared_ptr<camCamera> > l_Cameras;
		std::vector< shared_ptr<gpxCamera> > l_CameraProxies;

		// Callbacks
		std::vector<CameraListChangedCallback*> l_Callbacks;

		void notify_callbacks()
		{
			std::for_each(l_Callbacks.begin(), l_Callbacks.end(), 
				std::mem_fun(&CameraListChangedCallback::CameraListChanged));
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	//  Get number of cameras
	//--------------------------------------------------------------------
	int GetNumCameras()
	{
		return l_Names.size();
	}

	//--------------------------------------------------------------------
	//  Get names of cameras
	//--------------------------------------------------------------------
	void GetCameraNames(std::vector<nameString> &o_Names)
	{
		std::copy(l_Names.begin(), l_Names.end(), std::back_inserter(o_Names));
	}

	//--------------------------------------------------------------------
	//  Get name of camera
	//--------------------------------------------------------------------
	void GetCameraName( int i_Index, nameString &o_Name )
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < l_Names.size()), "index out of range" );
		o_Name = l_Names[i_Index];
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  SetCameraDescription(int i_Index, const std::string& i_Description)
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < l_Descs.size()), "index out of range" );
		l_Descs[i_Index] = i_Description;
		notify_callbacks();
	}
	void GetCameraDescription( int i_Index, std::string& o_Description )
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < l_Descs.size()), "index out of range" );
		o_Description = l_Descs[i_Index];
	}

	//--------------------------------------------------------------------
	//  Get camera by index
	//--------------------------------------------------------------------
	camCamera* GetCamera( int i_Index )
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < l_Cameras.size()), "index out of range" );
		return  l_Cameras[i_Index].get();
	}
	gpxCamera* GetCameraProxy( int i_Index )
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < l_CameraProxies.size()), "index out of range" );
		return  l_CameraProxies[i_Index].get();
	}

	//--------------------------------------------------------------------
	// return index of given named camera, returns -1 if not found
	//--------------------------------------------------------------------
	int GetIndexForName( const nameString& i_Name )
	{
		for (int i=0; i<l_Names.size(); i++)
			if (l_Names[i] == i_Name)
				return i;
		return -1;
	}

	//--------------------------------------------------------------------
	// return index of given camera, returns -1 if not found
	//--------------------------------------------------------------------
	int GetIndexForCamera( camCamera* i_pCamera )
	{
		for (int i=0; i<l_Cameras.size(); i++)
			if (l_Cameras[i].get() == i_pCamera)
				return i;
		return -1;
	}

	//--------------------------------------------------------------------
	//  Clear out list of cameras
	//--------------------------------------------------------------------
	void Clear()
	{
		l_Names.clear();
		l_Descs.clear();
		l_Cameras.clear();
		l_CameraProxies.clear();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Add new camera to list with given name
	//--------------------------------------------------------------------
	int  AddCamera(const nameString& i_Name, 
				   const std::string& i_Description,
				   shared_ptr<camCamera> i_pCamera,
				   shared_ptr<gpxCamera> i_pCameraProxy)
	{
		l_Names.push_back(i_Name);
		//l_Descs.resize(l_Names.size());
		l_Descs.push_back(i_Description);
		l_Cameras.push_back(i_pCamera);
		l_CameraProxies.push_back(i_pCameraProxy);

		notify_callbacks();

		int index = l_Names.size() - 1;
		return index;
	}

	//--------------------------------------------------------------------
	//  Remove new camera from list with given name
	//--------------------------------------------------------------------
	void  RemoveCamera(const nameString& i_Name)
	{
		int index = GetIndexForName(i_Name);
		RemoveCamera(index);
	}

	//--------------------------------------------------------------------
	//  Remove new camera from list with given name
	//--------------------------------------------------------------------
	void  RemoveCamera(const int i_Index)
	{
		if (i_Index >= 0)
		{
			l_Names.erase(l_Names.begin() + i_Index);
			l_Descs.erase(l_Descs.begin() + i_Index);
			l_Cameras.erase(l_Cameras.begin() + i_Index);
			l_CameraProxies.erase(l_CameraProxies.begin() + i_Index);
			
			notify_callbacks();
		}
	}

	//--------------------------------------------------------------------
	//  Add new camera to world
	//--------------------------------------------------------------------
	void  SetCameraName(int i_Index, const nameString& i_Name)
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < l_Names.size()), "index out of range" );
		l_Names[i_Index] = i_Name;
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Send message to callbacks that the camera with the given
	//	index should be selected in the user interface.
	//--------------------------------------------------------------------
	void  SelectCamera(int i_Index)
	{
		std::vector<CameraListChangedCallback*>::iterator it;
		for (it = l_Callbacks.begin(); it != l_Callbacks.end(); ++it)
		{
			(*it)->SelectCamera(i_Index);
		}
	}

	//--------------------------------------------------------------------
	//	List the names and IDs
	//--------------------------------------------------------------------
	void DebugNames()
	{
		DBG_TRACE("Names................................");
		int index;
		for (index = 0; index < l_Names.size(); ++index)
		{
			DBG_TRACE(l_Names[index].GetUID() << " - " << l_Names[index].GetString().c_str() );
		}
	}

	//--------------------------------------------------------------------
	//  Add/Remove callbacks - callback objects are not owned
	//--------------------------------------------------------------------
	void AddCallback(CameraListChangedCallback* i_pCallback)
	{
		l_Callbacks.push_back(i_pCallback);
	}
	void RemoveCallback(CameraListChangedCallback* i_pCallback)
	{
		envSTLHelpers::RemoveOneValue(l_Callbacks, i_pCallback);
	}

}	// end of namespace
