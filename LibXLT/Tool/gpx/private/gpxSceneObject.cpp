/*****************************************************************************
**	gpxSceneObject.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxSceneObject.hpp"

#include "Graphics/G3d/g3dThreadControl.hpp"
#include "Graphics/Sc/scObject.hpp"
#include "Tool/api3d/api3dObject.hpp"
#include "Tool/gpx/gpxProxyMgr.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxSceneObject::gpxSceneObject(api3dObject &i_Object)
:	m_Object(i_Object)
{
#if USE_PROXIES
	m_Position = i_Object.GetPosition();
	m_Rotation = i_Object.GetOrientation();
	m_Scale = i_Object.GetScale();
	m_bRenderable = i_Object.GetRenderable();
	m_bActiveInRenderLayer = i_Object.GetActiveInRenderLayer();
	m_bActiveInSceneMgr = i_Object.GetActiveInSceneMgr();
	m_bWireframe = i_Object.GetWireframe();
	m_bLowResolution = i_Object.GetLowResolution();
	m_bPickable = i_Object.GetGPUPickable();
	m_bPickHull = i_Object.GetPickHull();
	m_bInheritsTransform = i_Object.GetInheritsTransform();
	m_bPivotSet = false;
	m_PivotPoint = i_Object.GetPivotPoint();
	m_PivotCompensation = i_Object.GetPivotCompensation();
	m_Color.Set(0,0,0,1);
	m_bColorSet = false;
	m_WorldBox = m_Object.GetWorldBox();
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxSceneObject::~gpxSceneObject()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxSceneObject::SetPosition(const maPoint3d &i_Position)
{
	PROXY_SET_OR_STORE(m_Object, SetPosition, m_Position, i_Position);
}
void gpxSceneObject::SetOrientation(const maRotation &i_Rotation)
{
	PROXY_SET_OR_STORE(m_Object, SetOrientation, m_Rotation, i_Rotation);
}
void gpxSceneObject::SetUniformScale(const float i_fScale)
{
	this->SetScale( maVector3d(i_fScale, i_fScale, i_fScale) );
}
void gpxSceneObject::SetPivotPoint(const maPoint3d &i_Position, 
					   bool i_bPreserveTransformation)
{
	// Since setting the pivot point also alters the pivot compensation,
	// we have to write this Set block out.

#if USE_PROXIES
	// Compute how much the pivot is moving, so that we can compensate
	//	when preserving position
	maVector3d offset = i_Position - m_PivotPoint;
	m_PivotPoint = i_Position;
	if (i_bPreserveTransformation)
	{	
		maMatrix4x4 total_xform;
		scObject::ComputeFullTransformation(m_PivotPoint, m_PivotCompensation,
										m_Position, m_Scale, m_Rotation,
										total_xform);

		// Need to push the delta in the pivot point through the matrix
		// and maintain it in a compensation translation at the end
		maVector3d xformed_offset(offset);
		total_xform.TransformDir( xformed_offset );
		m_PivotCompensation += (xformed_offset - offset);
	}
	else
	{
		// Not preserving transformation
		m_PivotCompensation.Set(0,0,0); // remove old compensation
	}

	m_bPivotSet = true;
	this->SetNeedsUpdate(true);
#else
	m_Object.SetPivotPoint( i_Position, i_bPreserveTransformation );
#endif
}
void gpxSceneObject::SetPivotCompensation(const maVector3d &i_Compensation)
{
	PROXY_SET_OR_STORE(m_Object, SetPivotCompensation, m_PivotCompensation, i_Compensation);
}
void gpxSceneObject::SetWireframe(bool i_Wireframe)
{
	PROXY_SET_OR_STORE(m_Object, SetWireframe, m_bWireframe, i_Wireframe);
}	
void gpxSceneObject::SetLowResolution(bool i_LowResolution)
{
	PROXY_SET_OR_STORE(m_Object, SetLowResolution, m_bLowResolution, i_LowResolution);
}
void gpxSceneObject::SetGPUPickable(bool i_Pickable)
{
	PROXY_SET_OR_STORE(m_Object, SetGPUPickable, m_bPickable, i_Pickable);
}
void gpxSceneObject::SetPickHull(bool i_PickHull)
{
	PROXY_SET_OR_STORE(m_Object, SetPickHull, m_bPickHull, i_PickHull);
}
void gpxSceneObject::SetInheritsTransform(bool i_bInherits)
{
	PROXY_SET_OR_STORE(m_Object, SetInheritsTransform, m_bInheritsTransform, i_bInherits);
}
void gpxSceneObject::SetColor(const maFloatRGBA &i_Color)
{
	PROXY_SET_OR_STORE(m_Object, SetColor, m_Color, i_Color);

#if USE_PROXIES
	// Color is expensive to set, so use a separate dirty bit for it.
	m_bColorSet = true;
#endif
}
void gpxSceneObject::SetActiveFromRenderLayer(bool i_bActive)
{
	PROXY_SET_OR_STORE(m_Object, SetActiveFromRenderLayer, m_bActiveInRenderLayer, i_bActive);
}
void gpxSceneObject::SetActiveFromSceneMgr(bool i_bActive)
{
	PROXY_SET_OR_STORE(m_Object, SetActiveFromSceneMgr, m_bActiveInSceneMgr, i_bActive);
}
void gpxSceneObject::SetScale(const maVector3d& i_Scale)
{
	PROXY_SET_OR_STORE(m_Object, SetScale, m_Scale, i_Scale);
}
void gpxSceneObject::SetRenderable(bool i_Renderable)
{
	PROXY_SET_OR_STORE(m_Object, SetRenderable, m_bRenderable, i_Renderable);
}

//--------------------------------------------------------------------
//	Get functions just return the data internally based on
//	the "Set" calls earlier.
//--------------------------------------------------------------------
maPoint3d gpxSceneObject::GetPosition() const
{
	return PROXY_GET(m_Object, GetPosition, m_Position);
}
maRotation gpxSceneObject::GetOrientation() const
{
	return PROXY_GET(m_Object, GetOrientation, m_Rotation);
}
maVector3d gpxSceneObject::GetScale() const
{
	return PROXY_GET(m_Object, GetScale, m_Scale);
}
maPoint3d gpxSceneObject::GetPivotPoint() const
{
	return PROXY_GET(m_Object, GetPivotPoint, m_PivotPoint);
}
maVector3d gpxSceneObject::GetPivotCompensation() const
{
	return PROXY_GET(m_Object, GetPivotCompensation, m_PivotCompensation);
}
bool gpxSceneObject::GetRenderable() const
{
	return PROXY_GET(m_Object, GetRenderable, m_bRenderable);
}
bool gpxSceneObject::GetWireframe() const
{
	return PROXY_GET(m_Object, GetWireframe, m_bWireframe);
}
bool gpxSceneObject::GetLowResolution() const
{
	return PROXY_GET(m_Object, GetLowResolution, m_bLowResolution);
}
bool gpxSceneObject::GetGPUPickable() const
{
	return PROXY_GET(m_Object, GetGPUPickable, m_bPickable);
}
bool gpxSceneObject::GetPickHull() const
{
	return PROXY_GET(m_Object, GetPickHull, m_bPickHull);
}

//--------------------------------------------------------------------
// Get transformation for the root node of the given object.
// This is the matrix for the position, scale, rotation,
// along with pivots stored in this object.
//--------------------------------------------------------------------
void gpxSceneObject::GetTransformation(maMatrix4x4& o_Transformation)
{
#if USE_PROXIES
	scObject::ComputeFullTransformation(m_PivotPoint, m_PivotCompensation,
										m_Position, m_Scale, m_Rotation,
										o_Transformation);
#else
	m_Object->GetTransformation(o_Transformation);
#endif
}

//--------------------------------------------------------------------
// GetWorldBox returns the bounding box from the last time
//   "Update" was called.
//--------------------------------------------------------------------
const maAxisBox& gpxSceneObject::GetWorldBox() const
{
#if USE_PROXIES
	//bga - this is the big hack that is holding the multi-threaded
	// version together right now. A correct way to do this
	// is to proxy the entire scene graph and run all animations on that
	// scene graph and then push only the transforms through to the
	// rendering scene graph copy. 
	// This hack prevents conflicts where two threads alter the
	// total transform and bboxs of the scene graph and 
	// allows the capture mode to be correct, but
	// makes the interactive version one frame behind.
	//
	if (g3dThreadControl::IsRenderThreadActive())
	{
		// This is not going to be exactly right, it is going to be
		// the world box from the last update, which will not
		// be the current one if the transformation has changed.
		return m_WorldBox;
	}
	else
	{
		m_WorldBox = m_Object.GetWorldBox();
		return m_WorldBox;
	}
#else
	return m_Object.GetWorldBox();
#endif
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxSceneObject::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	m_Object.SetPosition( m_Position );
	m_Object.SetOrientation( m_Rotation );
	m_Object.SetScale( m_Scale );
	if (m_bPivotSet)
	{
		m_Object.SetPivotPoint( m_PivotPoint );
		m_Object.SetPivotCompensation( m_PivotCompensation );
		m_bPivotSet = false;
	}
	m_Object.SetRenderable( m_bRenderable );
	m_Object.SetActiveInRenderLayer( m_bActiveInRenderLayer );
	m_Object.SetActiveInSceneMgr( m_bActiveInSceneMgr );
	m_Object.SetWireframe( m_bWireframe );
	m_Object.SetLowResolution( m_bLowResolution );
	m_Object.SetGPUPickable( m_bPickable );
	m_Object.SetPickHull( m_bPickHull );
	m_Object.SetInheritsTransform( m_bInheritsTransform );
	if (m_bColorSet)
	{
		m_Object.SetColor( m_Color );
		m_bColorSet = false;
	}

	// store the world box in order to return it later
	m_WorldBox = m_Object.GetWorldBox();

	this->SetNeedsUpdate(false);
#endif

	return true;
}