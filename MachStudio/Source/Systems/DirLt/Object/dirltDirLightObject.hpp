/*****************************************************************************
**  dirltDirLightObject.hpp
**
**      A dirltDirLightObject is a derived class for displaying a point
**	light's position.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef DIRLT_DIRLIGHTOBJECT_HPP
#error dirltDirLightObject.hpp multiply included
#endif
#define DIRLT_DIRLIGHTOBJECT_HPP

#ifndef DIRLT_SCRIPTDATA_HPP
#include "dirltScriptData.hpp"
#endif
#ifndef MNM_OBJECT_HPP
#include "mnmObject.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "maAxisBox.hpp"
#endif
#ifndef NAME_OBJECT_HPP
#include "nameObject.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "prtyObject.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class g3dDirectionalLight;


//============================================================================
//============================================================================
class dirltDirLightObject : public mnmObject, public nameObject, public prtyObject
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		dirltDirLightObject(const dirltData &i_Data);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		dirltDirLightObject(g3dDirectionalLight* i_pLight);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~dirltDirLightObject();

	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a light data structure
		//--------------------------------------------------------------------
		dirltData GetData() const;

		//--------------------------------------------------------------------
		// Set from light data structure
		//--------------------------------------------------------------------
		void SetData(const dirltData &i_Data);

	//============================================================================
	//	3D icon/geometry
	//============================================================================

		//--------------------------------------------------------------------
		//	GetWorldBox returns a box which completely encloses the
		//	dirltDirLightObject.
		//--------------------------------------------------------------------
		virtual const maAxisBox& GetWorldBox() const;

		//--------------------------------------------------------------------
		//	GetLocalBox returns a box which would enclose the dirltDirLightObject
		//	if its transformations were identity
		//--------------------------------------------------------------------
		virtual const maAxisBox& GetLocalBox() const;

		//--------------------------------------------------------------------
		// UpdateName() is called when the gui sets the name of the object,
		//	derived classes can set dirty bits and do "undo" operations, etc.
		// The default behavior calls SetName()
		//--------------------------------------------------------------------
		virtual void UpdateName(const std::string& i_Name);
		void SetName(const nameString& i_Name);

		//--------------------------------------------------------------------
		//	Position
		//--------------------------------------------------------------------
		virtual maPoint3d GetPosition() const;
		virtual void UpdatePosition(const maPoint3d& i_Position);
		virtual void SetPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	Orientation
		//--------------------------------------------------------------------
		virtual maRotation GetOrientation() const;
		virtual void UpdateOrientation(const maRotation& i_Orientation);
		virtual void SetOrientation(const maRotation& i_Orientation);

		//--------------------------------------------------------------------
		//	Scale
		//--------------------------------------------------------------------
		virtual maPoint3d GetScale() const;
		virtual void UpdateScale(const maPoint3d& i_Scale);
		virtual void SetScale(const maPoint3d& i_Scale);

		//--------------------------------------------------------------------
		//	Intensity
		//--------------------------------------------------------------------
		virtual maFloatRGBA GetIntensity() const;
		virtual void SetIntensity(const maFloatRGBA& i_Intensity);

		//--------------------------------------------------------------------
		//	Enabled
		//--------------------------------------------------------------------
		virtual bool GetEnabled() const;
		virtual void SetEnabled(const bool& i_Enabled);

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the dirltDirLightObject.
		//	If it does, the t value is also returned in o_T.
		//--------------------------------------------------------------------
		virtual bool RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T);

		//--------------------------------------------------------------------
		//	Renderable sets whether the dirltDirLightObject can be selected.
		//--------------------------------------------------------------------
		virtual void SetRenderable(bool i_Renderable);
		virtual bool GetRenderable() const;

		//--------------------------------------------------------------------
		//	GetDefaultTerrainOffset is the desired offset from
		//	the terrain for this object.  This can be altered
		//	by the user during placement
		//--------------------------------------------------------------------
		virtual float GetDefaultTerrainOffset() const;

		//--------------------------------------------------------------------
		//	does this object own the lights it holds?
		//--------------------------------------------------------------------
		void SetLightOwner( bool i_bOwner );
		bool GetLightOwner() const;

		//--------------------------------------------------------------------
		// Flags for how object can be modified
		//--------------------------------------------------------------------
		virtual RotateFlags	GetRotateFlags();
		virtual ScaleFlags	GetScaleFlags();
		virtual TranslateFlags GetTranslateFlags();

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		virtual pick3dPickObject* GetParentObject() const;
		virtual void SetParentObject(pick3dPickObject* i_pParent);

	private:
		//--------------------------------------------------------------------
		//	SetObjectPositionAndOrientation()
		//--------------------------------------------------------------------
		void SetObjectPositionAndOrientation();

		//--------------------------------------------------------------------
		//	Register the properties for display
		//--------------------------------------------------------------------
		void RegisterProperties();

	private:
		bool	m_bLightOwner;
		g3dDirectionalLight* m_pLight;

		// 3D icon/geometry info
		api3dObject *m_pObject;
		api3dObject *m_pObjectLine;

		pick3dPickObject* m_pParent;

		maAxisBox m_WorldBox;
		maPoint3d	m_Position;
		maVector3d	m_Direction;
		maRotation	m_Orientation;

		dirltData	m_Data;
};

