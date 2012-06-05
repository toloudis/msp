/*****************************************************************************
**  dcutScriptObject.hpp
**
**      A dcutScriptObject contains the channel for the driver that switches
**	between cameras in a director's cut.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef DCUT_SCRIPTOBJECT_HPP
#error dcutScriptObject.hpp multiply included
#endif
#define DCUT_SCRIPTOBJECT_HPP

#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif
#ifndef TMLN_SCRIPTOBJECT_HPP
#include "Support/tmln/tmlnScriptObject.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class dcutCueData;
class dcutDirectorsCutObject;
class dcutChannelCamera;
class dcutChannelCapture;
class dcutScriptData;
class nameString;


//============================================================================
//============================================================================
class dcutScriptObject : public pick3dPickObject, public tmlnScriptObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		dcutScriptObject(const dcutScriptData &i_Data);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~dcutScriptObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetTmlnName() const;


	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a camera data structure
		//--------------------------------------------------------------------
		dcutScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from camera data structure
		//--------------------------------------------------------------------
		void SetScriptData(const dcutScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a base data structure
		//--------------------------------------------------------------------
		dcutCueData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const dcutCueData &i_Data);

	//============================================================================
	//	Timeline
	//============================================================================

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		dcutChannelCapture& CaptureChannel();
		dcutChannelCamera& CameraChannel();

	//============================================================================
	//	data
	//============================================================================

		//--------------------------------------------------------------------
		//	Name - passes functions on to pick object
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);
		const nameString& GetName() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		dcutDirectorsCutObject*	GetPickObject() const;

		//--------------------------------------------------------------------
		// This is called when a driver in this script object is changed,
		//	or if a driver is added or deleted from this object.
		//--------------------------------------------------------------------
		void NotifyDriverChanged();


	private:
		dcutDirectorsCutObject*	m_pIcon;

		// Timeline related
		dcutChannelCapture*		m_pCaptureChannel;
		dcutChannelCamera*		m_pCameraChannel;
};

