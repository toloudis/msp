/*****************************************************************************
**	gpxSceneObject.hpp
**
**		This class is a thread-safe proxy for a api3dObject.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_SCENEOBJECT_HPP
#error gpxSceneObject.hpp multiply included
#endif
#define GPX_SCENEOBJECT_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/Ma/maRotation.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class api3dObject;


//============================================================================
//============================================================================
class gpxSceneObject : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxSceneObject(api3dObject &i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxSceneObject();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the object when "Update()" is called.
	//--------------------------------------------------------------------
	void  SetPosition(const maPoint3d &i_Position);
	void  SetOrientation(const maRotation &i_Rotation);
	void  SetUniformScale(const float i_fScale);
	void  SetPivotPoint(const maPoint3d &i_Position, 
						   bool i_bPreserveTransformation = false);
	void  SetPivotCompensation(const maVector3d &i_Compensation);
	void SetWireframe(bool i_Wireframe);	
	void SetLowResolution(bool i_LowResolution);
	void SetGPUPickable(bool i_Pickable);
	void SetPickHull(bool i_PickHull);
	void SetInheritsTransform(bool i_bInherits);
	void SetColor(const maFloatRGBA &i_Color);
	void SetActiveFromRenderLayer(bool i_bActive);
	void SetActiveFromSceneMgr(bool i_bActive);

	//--------------------------------------------------------------------
	// SetScale and SetRenderable are special because of the way the 
	// gpxIconSet proxy keeps track of the icon layers scale and
	// renderable flags. So, make these virtual and let the icon
	// set proxy adjust the layers also.
	//--------------------------------------------------------------------
	virtual void  SetScale(const maVector3d& i_Scale);
	virtual void  SetRenderable(bool i_Renderable);

	//--------------------------------------------------------------------
	//	Get functions just return the data internally based on
	//	the "Set" calls earlier.
	//--------------------------------------------------------------------
	maPoint3d GetPosition() const;
	maRotation GetOrientation() const;
	maVector3d GetScale() const;
	maPoint3d GetPivotPoint() const;
	maVector3d GetPivotCompensation() const;
	bool GetRenderable() const;
	bool GetWireframe() const;
	bool GetLowResolution() const;
	bool GetGPUPickable() const;
	bool GetPickHull() const;

	//--------------------------------------------------------------------
	// Get transformation for the root node of the given object.
	// This is the matrix for the position, scale, rotation,
	// along with pivots stored in this object.
	//--------------------------------------------------------------------
	void GetTransformation(maMatrix4x4& o_Transformation);

	//--------------------------------------------------------------------
	// GetWorldBox returns the bounding box from the last time
	//   "Update" was called.
	//--------------------------------------------------------------------
	const maAxisBox& GetWorldBox() const;

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	api3dObject &m_Object;

#if USE_PROXIES
	maPoint3d m_Position;
	maRotation m_Rotation;
	maVector3d m_Scale;
	bool m_bRenderable;
	bool m_bActiveInSceneMgr;
	bool m_bActiveInRenderLayer;
	bool m_bWireframe;	
	bool m_bLowResolution;
	bool m_bPickable;
	bool m_bPickHull;
	bool m_bInheritsTransform;

	// world box from the last update
	mutable maAxisBox m_WorldBox;

	// Pivot is expensive to set, so track a separate bit for this
	bool m_bPivotSet;
	maPoint3d m_PivotPoint; 
	maVector3d m_PivotCompensation;

	// Color is expensive to set, so track a separate bit for this
	bool m_bColorSet;
	maFloatRGBA m_Color;
#endif
};