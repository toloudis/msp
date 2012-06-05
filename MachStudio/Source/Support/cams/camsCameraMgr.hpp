/*****************************************************************************
**	camsCameraMgr.hpp
**
**	Keeps track of the number and name of the cameras in a scene
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CAMS_CAMERAMGR_HPP
#error camsCameraMgr.hpp multiply included
#endif
#define CAMS_CAMERAMGR_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <string>
#include <vector>


//============================================================================
//============================================================================
class camCamera;
class gpxCamera;


//============================================================================
//============================================================================
namespace camsCameraMgr
{
	//--------------------------------------------------------------------
	//  Get number of cameras
	//--------------------------------------------------------------------
	int GetNumCameras();

	//--------------------------------------------------------------------
	//  Get names of cameras
	//--------------------------------------------------------------------
	void GetCameraNames(std::vector<nameString> &o_Names);

	//--------------------------------------------------------------------
	//  Get name of camera
	//--------------------------------------------------------------------
	void GetCameraName( int i_Index, nameString &o_Name );

	//--------------------------------------------------------------------
	// Get description of camera
	//--------------------------------------------------------------------
	void SetCameraDescription(int i_Index, const std::string& i_Description);
	void GetCameraDescription( int i_Index, std::string& o_Description );

	//--------------------------------------------------------------------
	//  Get camera by index
	//--------------------------------------------------------------------
	camCamera* GetCamera( int i_Index );
	gpxCamera* GetCameraProxy( int i_Index );

	//--------------------------------------------------------------------
	// return index of given named camera, returns -1 if not found
	//--------------------------------------------------------------------
	int GetIndexForName( const nameString& i_Name );

	//--------------------------------------------------------------------
	// return index of given camera, returns -1 if not found
	//--------------------------------------------------------------------
	int GetIndexForCamera( camCamera* i_pCamera );

	//--------------------------------------------------------------------
	//  Clear out list of cameras
	//--------------------------------------------------------------------
	void Clear();

	//--------------------------------------------------------------------
	//  Add new camera to list with given name
	//--------------------------------------------------------------------
	int  AddCamera(const nameString& i_Name, 
				   const std::string& i_Description,
				   shared_ptr<camCamera> i_pCamera,
				   shared_ptr<gpxCamera> i_pCameraProxy);

	//--------------------------------------------------------------------
	//  Remove new camera from list with given name
	//--------------------------------------------------------------------
	void  RemoveCamera(const int i_Index);
	void  RemoveCamera(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Add new camera to world
	//--------------------------------------------------------------------
	void  SetCameraName(int i_Index, const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Send message to callbacks that the camera with the given
	//	index should be selected in the user interface.
	//--------------------------------------------------------------------
	void  SelectCamera(int i_Index);

	//--------------------------------------------------------------------
	// Callback for when number, names, descriptions change
	//--------------------------------------------------------------------
	class CameraListChangedCallback
	{
	public:
		virtual void CameraListChanged() = 0;
		virtual void SelectCamera(int i_Index) {}
	};

	//--------------------------------------------------------------------
	//  Add/Remove callbacks - callback objects are not owned
	//--------------------------------------------------------------------
	void AddCallback(CameraListChangedCallback* i_pCallback);
	void RemoveCallback(CameraListChangedCallback* i_pCallback);

}	// end of namespace
