/*****************************************************************************
**  camCamera.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/cam/camCamera.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"

#include "Graphics/g3d/g3dPassBuffers.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
camPassBuffersData::camPassBuffersData()
:	m_AOBuffer(NULL),	
	m_GIBuffer(NULL),	
	m_ReflBuffer(NULL),	
	m_ShadowMaskBuffer(NULL),	
	m_BeautyBuffer(NULL),
	m_AOIntensity(1.0f),
	m_GIIntensity(1.0f),
	m_ReflIntensity(1.0f),
	m_ShadowMaskIntensity(1.0f),
	m_BeautyIntensity(1.0f),
	m_AOBlendOp(g3dPassBuffers::e_MUL),
	m_GIBlendOp(g3dPassBuffers::e_ADD),
	m_ReflBlendOp(g3dPassBuffers::e_ADD),
	m_ShadowMaskBlendOp(g3dPassBuffers::e_MUL),
	m_BeautyBlendOp(g3dPassBuffers::e_ADD)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
camHDRData::camHDRData()
:	m_MiddleGray(1.0f),	
	m_BloomScale(1.0f),	
	m_StarScale(0.5f),	
	m_BrightPassThresh(5.0f),	
	m_BrightPassOffset(10.0f),	
	m_WhiteCutoff(1.0f),
	m_StarType(0),
	m_SceneLuminance(1)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
camDOFData::camDOFData()
:	m_NearBlurDist(-1),
	m_NearFocalDist(-1),
	m_FarFocalDist(-1),
	m_FarBlurDist(-1),
	m_MaxFarBlur(-1),
	m_MaxCoC(-1),
	m_bEnableDOF(false)
{
}

//--------------------------------------------------------------------
//	The constructor makes a camera facing towards positive z with
//	a 90 degree FOV and 4/3 aspect ratio.
//--------------------------------------------------------------------
camCamera::camCamera()
:	m_FOV(90),
	m_Aspect(4.0f / 3.0f),
	m_bMatchAspectToWindow(false),
	m_ProjectionDirty(true),
	m_Near(1.0f),
	m_Far(10000.0f),
	m_Position(0, 0, 0),
	m_Target(0, 0, 1),
	m_Up(0,1,0),
	m_bOrthographic(false),
	m_OrthoWidth(1.0f),
	m_ScreenXMin(-1), m_ScreenXMax(1), m_ScreenYMin(-1), m_ScreenYMax(1),
	m_HorizontalFilmOffset(0),
	m_StereoFD(75),
	m_StereoFilterColor(),
	m_StereoType(),
	m_StereoProjection(1.0),
	m_StereoIOD(1.0f)
{
	LookAt(m_Position, m_Target, m_Up);
}

//--------------------------------------------------------------------
// copy constructor
//--------------------------------------------------------------------
camCamera::camCamera(const camCamera& i_Copy)
:	m_FOV(i_Copy.m_FOV),		// degrees
	m_Aspect(i_Copy.m_Aspect),
	m_bMatchAspectToWindow(i_Copy.m_bMatchAspectToWindow),
	m_Near(i_Copy.m_Near),
	m_Far(i_Copy.m_Far),
	m_ProjectionDirty(i_Copy.m_ProjectionDirty),
	m_Projection(i_Copy.m_Projection),
	m_ScreenXMin(i_Copy.m_ScreenXMin),
	m_ScreenXMax(i_Copy.m_ScreenXMax),
	m_ScreenYMin(i_Copy.m_ScreenYMin),
	m_ScreenYMax(i_Copy.m_ScreenYMax),
	m_HorizontalFilmOffset(i_Copy.m_HorizontalFilmOffset),
	m_Camera(i_Copy.m_Camera),
	m_Position(i_Copy.m_Position),
	m_Target(i_Copy.m_Target),
	m_Up(i_Copy.m_Up),
	m_bOrthographic(i_Copy.m_bOrthographic),
	m_OrthoWidth(i_Copy.m_OrthoWidth),
	m_DOFData(i_Copy.m_DOFData),
	m_HDRData(i_Copy.m_HDRData),
	//m_PassBuffersData(i_Copy.m_PassBuffersData),
	m_StereoFD(i_Copy.m_StereoFD),
	m_StereoFilterColor(i_Copy.m_StereoFilterColor),
	m_StereoType(i_Copy.m_StereoType),
	m_StereoProjection(i_Copy.m_StereoProjection),
	m_StereoIOD(i_Copy.m_StereoIOD)
{
	// note: didn't copy callbacks.
}

//--------------------------------------------------------------------
// virtual base destructor
//--------------------------------------------------------------------
//virtual
camCamera::~camCamera()
{
}

//--------------------------------------------------------------------
//	Set should be called when you are done setting up the camera and
//	you are ready for the camCamera to set the Terawatt transforms.
//--------------------------------------------------------------------
void camCamera::Set() const
{
	if( m_ProjectionDirty )
		this->make_projection();

//	camCamera::SetFOV(m_FOV);
//	camCamera::SetAspect(m_Aspect);
//	camCamera::SetClip(m_Near, m_Far);
//	camCamera::LookAt(m_Position, m_Target, m_Up);
}

//--------------------------------------------------------------------
//	SetFOV changes the (horizontal) Field Of View of the camera.
//	Usually Terawatt prefers radian values for angular quantities
//	but people usually visualize this quantity in degrees.
//--------------------------------------------------------------------
void camCamera::SetFOV(float i_Degrees)
{
	m_FOV = i_Degrees;
	m_ProjectionDirty = true;
}

//--------------------------------------------------------------------
//	SetAspect changes the aspect ratio of the camera projection.  This
//	value is equal to the width of the visible area at some fixed
//	distance from the camera divided by the height at the same
//	distance.
//--------------------------------------------------------------------
void camCamera::SetAspect(float i_Aspect)
{
	if (i_Aspect <= 0)
		i_Aspect = 1.0f;
	m_Aspect = i_Aspect;
	m_ProjectionDirty = true;
}

//--------------------------------------------------------------------
// SetAspect sets aspect ratio for window with given dimensions
//--------------------------------------------------------------------
void camCamera::SetAspect( int i_Width, int i_Height )
{
	float aspect = i_Width / (float)i_Height;
	this->SetAspect(aspect);
}

//--------------------------------------------------------------------
//	SetClip sets the locations of the near and far clip planes.
//--------------------------------------------------------------------
void camCamera::SetClip(float i_Near, float i_Far)
{
	if (i_Near == i_Far)
		i_Far = i_Near + 1.0f; // i_Near must not equal to i_Far
	m_Near = i_Near;
	m_Far = i_Far;
	m_ProjectionDirty = true;
}

//--------------------------------------------------------------------
//	GetPosition returns the position of the camCamera
//--------------------------------------------------------------------
const maPoint3d& camCamera::GetPosition() const
{
	return m_Position;
}

//--------------------------------------------------------------------
//	GetTarget returns the target position of the camCamera
//--------------------------------------------------------------------
const maPoint3d& camCamera::GetTarget() const
{
	return m_Target;
}

//--------------------------------------------------------------------
//	GetDirection returns the direction that the camera is facing.
//--------------------------------------------------------------------
maVector3d camCamera::GetDirection() const
{
	return maPoint3d(m_Camera(0, 2), m_Camera(1, 2), m_Camera(2, 2));
}

//--------------------------------------------------------------------
//	GetLeft returns a normalized vector representing the left of the
//	camera.
//--------------------------------------------------------------------
maVector3d camCamera::GetLeft() const
{
	return maVector3d(m_Camera(0, 0), m_Camera(1, 0), m_Camera(2, 0));
}

//--------------------------------------------------------------------
//	GetUp returns a normalized vector representing the upwards
//	direction of the camera.
//--------------------------------------------------------------------
maVector3d camCamera::GetUp() const
{
	return maVector3d(m_Camera(0, 1), m_Camera(1, 1), m_Camera(2, 1));
}

//--------------------------------------------------------------------
//	GetFOV returns the field of view of the camera, in degrees.
//--------------------------------------------------------------------
float camCamera::GetFOV() const
{
	return m_FOV;
}

//--------------------------------------------------------------------
//	GetAspect returns the aspect ratio.
//--------------------------------------------------------------------
float camCamera::GetAspect() const
{
	return m_Aspect;
}

//--------------------------------------------------------------------
//	GetNearClip gets the distance from the camera position to the
//	near clip plane.
//--------------------------------------------------------------------
float camCamera::GetNearClip() const
{
	return m_Near;
}

//--------------------------------------------------------------------
//	GetFarClip gets the distance from the camera position to the far
//	clip plane.
//--------------------------------------------------------------------
float camCamera::GetFarClip() const
{
	return m_Far;
}

//--------------------------------------------------------------------
//	LookAt sets a camera matrix from position, target and up vector
// Camera pointing down +z direction in view space
//--------------------------------------------------------------------
// static
void camCamera::ConstructMatrixLH(const maPoint3d &i_Pos, 
					   const maPoint3d &i_Target,
					   const maVector3d &i_Up,
					   maMatrix4x4& o_Matrix)
{
	// compare with d3dxmatrixlookatLH

	maVector3d dir = i_Target - i_Pos;
	dir.Normalize();
	maVector3d left = i_Up.Cross(dir);

	if (!left.Normalize())
		left.Set(1, 0, 0);

	maVector3d camera_up = dir.Cross(left);
	camera_up.Normalize();

	o_Matrix.Identity();

	o_Matrix(0, 0) = left.m_X;
	o_Matrix(1, 0) = left.m_Y;
	o_Matrix(2, 0) = left.m_Z;

	o_Matrix(0, 1) = camera_up.m_X;
	o_Matrix(1, 1) = camera_up.m_Y;
	o_Matrix(2, 1) = camera_up.m_Z;

	o_Matrix(0, 2) = dir.m_X;
	o_Matrix(1, 2) = dir.m_Y;
	o_Matrix(2, 2) = dir.m_Z;

	maMatrix4x4 translate;
	translate.MakeTranslate(-i_Pos.m_X, -i_Pos.m_Y, -i_Pos.m_Z);
	o_Matrix = translate * o_Matrix;
}

//--------------------------------------------------------------------
// Camera pointing down -z direction in view space
//--------------------------------------------------------------------
void camCamera::ConstructMatrixRH(const maPoint3d &i_Pos, 
					   const maPoint3d &i_Target,
					   const maVector3d &i_Up,
					   maMatrix4x4& o_Matrix,
					   bool i_bTranslate)
{
	// compare with d3dxmatrixlookatRH

	maVector3d dir = i_Pos - i_Target;
	dir.Normalize();
	maVector3d left = i_Up.Cross(dir);

	if (!left.Normalize())
		left.Set(1, 0, 0);

	maVector3d camera_up = dir.Cross(left);
	camera_up.Normalize();

	o_Matrix.Identity();

	o_Matrix(0, 0) = left.m_X;
	o_Matrix(1, 0) = left.m_Y;
	o_Matrix(2, 0) = left.m_Z;

	o_Matrix(0, 1) = camera_up.m_X;
	o_Matrix(1, 1) = camera_up.m_Y;
	o_Matrix(2, 1) = camera_up.m_Z;

	o_Matrix(0, 2) = dir.m_X;
	o_Matrix(1, 2) = dir.m_Y;
	o_Matrix(2, 2) = dir.m_Z;

	if (i_bTranslate)
	{
		maMatrix4x4 translate;
		translate.MakeTranslate(-i_Pos.m_X, -i_Pos.m_Y, -i_Pos.m_Z);
		o_Matrix = translate * o_Matrix;
	}
}

//--------------------------------------------------------------------
//	LookAt sets a camera matrix from position, target and up vector
//--------------------------------------------------------------------
void camCamera::LookAt(const maPoint3d &i_Pos, const maPoint3d &i_Target,
			 const maVector3d &i_Up)
{
	camCamera::ConstructMatrixLH(i_Pos, i_Target, i_Up, m_Camera);
	m_Position = i_Pos;
	m_Target = i_Target;
	m_Up = i_Up;
}

//--------------------------------------------------------------------
//	SetPosition sets the position of the camera
//--------------------------------------------------------------------
void camCamera::SetPosition( maPoint3d newPos ) {
	m_Position = newPos;
}

//--------------------------------------------------------------------
//	SetCameraMatrix sets a camera matrix
//--------------------------------------------------------------------
void camCamera::SetCameraMatrix(maMatrix4x4& o_Matrix)
{
	m_Camera = o_Matrix;

	// If matrix is set directly, need to set
	// position from matrix.
	// There might be a better way than this,
	// I'm not sure.
	maMatrix4x4 inv_matx(o_Matrix);
	inv_matx.Invert();
	m_Position = inv_matx * maPoint3d(0,0,0);
}

//--------------------------------------------------------------------
//	GetCameraMatrix sets a camera matrix
//--------------------------------------------------------------------
void camCamera::GetCameraMatrix(maMatrix4x4& o_Matrix) const
{
	o_Matrix = m_Camera;
}

//--------------------------------------------------------------------
//	GetProjectionMatrix sets a projection matrix
//--------------------------------------------------------------------
void camCamera::GetProjectionMatrix(maMatrix4x4& o_Matrix) const
{
	if( m_ProjectionDirty )
		this->make_projection();

	o_Matrix = m_Projection;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void camCamera::make_projection() const
{
	m_Projection.Identity();

	if (m_bOrthographic)
	{
		//float w = 2.0f / m_OrthoWidth;
		//float h = w * m_Aspect;
		float w = m_OrthoWidth / 2.0f;
		float left = m_ScreenXMin * w;
		float right = m_ScreenXMax * w;

		float h = w / m_Aspect;
		float top = m_ScreenYMax * h; // based on OpenGL code so switching top-bottom
		float bottom = m_ScreenYMin * h;

		float Q = 1.0f / (m_Far - m_Near);

		//m_Projection(0, 0) = w;
		//m_Projection(1, 1) = h;
		m_Projection(0, 0) = 2.0f / (right - left);
		m_Projection(1, 1) = 2.0f / (top - bottom);
		m_Projection(2, 2) = Q;
		m_Projection(3, 0) = (right + left) / (right - left);
		m_Projection(3, 1) = (top + bottom) / (top - bottom);
		m_Projection(3, 2) = Q * -m_Near;
	}
	else
	{
		// use screenmin and screenmax to allow sub-viewports or camera plane jitter

		float w = m_Near * float(tan(m_FOV * maConstants::c_fAngleToRad / 2.0f));
		float left = m_ScreenXMin * w + m_HorizontalFilmOffset;
		float right = m_ScreenXMax * w + m_HorizontalFilmOffset;

		float h = w / m_Aspect;
		float top = m_ScreenYMax * h; // based on OpenGL code so switching top-bottom
		float bottom = m_ScreenYMin * h;

		float Q = m_Far / (m_Far - m_Near);

		// Depth buffer precision is affected by the values specified for zNear and zFar.
		// The greater the ratio of zFar to zNear is, the less effective the depth buffer 
		// will be at distinguishing between surfaces that are near each other.
        // If r=zFar/zNear roughly log2(r) bits of depth buffer precision are lost.
		// Because r approaches infinity as zNear approaches 0, zNear must never be set to 0.
        
		m_Projection(0, 0) = 2.0f * m_Near / (right - left);
		m_Projection(1, 1) = 2.0f * m_Near / (top - bottom);
		m_Projection(2, 0) = (right + left) / (right - left);
		m_Projection(2, 1) = (top + bottom) / (top - bottom);
		m_Projection(2, 2) = Q;
		m_Projection(3, 2) = -Q * m_Near;
		m_Projection(2, 3) = 1;
		m_Projection(3, 3) = 0;
	}

	m_ProjectionDirty = false;
}

//--------------------------------------------------------------------
//	GetScreenPoint gets the point in world space corresponding to
//	the given point in (normalized) screen space.  The screen
//	space point should be in the range x: [-1, 1], y: [-1, 1].
//--------------------------------------------------------------------
maPoint3d camCamera::GetScreenPoint(float i_X, float i_Y) const
{
	if (this->m_bOrthographic)
	{
		float width = m_OrthoWidth / 2.0;
		maVector3d up = this->GetUp() * (width / m_Aspect);
		maVector3d left = this->GetLeft() * width;
		maVector3d dir = this->GetDirection();

		return this->GetPosition() + dir * m_Near - left * i_X + up * i_Y;
	}
	else
	{
		//	first make camera space point
		//
		maMatrix4x4 inv_proj = m_Projection;
		inv_proj.Invert();

		maVector3d ret_val(	-i_X,
							i_Y,
							m_Near);

		inv_proj.Transform(ret_val);

		//	now we can translate to world space
		//
		maMatrix4x4 inverse_mat = m_Camera;
		inverse_mat.Invert();
		inverse_mat.Transform(ret_val);
		return ret_val;
	}
}


//====================================================================
// Setup projection matrix to render part of normalized
// view volume.  This can be used to break up a large
// render into smaller areas.
// The arguments should be numbers between -1.0 and 1.0
//====================================================================
void camCamera::SetSubViewport(float i_Top, float i_Bottom,
							   float i_Left, float i_Right)
{
//	m_ScreenXMin(-1), m_ScreenXMax(1), m_ScreenYMin(-1), m_ScreenYMax(1)
	m_ScreenYMin = i_Top; // based on OpenGL code so switching top-bottom
	m_ScreenYMax = i_Bottom;
	m_ScreenXMin = i_Left;
	m_ScreenXMax = i_Right;

	m_ProjectionDirty = true;
}
void camCamera::GetSubViewport(float& o_Top, float& o_Bottom,
					float& o_Left, float& o_Right) const
{
	o_Top	 = m_ScreenYMin; // based on OpenGL code so switching top-bottom
	o_Bottom = m_ScreenYMax;
	o_Left	 = m_ScreenXMin;
	o_Right	 = m_ScreenXMax;
}

//--------------------------------------------------------------------
// Horizontal Film Offset shifts the viewport an absolute amount
// left or right in order to accomplish an off-axis stereo projection.
//--------------------------------------------------------------------
void camCamera::SetHorizontalFilmOffset(float i_FilmOffset)
{
	m_HorizontalFilmOffset = i_FilmOffset;
	m_ProjectionDirty = true;
}
float camCamera::GetHorizontalFilmOffset() const
{
	return m_HorizontalFilmOffset;
}


//--------------------------------------------------------------------
//	SetDOFParams sets information used by the depth of field effect
//--------------------------------------------------------------------
void camCamera::SetDOFParams(const camDOFData& i_DOFData)
{
	m_DOFData = i_DOFData;
}

//--------------------------------------------------------------------
//	GetDOFParams returns information used by the depth of field effect
//--------------------------------------------------------------------
void camCamera::GetDOFParams(camDOFData& o_DOFData) const
{
	o_DOFData = m_DOFData;
}

//--------------------------------------------------------------------
//	SetHDRParams sets data used by the high dynamic range renderer
//--------------------------------------------------------------------
void camCamera::SetHDRParams(const camHDRData& i_HDRData)
{
	m_HDRData = i_HDRData;
}

//--------------------------------------------------------------------
//	GetHDRParams returns data used by the high dynamic range renderer
//--------------------------------------------------------------------
void camCamera::GetHDRParams(camHDRData& o_HDRData) const
{
	o_HDRData = m_HDRData;
}

//--------------------------------------------------------------------
//	SetHDRParams sets data used by the high dynamic range renderer
//--------------------------------------------------------------------
void camCamera::SetPassBuffersParams(const camPassBuffersData& i_PassBuffersData)
{
	m_PassBuffersData = i_PassBuffersData;
}

//--------------------------------------------------------------------
//	GetHDRParams returns data used by the high dynamic range renderer
//--------------------------------------------------------------------
void camCamera::GetPassBuffersParams(camPassBuffersData& o_PassBuffersData) const
{
	o_PassBuffersData = m_PassBuffersData;
}

//--------------------------------------------------------------------
//	Set whether this camera is perspective or orthographic
//--------------------------------------------------------------------
void camCamera::SetOrthographic(bool i_bOrtho)
{	
	m_bOrthographic = i_bOrtho;
	m_ProjectionDirty = true;
}
bool camCamera::IsOrthographic() const
{
	return m_bOrthographic;
}

//--------------------------------------------------------------------
// Orthographic width controls the size of the orthographic 
//	view plane
//--------------------------------------------------------------------
void camCamera::SetOrthoWidth(float i_Width)
{	
	m_OrthoWidth = i_Width;
	m_ProjectionDirty = true;
}
float camCamera::GetOrthoWidth() const
{
	return m_OrthoWidth;
}

//----------------------------------------------------------------------------
// If this flag is set to true, then the aspect ratio of the camera
//	will be set to match the window's width and height.
//----------------------------------------------------------------------------
bool camCamera::GetMatchAspectToWindow() const
{
	return m_bMatchAspectToWindow;
}
void camCamera::SetMatchAspectToWindow(bool i_bMatch)
{
	m_bMatchAspectToWindow = i_bMatch;
	m_ProjectionDirty = true;
}

//--------------------------------------------------------------------
// SetStereoFD sets focal distance
//--------------------------------------------------------------------
void camCamera::SetStereoFD(float stereoFD)
{
	if (stereoFD == 0)
		stereoFD = 1.0f;
	m_StereoFD = stereoFD;
}

//--------------------------------------------------------------------
// SetStereoFilterColor sets the eyeglass color
//--------------------------------------------------------------------
void camCamera::SetStereoFilterColor(int color)
{
	m_StereoFilterColor = color;
}

//--------------------------------------------------------------------
// SetStereoType sets the stereo output type
//--------------------------------------------------------------------
void camCamera::SetStereoType(int type)
{
	m_StereoType = type;
}

//--------------------------------------------------------------------
// SetStereoIOD sets inter ocular distance
//--------------------------------------------------------------------
void camCamera::SetStereoIOD( float StereoIOD )
{
	m_StereoIOD = StereoIOD;
}

//--------------------------------------------------------------------
// SetStereoProjection sets the stereo output projection type
//--------------------------------------------------------------------
void camCamera::SetStereoProjection(int projection)
{
	m_StereoProjection = projection;
}

//--------------------------------------------------------------------
// SetStereoFD gets focal distance
//--------------------------------------------------------------------
float camCamera::GetStereoFD()
{
	return m_StereoFD;
}

//--------------------------------------------------------------------
// GetStereoFilterColor gets eyeglass color
//--------------------------------------------------------------------
int camCamera::GetStereoFilterColor()
{
	return m_StereoFilterColor;
}

//--------------------------------------------------------------------
// GetStereoFilterColor gets stereo output type
//--------------------------------------------------------------------
int camCamera::GetStereoType()
{
	return m_StereoType;
}

//--------------------------------------------------------------------
// SetStereoIOD gets inter ocular distance
//--------------------------------------------------------------------
float camCamera::GetStereoIOD()
{
	return m_StereoIOD;
}

//--------------------------------------------------------------------
// GetStereoProjection gets stereo output type
//--------------------------------------------------------------------
int camCamera::GetStereoProjection()
{
	return m_StereoProjection;
}

//--------------------------------------------------------------------
// SetName()
//--------------------------------------------------------------------
void camCamera::SetName( const std::string &i_Name )
{
	m_Name = i_Name;
}

//--------------------------------------------------------------------
// GetName()
//--------------------------------------------------------------------
std::string camCamera::GetName()
{
	return m_Name;
}

//--------------------------------------------------------------------
// SetEnableALP()
//--------------------------------------------------------------------
void camCamera::SetEnableALP(bool i_bEnableALP)
{
	m_bEnableALP = i_bEnableALP;
}

//--------------------------------------------------------------------
// SetFStop()
//--------------------------------------------------------------------
void camCamera::SetFStop(float i_Fstop)
{
	m_FStop = i_Fstop;
}

//--------------------------------------------------------------------
// SetFocalDistance()
//--------------------------------------------------------------------
void camCamera::SetFocalDistance(float i_FocalDistance)
{
	m_FocalDistance = i_FocalDistance;
}

//--------------------------------------------------------------------
// SetFocalLength()
//--------------------------------------------------------------------
void camCamera::SetFocalLength(float i_FocalLength)
{
	m_FocalLength = i_FocalLength;
}

//--------------------------------------------------------------------
// GetEnableALP()
//--------------------------------------------------------------------
bool camCamera::GetEnableALP()
{
	return m_bEnableALP;
}

//--------------------------------------------------------------------
// GetFStop()
//--------------------------------------------------------------------
float camCamera::GetFStop()
{
	return m_FStop;
}

//--------------------------------------------------------------------
// GetFocalDistance()
//--------------------------------------------------------------------
float camCamera::GetFocalDistance()
{
	return m_FocalDistance;
}

//--------------------------------------------------------------------
// GetFocalLength()
//--------------------------------------------------------------------
float camCamera::GetFocalLength()
{
	return m_FocalLength;
}