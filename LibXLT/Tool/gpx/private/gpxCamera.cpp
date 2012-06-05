/*****************************************************************************
**	gpxCamera.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxCamera.hpp"

#include "Core/Env/envSTLHelpers.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to camera it will control
//--------------------------------------------------------------------
gpxCamera::gpxCamera(camCamera &i_Camera)
:	m_Camera(i_Camera)
{
#if USE_PROXIES
	m_Position = i_Camera.GetPosition();
	m_Target = i_Camera.GetTarget();
	m_Up = i_Camera.GetUp();
	i_Camera.GetCameraMatrix( m_CameraMatrix );
	m_bOrthographic = i_Camera.IsOrthographic();
	m_Aspect = i_Camera.GetAspect();
	m_FOV = i_Camera.GetFOV();
	m_NearClip = i_Camera.GetNearClip();
	m_FarClip = i_Camera.GetFarClip();
	m_OrthoWidth = i_Camera.GetOrthoWidth();

	i_Camera.GetDOFParams(m_DOFData);
	i_Camera.GetHDRParams(m_HDRData);
	i_Camera.GetPassBuffersParams(m_PassBuffersData);

	m_StereoFD = i_Camera.GetStereoFD();
	m_StereoFilterColor = i_Camera.GetStereoFilterColor();
	m_StereoType = i_Camera.GetStereoType();
	m_StereoIOD = i_Camera.GetStereoIOD();
	m_StereoProjection = i_Camera.GetStereoProjection();
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxCamera::~gpxCamera()
{
	PROXY_REMOVE();
}

//--------------------------------------------------------------------
// Access to camera being proxied
//--------------------------------------------------------------------
camCamera&	gpxCamera::GetCamera()
{
	return m_Camera;
}

//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
bool gpxCamera::LookAt(const maPoint3d &i_Position, 
			const maPoint3d &i_Target,
			const maVector3d &i_Up)
{
#if USE_PROXIES
	// Check for any real differences before marking dirty
	if ( ((m_Position - i_Position).LengthSqr() > maConstants::c_fEpsilon) ||
		 ((m_Target - i_Target).LengthSqr() > maConstants::c_fEpsilon) ||
		 ((m_Up - i_Up).LengthSqr() > maConstants::c_fEpsilon ) )
	{
		m_Position = i_Position;
		m_Target = i_Target;
		m_Up = i_Up;

		camCamera::ConstructMatrixLH(i_Position, i_Target, i_Up, m_CameraMatrix);
	
		this->SetNeedsUpdate(true);
		return true;
	}
	return false;
#else
	m_Camera.LookAt( i_Position, i_Target, i_Up );
	return true;
#endif

}
void  gpxCamera::SetOrthographic(bool i_bOrthographic)
{
	PROXY_SET_OR_STORE(m_Camera, SetOrthographic, m_bOrthographic, i_bOrthographic);
}
void gpxCamera::SetFOV(float i_Degrees)
{
	PROXY_SET_OR_STORE(m_Camera, SetFOV, m_FOV, i_Degrees);
}
void gpxCamera::SetAspect(float i_Aspect)
{
	PROXY_SET_OR_STORE(m_Camera, SetAspect, m_Aspect, i_Aspect);
}
void gpxCamera::SetClip(float i_Near, float i_Far)
{
#if USE_PROXIES
	m_NearClip = i_Near;
	m_FarClip = i_Far;
	this->SetNeedsUpdate(true);
#else
	m_Camera.SetClip( i_Near, i_Far );
#endif
}
void gpxCamera::SetDOFParams(const camDOFData& i_DOFData)
{
	PROXY_SET_OR_STORE(m_Camera, SetDOFParams, m_DOFData, i_DOFData);
}
void gpxCamera::SetHDRParams(const camHDRData& i_HDRData)
{
	PROXY_SET_OR_STORE(m_Camera, SetHDRParams, m_HDRData, i_HDRData);
}
void gpxCamera::SetPassBuffersParams(const camPassBuffersData& i_PassBuffersData)
{
	PROXY_SET_OR_STORE(m_Camera, SetPassBuffersParams, m_PassBuffersData, i_PassBuffersData);
}
void gpxCamera::SetOrthoWidth(float i_Width)
{
	PROXY_SET_OR_STORE(m_Camera, SetOrthoWidth, m_OrthoWidth, i_Width);
}
void gpxCamera::SetStereoFD(float i_StereoFD)
{
	PROXY_SET_OR_STORE(m_Camera, SetStereoFD, m_StereoFD, i_StereoFD);
}
void gpxCamera::SetStereoFilterColor(int i_Color)
{
	PROXY_SET_OR_STORE(m_Camera, SetStereoFilterColor, m_StereoFilterColor, i_Color);
}
void gpxCamera::SetStereoType(int i_Type)
{
	PROXY_SET_OR_STORE(m_Camera, SetStereoType, m_StereoType, i_Type);
}
void gpxCamera::SetStereoIOD(float i_StereoIOD)
{
	PROXY_SET_OR_STORE(m_Camera, SetStereoIOD, m_StereoIOD, i_StereoIOD);
}
void gpxCamera::SetStereoProjection(int i_Projection)
{
	PROXY_SET_OR_STORE(m_Camera, SetStereoProjection, m_StereoProjection, i_Projection);
}
void gpxCamera::SetEnableALP(bool i_bEnableALP)
{
	PROXY_SET_OR_STORE(m_Camera, SetEnableALP, m_bEnableALP, i_bEnableALP);
}
void gpxCamera::SetFStop(float i_FStop)
{
	PROXY_SET_OR_STORE(m_Camera, SetFStop, m_FStop, i_FStop);
}
void gpxCamera::SetFocalDistance(float i_FocalDistance)
{
	PROXY_SET_OR_STORE(m_Camera, SetFocalDistance, m_FocalDistance, i_FocalDistance);
}
void gpxCamera::SetFocalLength(float i_FocalLength)
{
	PROXY_SET_OR_STORE(m_Camera, SetFocalLength, m_FocalLength, i_FocalLength);
}

//--------------------------------------------------------------------
//	Get functions just return the data internally based on
//	the "Set" calls earlier.
//--------------------------------------------------------------------
void gpxCamera::GetHDRParams(camHDRData& o_HDRData) const
{
#if USE_PROXIES
	o_HDRData = m_HDRData;
#else
	m_Camera.GetHDRParams(o_HDRData);
#endif
}
void gpxCamera::GetPassBuffersParams(camPassBuffersData& o_PassBuffersData) const
{
#if USE_PROXIES
	o_PassBuffersData = m_PassBuffersData;
#else
	m_Camera.GetPassBuffersParams(o_PassBuffersData);
#endif
	///o_PassBuffersData = PROXY_GET(m_Camera, GetPassBuffersParams, m_PassBuffersData);
}
const maPoint3d& gpxCamera::GetPosition() const
{
	return PROXY_GET(m_Camera, GetPosition, m_Position);
}
const maPoint3d& gpxCamera::GetTarget() const
{
	return PROXY_GET(m_Camera, GetTarget, m_Target);
}
maVector3d gpxCamera::GetDirection() const
{
#if USE_PROXIES
	return maPoint3d(m_CameraMatrix(0, 2), m_CameraMatrix(1, 2), m_CameraMatrix(2, 2));
#else
	return m_Camera.GetDirection();
#endif
}
maVector3d gpxCamera::GetLeft() const
{
#if USE_PROXIES
	return maVector3d(m_CameraMatrix(0, 0), m_CameraMatrix(1, 0), m_CameraMatrix(2, 0));
#else
	return m_Camera.GetLeft();
#endif
}
maVector3d gpxCamera::GetUp() const
{
#if USE_PROXIES
	return maVector3d(m_CameraMatrix(0, 1), m_CameraMatrix(1, 1), m_CameraMatrix(2, 1));
#else
	return m_Camera.GetUp();
#endif
}
float gpxCamera::GetFOV() const
{
	return PROXY_GET(m_Camera, GetFOV, m_FOV);
}
float gpxCamera::GetAspect() const
{
	return PROXY_GET(m_Camera, GetAspect, m_Aspect);
}
float gpxCamera::GetFarClip() const
{
	return PROXY_GET(m_Camera, GetFarClip, m_FarClip);
}
float gpxCamera::GetNearClip() const
{
	return PROXY_GET(m_Camera, GetNearClip, m_NearClip);
}
bool gpxCamera::IsOrthographic() const
{
	// Not buffered here because no Set function is exposed...
	return m_Camera.IsOrthographic();
}
float gpxCamera::GetOrthoWidth() const
{
	return PROXY_GET(m_Camera, GetOrthoWidth, m_OrthoWidth);
}

//--------------------------------------------------------------------
// Set is needed to push changes through to matrices.
// I am not sure how to handle this with proxies yet.
//--------------------------------------------------------------------
void gpxCamera::Set() const
{
#if USE_PROXIES
	// Not sure what to do with proxies,
	// maybe we have to call Set() in Update() always?
#else
	m_Camera.Set();
#endif
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxCamera::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	m_Camera.LookAt( m_Position, m_Target, m_Up );
	m_Camera.SetOrthographic( m_bOrthographic );
	m_Camera.SetAspect( m_Aspect );
	m_Camera.SetFOV( m_FOV );
	m_Camera.SetClip( m_NearClip, m_FarClip );
	m_Camera.SetOrthoWidth( m_OrthoWidth );

	m_Camera.SetDOFParams( m_DOFData );
	m_Camera.SetHDRParams( m_HDRData );
	m_Camera.SetPassBuffersParams( m_PassBuffersData );

	m_Camera.SetStereoFD( m_StereoFD );
	m_Camera.SetStereoFilterColor( m_StereoFilterColor );
	m_Camera.SetStereoType( m_StereoType );
	m_Camera.SetStereoIOD( m_StereoIOD );
	m_Camera.SetStereoProjection( m_StereoProjection );

	// need this to flush changes into matrices?
	m_Camera.Set();

	this->SetNeedsUpdate(false);
#endif

	return true;
}

//--------------------------------------------------------------------
// Add/Remove notification callback for when camera has been moved.
// Callback pointer is not owned by this camera.
//--------------------------------------------------------------------
void gpxCamera::AddCameraChangedCallback(CameraChangedCallback* i_pCallback)
{
	m_Callbacks.push_back(i_pCallback);
}
void gpxCamera::RemoveCameraChangedCallback(CameraChangedCallback* i_pCallback)
{
	envSTLHelpers::RemoveOneValue(m_Callbacks, i_pCallback);
}

//--------------------------------------------------------------------
// Remove all callbacks. 
//--------------------------------------------------------------------
void gpxCamera::ClearCallbacks()
{
	m_Callbacks.clear();
}


//--------------------------------------------------------------------
// Camera manipulators should call this function when altering
//	the camera matrix of the camera proxy.
//--------------------------------------------------------------------
void gpxCamera::NotifyCallbacks() const
{
	std::vector<CameraChangedCallback*>::iterator it, end = m_Callbacks.end();
	for (it = m_Callbacks.begin(); it != end; ++it)
	{
		(*it)->CameraChanged(this);
	}
}
