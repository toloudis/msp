/*****************************************************************************
**  cmpsCompassObjectRotate.hpp
**
**      cmpsCompassObjectRotate is a geometric object used to display axis
**	for Rotate.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_COMPASSOBJECTROTATE_HPP
#error cmpsCompassObjectRotate.hpp multiply included
#endif
#define CMPS_COMPASSOBJECTROTATE_HPP

#ifndef CMPS_COMPASSOBJECT_HPP
#include "Support/cmps/private/cmpsCompassObject.hpp"
#endif

#include <vector>


//============================================================================
//	forward references
//============================================================================
class cmpsObjectSimple;


//============================================================================
//============================================================================
class cmpsCompassObjectRotate : public cmpsCompassObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmpsCompassObjectRotate(int i_RenderLayer);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmpsCompassObjectRotate();

		//--------------------------------------------------------------------
		//	SetPosition  moves the compass to the position passed in
		//--------------------------------------------------------------------
		void SetPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	SetScale changes the scale of the compass.
		//--------------------------------------------------------------------
		void SetScale(const maPoint3d& i_Scale);

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

		//----------------------------------------------------------------------------
		//	SetBounds sets the bounding area for this compass
		//----------------------------------------------------------------------------
		virtual void SetBounds( const maAxisBox& i_Bounds,
							   const maPoint3d& i_WorldPivot, 
							   const maPoint3d& i_CameraPos  );

	protected:
		//--------------------------------------------------------------------
		//	Add_Line adds line to list
		//--------------------------------------------------------------------
		void Add_Line( cmpsObjectSimple* i_pObject );

		//--------------------------------------------------------------------
		//	Create_Circle creates a circle
		//--------------------------------------------------------------------
		cmpsObjectSimple* Create_Circle(const maFloatRGBA& i_Color, float i_fRadius = 0.2f );

		//----------------------------------------------------------------------------
		//	GetDefaultSize() - get the default size of the shape
		//----------------------------------------------------------------------------
		virtual float GetDefaultSize() const;

		//--------------------------------------------------------------------
		//	using the latest compass settings, modify the object's shape.
		//--------------------------------------------------------------------
		void ResizeObject(const maPoint3d& i_CameraPos);

	private:
		std::vector<cmpsObjectSimple*> m_Lines;
};
