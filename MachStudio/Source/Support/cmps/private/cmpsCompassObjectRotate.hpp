/*****************************************************************************
**  cmpsCompassObjectRotate.hpp
**
**      cmpsCompassObjectRotate is a geometric object used to display axis
**	for Rotate.
**
**	StudioGPU
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
		cmpsCompassObjectRotate(cmpsRenderLayer::RenderLayer i_RenderLayer);

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
		virtual void SetLayerScale(int i_IconLayerIndex, const maPoint3d& i_Scale);

		//--------------------------------------------------------------------
		//	SetLayerRenderable sets the compass visibility for only a given icon 
		// layer (a render panel)
		//--------------------------------------------------------------------
		virtual void SetLayerRenderable(int i_IconLayerIndex, bool i_bRender);

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
										    float i_ScalingFactor );

	protected:
		//--------------------------------------------------------------------
		//	Add_Line adds line to list
		//--------------------------------------------------------------------
		void Add_Line( cmpsObjectSimple* i_pObject );

		//--------------------------------------------------------------------
		//	Create_Circle creates a circle
		//--------------------------------------------------------------------
		cmpsObjectSimple* Create_Circle(const maFloatRGBA& i_Color, float i_fRadius, 
									   cmpsRenderLayer::RenderLayer i_RenderLayer );

		//----------------------------------------------------------------------------
		//	GetDefaultSize() - get the default size of the shape
		//----------------------------------------------------------------------------
		virtual float GetDefaultSize() const;

		//--------------------------------------------------------------------
		//	Resize the compass based on the given camera view
		//--------------------------------------------------------------------
		void ResizeObject(int i_IconLayerIndex, const camCamera& i_Camera );

	private:
		std::vector<cmpsObjectSimple*> m_Lines;
};
