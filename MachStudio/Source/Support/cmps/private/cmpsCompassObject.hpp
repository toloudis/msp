/*****************************************************************************
**  cmpsCompassObject.hpp
**
**      cmpsCompassObject is an geometric object base class
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_COMPASSOBJECT_HPP
#error cmpsCompassObject.hpp multiply included
#endif
#define CMPS_COMPASSOBJECT_HPP

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/Cam/camCamera.hpp"
#endif 
#ifndef CMPS_RENDERLAYER_HPP
#include "Support/cmps/cmpsRenderLayer.hpp"
#endif 

//----------------------------------------------------------------------------
//	forward references
//----------------------------------------------------------------------------
class maFloatRGBA;
class matMaterial;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class cmpsCompassObject : public pick3dPickObject
{
	public:

		enum Parts
		{
			e_X = 0x0001,
			e_Y = 0x0002,
			e_Z = 0x0004,
			e_Center = 0x0010,
			e_All = 0xffff
		};

		//--------------------------------------------------------------------
		// Pass in render layer in scene to use for icons
		//--------------------------------------------------------------------
		cmpsCompassObject(cmpsRenderLayer::RenderLayer i_RenderLayer);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmpsCompassObject();

		//--------------------------------------------------------------------
		//	GetPosition returns the position of the compass.
		//--------------------------------------------------------------------
		virtual inline const maPoint3d& GetPosition() const;

		//--------------------------------------------------------------------
		//	SetPosition  moves the compass to the position passed in
		//--------------------------------------------------------------------
		virtual void SetPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	LockScale() - don't allow the scale of the compass to change
		//	anymore.
		//--------------------------------------------------------------------
		virtual void LockScale( bool i_bLockScale );

		//--------------------------------------------------------------------
		//	LockScale() - get the lock scale
		//--------------------------------------------------------------------
		inline const bool GetLockScale() const;

		//--------------------------------------------------------------------
		//	SetScale changes the scale of the compass.
		//--------------------------------------------------------------------
		virtual void SetScale(const maPoint3d& i_Scale);

		//--------------------------------------------------------------------
		//	GetScale returns the scale
		//--------------------------------------------------------------------
		inline const maPoint3d& GetScale() const;

		//--------------------------------------------------------------------
		//	SetOrientation changes the orientation of the compass.
		//--------------------------------------------------------------------
		virtual void SetOrientation(const maRotation& i_Orientation);

		//--------------------------------------------------------------------
		//	GetOrientation returns the Orientation
		//--------------------------------------------------------------------
		inline const maRotation& cmpsCompassObject::GetOrientation() const;

		//----------------------------------------------------------------------------
		//	AdjustCompassToBounds updates the compass to the position
		//		and bounding area of an object. This replaces the individual calls
		//		to SetPosition, SetOrientation and SetBounds.
		//----------------------------------------------------------------------------
		virtual void AdjustCompassToBounds( int i_IconLayerIndex,
											const maPoint3d& i_Position,
											const maAxisBox& i_Bounds,
										    const maPoint3d& i_WorldPivot, 
										    const camCamera& i_Camera,
										    float i_ScalingFactor ) = 0;

		//--------------------------------------------------------------------
		//	Resize the compass based on the given camera view
		//--------------------------------------------------------------------
		virtual void ResizeObject(int i_IconLayerIndex, const camCamera& i_Camera ) = 0;

		//----------------------------------------------------------------------------
		//	GetBounds returns the bounding area for this compass
		//----------------------------------------------------------------------------
		inline const maAxisBox& GetBounds();

		//----------------------------------------------------------------------------
		//	GetBounds returns the pivot point in world space for this compass
		//----------------------------------------------------------------------------
		inline const maPoint3d& GetPivot();

		//----------------------------------------------------------------------------
		//	GetScalingFactor returns the relative scaling to apply after the compass
		//  has been adjusted to the bounds of the selected objects.
		//----------------------------------------------------------------------------
		inline float GetScalingFactor();

		//--------------------------------------------------------------------
		//	SetRenderable turns display of the compass on and off.
		//--------------------------------------------------------------------
		virtual void SetRenderable(bool i_bRender);
		inline const bool GetRenderable() const;

		//--------------------------------------------------------------------
		//	SetActive turns enabled state of the compass on and off.
		//--------------------------------------------------------------------
		virtual void SetActive(bool i_bActive);
		inline const bool GetActive() const;

		//--------------------------------------------------------------------
		//	SetParts turns on only the specified parts of the compass
		//--------------------------------------------------------------------
		virtual void SetParts( const int i_Parts );

		//--------------------------------------------------------------------
		//	GetParts gets the currently rendered parts of the compass
		//--------------------------------------------------------------------
		virtual int GetParts();

		//----------------------------------------------------------------------------
		//	HighlightPart - highlight a part of a compass
		//----------------------------------------------------------------------------
		virtual void HighlightPart( int i_Part );

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the cmpsCompassObject.
		//	If it does, the t value is also returned in o_T and the axis that
		//	was selected is returned in o_Axis
		//--------------------------------------------------------------------
		virtual bool RayPick(	int i_IconLayerIndex,
								const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T,
								int& o_Part);

		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual bool MatchPickCode(envType::UInt32 i_PickCode);

		//--------------------------------------------------------------------
		//	SetLayerScale changes the scale of the compass for only a 
		// given icon layer (a render panel)
		//--------------------------------------------------------------------
		virtual void SetLayerScale(int i_IconLayerIndex, const maPoint3d& i_Scale)  = 0;

		//--------------------------------------------------------------------
		//	SetLayerRenderable sets the compass visibility for only a given icon 
		// layer (a render panel)
		//--------------------------------------------------------------------
		virtual void SetLayerRenderable(int i_IconLayerIndex, bool i_bRender) = 0;

	protected:
		//--------------------------------------------------------------------
		// Return the index of the render layer in the scene to use for
		//	the 3d icons of the compass
		//--------------------------------------------------------------------
		cmpsRenderLayer::RenderLayer GetRenderLayer();

	private:

		maPoint3d	m_Position;
		maPoint3d	m_Scale;
		maRotation	m_Orientation;
		maAxisBox	m_Bounds;
		maPoint3d	m_Pivot;
		float		m_ScalingFactor;

		bool m_bRenderable;
		bool m_bLockScale;
		bool m_bActive;
		cmpsRenderLayer::RenderLayer m_RenderLayer;

		int m_nShowOnlyParts;
};


//----------------------------------------------------------------------------
//	GetPosition returns the position of the compass.
//----------------------------------------------------------------------------
inline const maPoint3d& cmpsCompassObject::GetPosition() const
{
	return m_Position;
}

//----------------------------------------------------------------------------
//	GetRenderable
//----------------------------------------------------------------------------
inline const bool cmpsCompassObject::GetRenderable() const
{
	return m_bRenderable;
}

//----------------------------------------------------------------------------
//	GetActive
//----------------------------------------------------------------------------
inline const bool cmpsCompassObject::GetActive() const
{
	return m_bActive;
}

//--------------------------------------------------------------------
//	LockScale() - get the lock scale
//--------------------------------------------------------------------
inline const bool cmpsCompassObject::GetLockScale() const
{
	return m_bLockScale;
}

//--------------------------------------------------------------------
//	GetScale returns the scale
//--------------------------------------------------------------------
inline const maPoint3d& cmpsCompassObject::GetScale() const
{
	return m_Scale;
}

//--------------------------------------------------------------------
//	GetOrientation returns the Orientation
//--------------------------------------------------------------------
inline const maRotation& cmpsCompassObject::GetOrientation() const
{
	return m_Orientation;
}

//----------------------------------------------------------------------------
//	GetBounds returns the bounding area for this compass
//----------------------------------------------------------------------------
inline const maAxisBox& cmpsCompassObject::GetBounds()
{
	return m_Bounds;
}

//----------------------------------------------------------------------------
//	GetBounds returns the pivot point in world space for this compass
//----------------------------------------------------------------------------
inline const maPoint3d& cmpsCompassObject::GetPivot()
{
	return m_Pivot;
}

//----------------------------------------------------------------------------
//	GetScalingFactor returns the relative scaling to apply after the compass
//  has been adjusted to the bounds of the selected objects.
//----------------------------------------------------------------------------
inline float cmpsCompassObject::GetScalingFactor()
{
	return m_ScalingFactor;
}