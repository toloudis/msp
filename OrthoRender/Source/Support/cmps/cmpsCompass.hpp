/*****************************************************************************
**  cmpsCompass.hpp
**
**      cmpsCompass is a compass object for object manipulation.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_COMPASS_HPP
#error cmpsCompass.hpp multiply included
#endif
#define CMPS_COMPASS_HPP

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


//----------------------------------------------------------------------------
//	forward references
//----------------------------------------------------------------------------
class maFloatRGBA;
class g3dSceneNode;
class matMaterial;
class cmpsCompassObject;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class cmpsCompass : public pick3dPickObject
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

		enum RenderLayer
		{
			e_World = 0,
			e_Camera
		};

		//--------------------------------------------------------------------
		// Pass in enumerated value for which layer to display the compass
		//--------------------------------------------------------------------
		cmpsCompass(RenderLayer i_RenderLayer = e_World);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmpsCompass();

		//--------------------------------------------------------------------
		//	GetPosition returns the position of the compass.
		//--------------------------------------------------------------------
		const maPoint3d& GetPosition() const;

		//--------------------------------------------------------------------
		//	SetPosition  moves the compass to the position passed in
		//--------------------------------------------------------------------
		void SetPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	LockScale() - don't allow the scale of the compass to change
		//	anymore.
		//--------------------------------------------------------------------
		void LockScale( bool i_bLockScale );

		//--------------------------------------------------------------------
		//	GetScale the scale of the compass.
		//--------------------------------------------------------------------
		const maPoint3d& GetScale();

		//--------------------------------------------------------------------
		//	SetScale changes the scale of the compass.
		//--------------------------------------------------------------------
		void SetScale(const maPoint3d& i_Scale);

		//--------------------------------------------------------------------
		//	SetOrientation changes the orientation of the compass.
		//--------------------------------------------------------------------
		virtual void SetOrientation(const maRotation& i_Orientation);

		//----------------------------------------------------------------------------
		//	SetBounds sets the bounding area for this compass
		//----------------------------------------------------------------------------
		virtual void SetBounds( const maAxisBox& i_Bounds,
							   const maPoint3d& i_WorldPivot,
							   const maPoint3d& i_CameraPos  );

		//--------------------------------------------------------------------
		//	SetRenderable turns display of the compass on and off.
		//--------------------------------------------------------------------
		void SetRenderable(bool i_bRender);
		bool GetRenderable();

		//--------------------------------------------------------------------
		//	SetActive turns enabled state of the compass on and off.
		//--------------------------------------------------------------------
		void SetActive(bool i_bActive);
		bool GetActive();

		//--------------------------------------------------------------------
		//	SetParts turns on only the specified parts of the compass
		//--------------------------------------------------------------------
		void SetParts(const int i_Parts);

		//--------------------------------------------------------------------
		//	GetParts gets the currently rendered parts of the compass
		//--------------------------------------------------------------------
		int GetParts();

		//----------------------------------------------------------------------------
		//	HighlightPart - highlight a part of a compass
		//----------------------------------------------------------------------------
		virtual void HighlightPart( int i_Part );

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the cmpsCompass.
		//	If it does, the t value is also returned in o_T and the axis that
		//	was selected is returned in o_Axis
		//--------------------------------------------------------------------
		virtual bool RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T,
								int& o_Part);
		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode) const;


	protected:
		//--------------------------------------------------------------------
		// Return the index of the render layer in the scene to use for
		//	the 3d icons of the compass
		//--------------------------------------------------------------------
		int GetObjectRenderLayer();

		//
		// data
		//
		cmpsCompassObject* m_pCompassObject;
		RenderLayer m_RenderLayer;
};


