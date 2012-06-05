/*****************************************************************************
**  dirltScriptObject.hpp
**
**      A dirltScriptObject is a derived class for displaying a point
**	light's position.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef DIRLT_SCRIPTOBJECT_HPP
#error dirltScriptObject.hpp multiply included
#endif
#define DIRLT_SCRIPTOBJECT_HPP

#ifndef DIRLT_DIRLIGHTOBJECT_HPP
#include "dirltDirLightObject.hpp"
#endif
#ifndef DIRLT_SCRIPTDATA_HPP
#include "dirltScriptData.hpp"
#endif

#ifndef MNM_OBJECT_HPP
#include "mnmObject.hpp"
#endif
#ifndef NAME_OBJECT_HPP
#include "nameObject.hpp"
#endif
#ifndef TMLN_SCRIPTOBJECT_HPP
#include "tmlnScriptObject.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class cmpsManipObject;
class dirltChannelLightPos;
class dirltChannelEnabled;
class dirltChannelColor;
class g3dDirectionalLight;
class tmlnChannelBoolean;
class tmlnChannelColor;
class tmlnChannelPosition;

//============================================================================
//============================================================================
class dirltScriptObject : public pick3dPickObject, public tmlnScriptObject, public nameObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		dirltScriptObject(const dirltScriptData &i_Data);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		dirltScriptObject(g3dDirectionalLight* i_pLight);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~dirltScriptObject();

	//========================================================================
	//	Data
	//========================================================================

		//--------------------------------------------------------------------
		// Get values as a light data structure
		//--------------------------------------------------------------------
		dirltScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from light data structure
		//--------------------------------------------------------------------
		void SetScriptData(const dirltScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a light base data structure
		//--------------------------------------------------------------------
		dirltData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from light base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const dirltData &i_Data);

	//============================================================================
	//	Timeline
	//============================================================================

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		tmlnChannelPosition& PositionChannel();
		tmlnChannelBoolean& EnabledChannel();
		tmlnChannelColor& ColorChannel();

	//============================================================================
	//	3D icon/geometry
	//============================================================================

		//--------------------------------------------------------------------
		//	Position
		//--------------------------------------------------------------------
		virtual maPoint3d GetPosition() const;
		void SetPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	Orientation
		//--------------------------------------------------------------------
		virtual maRotation GetOrientation() const;
		virtual void SetOrientation(const maRotation& i_Orientation);

		//--------------------------------------------------------------------
		//	Scale
		//--------------------------------------------------------------------
		virtual maPoint3d GetScale() const;
		virtual void SetScale(const maPoint3d& i_Scale);

		//--------------------------------------------------------------------
		// Name
		//--------------------------------------------------------------------
		virtual void SetName(const nameString& i_Name);

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the dirltScriptObject.
		//	If it does, the t value is also returned in o_T.
		//--------------------------------------------------------------------
		virtual bool RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T);

		//--------------------------------------------------------------------
		//	Renderable sets whether the dirltScriptObject can be selected.
		//--------------------------------------------------------------------
		virtual void SetRenderable(bool i_Renderable);
		virtual bool GetRenderable() const;

		//--------------------------------------------------------------------
		//	does this object own the lights it holds?
		//--------------------------------------------------------------------
		void SetLightOwner( bool i_bOwner );
		bool GetLightOwner() const;

		//--------------------------------------------------------------------
		// For polling if an manipulation operation is currently enabled
		//--------------------------------------------------------------------
		virtual bool IsOperationEnabled(cmpsManipObject::Operations i_Operation, float i_Time);

		//--------------------------------------------------------------------
		// This is called when a driver in this script object is changed,
		//	or if a driver is added or deleted from this object.
		//--------------------------------------------------------------------
		virtual void NotifyDriverChanged();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		dirltDirLightObject*	GetPickObject() const;

	private:
		// Timeline related
		dirltChannelLightPos*	m_pPosChannel;
		dirltChannelEnabled*	m_pEnableChannel;
		dirltChannelColor*		m_pColorChannel;

		dirltDirLightObject*	m_pIcon;
		dirltScriptData			m_Data;
};

