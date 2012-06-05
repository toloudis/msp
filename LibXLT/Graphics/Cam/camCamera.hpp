/*****************************************************************************
**  camCamera.hpp
**
**      camCamera is a camera class which can be used to manipulate the
**	Terawatt projection and view matrices.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CAM_CAMERA_HPP
#error camCamera.hpp multiply included
#endif
#define CAM_CAMERA_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif

#include <vector>

class matTexture;

//============================================================================
//============================================================================
struct camPassBuffersData
{
	camPassBuffersData();

	matTexture*	m_AOBuffer;
	float		m_AOIntensity;
	int			m_AOBlendOp;

	matTexture*	m_GIBuffer;
	float		m_GIIntensity;
	int			m_GIBlendOp;

	matTexture*	m_ReflBuffer;
	float		m_ReflIntensity;
	int			m_ReflBlendOp;

	matTexture*	m_ShadowMaskBuffer;
	float		m_ShadowMaskIntensity;
	int			m_ShadowMaskBlendOp;

	matTexture*	m_BeautyBuffer;
	float		m_BeautyIntensity;
	int			m_BeautyBlendOp;

};

struct camHDRData
{
	camHDRData();

	// HDR middle gray level
	float m_MiddleGray;
	// HDR Bloom Scale
	float m_BloomScale;
	// HDR Star Scale
	float m_StarScale;
	// HDR Bright Pass Threshold
	float m_BrightPassThresh;
	// HDR Bright Pass Offset
	float m_BrightPassOffset;
	// HDR White Cutoff
	float m_WhiteCutoff;
	// HDR Star Type
	int m_StarType;

	float m_SceneLuminance;

};

//============================================================================
//============================================================================
struct camDOFData
{
	camDOFData();

	// distances in camera space units
	float m_NearBlurDist;
	float m_NearFocalDist;
	float m_FarFocalDist;
	float m_FarBlurDist;
	// blur amount 0..1
	float m_MaxFarBlur;
	float m_MaxCoC;
	float m_bEnableDOF;
};


//============================================================================
//============================================================================
class camCamera
{
	public:
		//--------------------------------------------------------------------
		//	The constructor makes a camera facing towards positive z with
		//	a 90 degree FOV and 4/3 aspect ratio.
		//--------------------------------------------------------------------
		camCamera();

		//--------------------------------------------------------------------
		// copy constructor
		//--------------------------------------------------------------------
		camCamera(const camCamera& i_Copy);

		//--------------------------------------------------------------------
		// base destructor
		//--------------------------------------------------------------------
		~camCamera();

		//--------------------------------------------------------------------
		//	Set should be called when you are done setting up the camera and
		//	you are ready for the camCamera to set the Terawatt transforms.
		//--------------------------------------------------------------------
		void Set() const;

		//--------------------------------------------------------------------
		//	SetFOV changes the (horizontal) Field Of View of the camera.
		//	Usually Terawatt prefers radian values for angular quantities
		//	but people usually visualize this quantity in degrees.
		//--------------------------------------------------------------------
		void SetFOV(float i_Degrees);

		//--------------------------------------------------------------------
		//	SetAspect changes the aspect ratio of the camera projection.  This
		//	value is equal to the width of the visible area at some fixed
		//	distance from the camera divided by the height at the same
		//	distance.
		//--------------------------------------------------------------------
		void SetAspect(float i_Aspect);

		//--------------------------------------------------------------------
		// SetAspect sets aspect ratio for window with given dimensions
		//--------------------------------------------------------------------
		void SetAspect( int i_Width, int i_Height );

		//--------------------------------------------------------------------
		//	SetClip sets the locations of the near and far clip planes.
		//--------------------------------------------------------------------
		void SetClip(float i_Near, float i_Far);

		//--------------------------------------------------------------------
		//	SetDOFParams sets information used by the depth of field effect
		//--------------------------------------------------------------------
		void SetDOFParams(const camDOFData& i_DOFData);

		//--------------------------------------------------------------------
		//	GetDOFParams returns information used by the depth of field effect
		//--------------------------------------------------------------------
		void GetDOFParams(camDOFData& o_DOFData) const;

		//--------------------------------------------------------------------
		//	SetHDRParams sets data used by the high dynamic range renderer
		//--------------------------------------------------------------------
		void SetHDRParams(const camHDRData& i_HDRData);

		//--------------------------------------------------------------------
		//	GetHDRParams returns data used by the high dynamic range renderer
		//--------------------------------------------------------------------
		void GetHDRParams(camHDRData& o_HDRData) const;

		//--------------------------------------------------------------------
		//	SetHDRParams sets data used by the high dynamic range renderer
		//--------------------------------------------------------------------
		void SetPassBuffersParams(const camPassBuffersData& i_PassBuffersData);

		//--------------------------------------------------------------------
		//	GetHDRParams returns data used by the high dynamic range renderer
		//--------------------------------------------------------------------
		void GetPassBuffersParams(camPassBuffersData& o_PassBuffersData) const;

		//--------------------------------------------------------------------
		//	GetPosition returns the position of the camCamera
		//--------------------------------------------------------------------
		const maPoint3d& GetPosition() const;

		//--------------------------------------------------------------------
		//	GetTarget returns the target position of the camCamera
		//--------------------------------------------------------------------
		const maPoint3d& GetTarget() const;

		//--------------------------------------------------------------------
		//	GetDirection returns the direction that the camera is facing.
		//--------------------------------------------------------------------
		maVector3d GetDirection() const;

		//--------------------------------------------------------------------
		//	GetLeft returns a normalized vector representing the left of the
		//	camera.
		//--------------------------------------------------------------------
		maVector3d GetLeft() const;

		//--------------------------------------------------------------------
		//	GetUp returns a normalized vector representing the upwards
		//	direction of the camera.
		//--------------------------------------------------------------------
		maVector3d GetUp() const;

		//--------------------------------------------------------------------
		//	GetFOV returns the field of view of the camera, in degrees.
		//--------------------------------------------------------------------
		float GetFOV() const;

		//--------------------------------------------------------------------
		//	GetAspect returns the aspect ratio.
		//--------------------------------------------------------------------
		float GetAspect() const;

		//--------------------------------------------------------------------
		//	GetNearClip gets the distance from the camera position to the
		//	near clip plane.
		//--------------------------------------------------------------------
		float GetNearClip() const;

		//--------------------------------------------------------------------
		//	GetFarClip gets the distance from the camera position to the far
		//	clip plane.
		//--------------------------------------------------------------------
		float GetFarClip() const;
		
		//--------------------------------------------------------------------
		// Camera pointing down +z direction in view space (D3D-like)
		//--------------------------------------------------------------------
		static void ConstructMatrixLH(const maPoint3d &i_Pos, 
							   const maPoint3d &i_Target,
							   const maVector3d &i_Up,
							   maMatrix4x4& o_Matrix);
		//--------------------------------------------------------------------
		// Camera pointing down -z direction in view space (OpenGL-like)
		//--------------------------------------------------------------------
		static void ConstructMatrixRH(const maPoint3d &i_Pos, 
					   const maPoint3d &i_Target,
					   const maVector3d &i_Up,
					   maMatrix4x4& o_Matrix,
					   bool i_bTranslate = true);

		//--------------------------------------------------------------------
		//	LookAt sets a camera matrix from position, target and up vector
		//--------------------------------------------------------------------
		void LookAt(const maPoint3d &i_Pos, const maPoint3d &i_Target,
						 const maVector3d &i_Up);

		//--------------------------------------------------------------------
		//	SetPosition sets the position of the camCamera
		//--------------------------------------------------------------------
		void SetPosition( maPoint3d newPos );

		//--------------------------------------------------------------------
		//	SetCameraMatrix sets a camera matrix
		//--------------------------------------------------------------------
		void SetCameraMatrix(maMatrix4x4& o_Matrix);

		//--------------------------------------------------------------------
		//	GetCameraMatrix sets a camera matrix
		//--------------------------------------------------------------------
		void GetCameraMatrix(maMatrix4x4& o_Matrix) const;

		//--------------------------------------------------------------------
		//	GetProjectionMatrix sets a projection matrix
		//--------------------------------------------------------------------
		void GetProjectionMatrix(maMatrix4x4& o_Matrix) const;

		//--------------------------------------------------------------------
		//	GetScreenPoint gets the point in world space corresponding to
		//	the given point in (normalized) screen space.  The screen
		//	space point should be in the range x: [-1, 1], y: [-1, 1].
		//--------------------------------------------------------------------
		maPoint3d GetScreenPoint(float i_X, float i_Y) const;

		//====================================================================
		// Setup projection matrix to render part of normalized
		// view volume.  This can be used to break up a large
		// render into smaller areas.
		// The arguments should be numbers between -1.0 and 1.0
		//====================================================================
		void SetSubViewport(float i_Top, float i_Bottom,
							float i_Left, float i_Right);
		void GetSubViewport(float& o_Top, float& o_Bottom,
							float& o_Left, float& o_Right) const;

		//--------------------------------------------------------------------
		// Horizontal Film Offset shifts the viewport an absolute amount
		// left or right in order to accomplish an off-axis stereo projection.
		//--------------------------------------------------------------------
		void SetHorizontalFilmOffset(float i_FilmOffset);
		float GetHorizontalFilmOffset() const;

		//--------------------------------------------------------------------
		//	Set whether this camera is perspective or orthographic
		//--------------------------------------------------------------------
		void SetOrthographic(bool i_bOrtho);
		bool IsOrthographic() const;

		//--------------------------------------------------------------------
		// Orthographic width controls the size of the orthographic 
		//	view plane
		//--------------------------------------------------------------------
		void SetOrthoWidth(float i_Width);
		float GetOrthoWidth() const;

		//----------------------------------------------------------------------------
		// If this flag is set to true, then the aspect ratio of the camera
		//	will be set to match the window's width and height.
		//	It is FALSE by default.
		//----------------------------------------------------------------------------
		bool GetMatchAspectToWindow() const;
		void SetMatchAspectToWindow(bool i_bMatch);

		//--------------------------------------------------------------------
		// This function is needed to satisfy the camCaameraManipTarget 
		// interface, but the callbacks have been removed from this class,
		// so the function is empty now.
		//--------------------------------------------------------------------
		void NotifyCallbacks() {}

		//--------------------------------------------------------------------
		// SetName()
		//--------------------------------------------------------------------
		void SetName( const std::string &i_Name );

		//--------------------------------------------------------------------
		// GetName()
		//--------------------------------------------------------------------
		std::string GetName();

		//--------------------------------------------------------------------
		// SetEnableALP()
		//--------------------------------------------------------------------
		void SetEnableALP(bool i_bEnableALP);

		//--------------------------------------------------------------------
		// SetFStop()
		//--------------------------------------------------------------------
		void SetFStop(float i_FStop);

		//--------------------------------------------------------------------
		// SetFocalDistance()
		//--------------------------------------------------------------------
		void SetFocalDistance(float i_FocalDistance);

		//--------------------------------------------------------------------
		// SetFocalLength()
		//--------------------------------------------------------------------
		void SetFocalLength(float i_FocalLength);

		//--------------------------------------------------------------------
		// GetEnableALP()
		//--------------------------------------------------------------------
		bool GetEnableALP();

		//--------------------------------------------------------------------
		// GetFStop()
		//--------------------------------------------------------------------
		float GetFStop();

		//--------------------------------------------------------------------
		// GetFocalDistance()
		//--------------------------------------------------------------------
		float GetFocalDistance();

		//--------------------------------------------------------------------
		// GetFocalLength()
		//--------------------------------------------------------------------
		float GetFocalLength();

		//--------------------------------------------------------------------
		// Stereo accessors and modifiers
		//--------------------------------------------------------------------
		void SetStereoFD(float stereoFD);
		void SetStereoFilterColor(int color);
		void SetStereoType(int color);
		void SetStereoIOD(float StereoIOD);
		void SetStereoProjection(int projection);

		float GetStereoFD();
		int GetStereoFilterColor();
		int GetStereoType();
		int GetStereoProjection();
		float GetStereoIOD();		//Inter Ocular Distance (spacing between the eyes)

	private:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void make_projection() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void make_camera() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void make_yaw_pitch();

		float m_FOV;		// degrees
		float m_Aspect;
		bool m_bMatchAspectToWindow;
		float m_Near;
		float m_Far;
		mutable bool m_ProjectionDirty;
		mutable maMatrix4x4 m_Projection;
		float m_ScreenXMin, m_ScreenXMax, m_ScreenYMin, m_ScreenYMax;
		float m_HorizontalFilmOffset;
		maMatrix4x4 m_Camera;
		maPoint3d m_Position;
		maPoint3d m_Target;
		maPoint3d m_Up;
		std::string m_Name;

		// orthographic camera settings
		bool m_bOrthographic;
		float m_OrthoWidth;

		float m_StereoFD;
		int m_StereoFilterColor;
		int m_StereoType;
		int m_StereoProjection;
		float m_StereoIOD;

		bool m_bEnableALP;
		float m_FStop;
		float m_FocalDistance;
		float m_FocalLength;

		camDOFData m_DOFData;

		camHDRData m_HDRData;

		camPassBuffersData m_PassBuffersData;
};
