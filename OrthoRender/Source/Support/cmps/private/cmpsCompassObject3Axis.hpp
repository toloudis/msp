/*****************************************************************************
**  cmpsCompassObject3Axis.hpp
**
**      cmpsCompassObject3Axis is an geometric object base class
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_COMPASSOBJECT3AXIS_HPP
#error cmpsCompassObject3Axis.hpp multiply included
#endif
#define CMPS_COMPASSOBJECT3AXIS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

#ifndef CMPS_COMPASSOBJECT_HPP
#include "Support/cmps/private/cmpsCompassObject.hpp"
#endif

#include <vector>
#include <set>


//============================================================================
//	forward references
//============================================================================
class maFloatRGBA;
class cmpsObjectSimple;
class g3dSceneNode;
class matMaterial;


//============================================================================
//============================================================================
class cmpsCompassObject3Axis : public cmpsCompassObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmpsCompassObject3Axis(int i_RenderLayer);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmpsCompassObject3Axis();

		//--------------------------------------------------------------------
		//	SetPosition  moves the compass to the position passed in
		//--------------------------------------------------------------------
		void SetPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	SetScale changes the scale of the compass.
		//--------------------------------------------------------------------
		virtual void SetScale(const maPoint3d& i_Scale);

		//--------------------------------------------------------------------
		//	SetOrientation changes the orientation of the compass.
		//--------------------------------------------------------------------
		virtual void SetOrientation(const maRotation& i_Orientation);

		//--------------------------------------------------------------------
		//	SetRenderable turns display of the compass on and off.
		//--------------------------------------------------------------------
		virtual void SetRenderable(bool i_bRender);

		//----------------------------------------------------------------------------
		//	HighlightPart - highlight a part of a compass
		//----------------------------------------------------------------------------
		virtual void HighlightPart( int i_Part );

		//--------------------------------------------------------------------
		//	SetActive turns enabled state of the compass on and off.
		//--------------------------------------------------------------------
		virtual void SetActive(bool i_bActive);

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the cmpsCompassObject3Axis.
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
		virtual bool MatchPickCode(envType::UInt32 i_PickCode);

		//--------------------------------------------------------------------
		//	Add_Object adds object to list
		//--------------------------------------------------------------------
		void Add_Object( cmpsObjectSimple* i_pObject );

	protected:
		//--------------------------------------------------------------------
		//	Add_Line adds line to list
		//--------------------------------------------------------------------
		void Add_Line( cmpsObjectSimple* i_pObject );

		//--------------------------------------------------------------------
		//	Add_Center sets the center object
		//--------------------------------------------------------------------
		void Add_Center( cmpsObjectSimple* i_pObject );

		//--------------------------------------------------------------------
		//	Create_Line creates a line
		//--------------------------------------------------------------------
		enum Axis 
		{ 
			e_AxisX, e_AxisY, e_AxisZ 
		};
		cmpsObjectSimple* Create_Line(Axis i_Axis, const maFloatRGBA& i_Color );

		//--------------------------------------------------------------------
		//	Create_Center creates a sphere
		//--------------------------------------------------------------------
		cmpsObjectSimple* Create_Center(const maFloatRGBA& i_Color );

		//--------------------------------------------------------------------
		//	Get_Center returns the center object
		//--------------------------------------------------------------------
		const cmpsObjectSimple* Get_Center() const;

		//--------------------------------------------------------------------
		//	using the latest compass settings, modify the object's shape.
		//--------------------------------------------------------------------
		virtual void ResizeObject(const maPoint3d& i_CameraPos);

		//----------------------------------------------------------------------------
		//	SetBounds sets the bounding area for this compass
		//----------------------------------------------------------------------------
		virtual void SetBounds( const maAxisBox& i_Bounds,
							   const maPoint3d& i_WorldPivot, 
							   const maPoint3d& i_CameraPos  );

		//----------------------------------------------------------------------------
		//	GetDefaultSize() - get the default size of the shape
		//----------------------------------------------------------------------------
		virtual float GetDefaultSize() const;

		std::vector<cmpsObjectSimple*> m_Objects;
		std::vector<cmpsObjectSimple*> m_Lines;
		cmpsObjectSimple* m_pCenter;
};

