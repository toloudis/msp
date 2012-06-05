/*****************************************************************************
**	gpxTransformControl.hpp
**
**		This class is a thread-safe proxy for a scTransformControl.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_TRANSFORMCONTROL_HPP
#error gpxTransformControl.hpp multiply included
#endif
#define GPX_TRANSFORMCONTROL_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 
#ifndef MA_ROTATION_HPP
#include "Core/Ma/maRotation.hpp"
#endif 
#ifndef MA_VECTOR3D_HPP
#include "Core/Ma/maVector3d.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class scTransformControl;


//============================================================================
//============================================================================
class gpxTransformControl : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxTransformControl(scTransformControl &i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxTransformControl();

	//--------------------------------------------------------------------
	// Return accessor to control anim for this proxy.
	//--------------------------------------------------------------------
	scTransformControl* GetTransformControl();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the object when "Update()" is called.
	//--------------------------------------------------------------------
	void SetRotation(const maRotation& i_Rotation);
	void SetEulerAngles(const maVector3d& i_Angles);
	void SetTranslation(const maVector3d& i_Translation);
	void SetScale(const maVector3d& i_Scale);

	//--------------------------------------------------------------------
	//	Get functions just return the data internally based on
	//	the "Set" calls earlier.
	//--------------------------------------------------------------------
	const maRotation& GetRotation() const;

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	scTransformControl &m_Control;

#if USE_PROXIES
	bool m_bEulerAngles;
	maRotation m_Rotation;
	maVector3d m_EulerAngles;
	maVector3d m_Translation;
	maVector3d m_Scale;
#endif
};
