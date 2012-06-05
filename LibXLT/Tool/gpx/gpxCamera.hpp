/*****************************************************************************
**	gpxCamera.hpp
**
**	This class is a thread-safe proxy for a camCamera.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_CAMERA_HPP
#error gpxCamera.hpp multiply included
#endif
#define GPX_CAMERA_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 

#ifndef CAM_CAMERA_HPP
#include "Graphics/Cam/camCamera.hpp"
#endif 


//============================================================================
//============================================================================
class gpxCamera : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Callback for when manipulators change this camera
	//--------------------------------------------------------------------
	class CameraChangedCallback
	{
	public:
		virtual void CameraChanged(const gpxCamera*) = 0;
	};

	//--------------------------------------------------------------------
	// Constructor takes reference to camera it will control
	//--------------------------------------------------------------------
	explicit gpxCamera(camCamera &i_Camera);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxCamera();

	//--------------------------------------------------------------------
	// Access to camera being proxied
	//--------------------------------------------------------------------
	camCamera&	GetCamera();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the camera when "Update()" is called.
	//--------------------------------------------------------------------
	bool LookAt(const maPoint3d &i_Pos, 
				const maPoint3d &i_Target,
				const maVector3d &i_Up);
	void SetOrthographic(bool i_bOrthographic);
	void SetFOV(float i_Degrees);
	void SetAspect(float i_Aspect);
	//void SetAspect( int i_Width, int i_Height );
	void SetClip(float i_Near, float i_Far);
	void SetDOFParams(const camDOFData& i_DOFData);
	void SetHDRParams(const camHDRData& i_HDRData);
	void SetPassBuffersParams(const camPassBuffersData& i_PassBuffersData);	
	void SetOrthoWidth(float i_Width);
	void SetStereoFD(float i_StereoFD);
	void SetStereoFilterColor(int i_Color);
	void SetStereoType(int i_Type);
	void SetStereoIOD(float i_StereoIOD);
	void SetStereoProjection(int i_StereoProjection);
	void SetEnableALP(bool i_bEnableALP);
	void SetFStop(float i_FStop);
	void SetFocalDistance(float i_FocalDistance);
	void SetFocalLength(float i_FocalLength);

	//--------------------------------------------------------------------
	//	Get functions just return the data internally based on
	//	the "Set" calls earlier.
	//--------------------------------------------------------------------
	//void GetDOFParams(camDOFData& o_DOFData) const;
	void GetHDRParams(camHDRData& o_HDRData) const;
	void GetPassBuffersParams(camPassBuffersData& o_PassBuffersData) const;
	const maPoint3d& GetPosition() const;
	const maPoint3d& GetTarget() const;
	maVector3d GetDirection() const;
	maVector3d GetLeft() const;
	maVector3d GetUp() const;
	float GetFOV() const;
	float GetAspect() const;
	float GetNearClip() const;
	float GetFarClip() const;
	//void GetCameraMatrix(maMatrix4x4& o_Matrix) const;
	//void GetProjectionMatrix(maMatrix4x4& o_Matrix) const;
	//maPoint3d GetScreenPoint(float i_X, float i_Y) const;
	bool IsOrthographic() const;
	float GetOrthoWidth() const;
	//bool GetMatchAspectToWindow() const;

	//--------------------------------------------------------------------
	// Set is needed to push changes through to matrices.
	// I am not sure how to handle this with proxies yet.
	//--------------------------------------------------------------------
	void Set() const;

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

	//--------------------------------------------------------------------
	// Add/Remove notification callback for when camera has been moved.
	// Callback pointer is not owned by this camera.
	//--------------------------------------------------------------------
	void AddCameraChangedCallback(CameraChangedCallback* i_pCallback);
	void RemoveCameraChangedCallback(CameraChangedCallback* i_pCallback);

	//--------------------------------------------------------------------
	// Remove all callbacks. 
	//--------------------------------------------------------------------
	void ClearCallbacks();

	//--------------------------------------------------------------------
	// Camera manipulators should call this function when altering
	//	the camera matrix of the camera proxy.
	//--------------------------------------------------------------------
	void NotifyCallbacks() const;

private:
	camCamera &m_Camera;

	// Notify callbacks
	mutable std::vector<CameraChangedCallback*> m_Callbacks;

#if USE_PROXIES
	maPoint3d m_Position;
	maPoint3d m_Target;
	maVector3d m_Up;
	maMatrix4x4 m_CameraMatrix;
	bool m_bOrthographic;
	float m_Aspect;
	float m_FOV;
	float m_NearClip;
	float m_FarClip;
	float m_OrthoWidth;

	camDOFData m_DOFData;
	camHDRData m_HDRData;
	camPassBuffersData m_PassBuffersData;

	bool m_bEnableStereo;
	float m_StereoFD;
	int m_StereoFilterColor;
	int m_StereoType;
	int m_StereoProjection;
	float m_StereoIOD;

	bool m_bEnableALP;
	float m_FStop;
	float m_FocalDistance;
	float m_FocalLength;

#endif
};
