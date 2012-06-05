/*****************************************************************************
**  cmpsCompassObjectSingle.hpp
**
**      cmpsCompassObjectSingle is an geometric object base class
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_COMPASSOBJECTSINGLE_HPP
#error cmpsCompassObjectSingle.hpp multiply included
#endif
#define CMPS_COMPASSOBJECTSINGLE_HPP

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
class matMaterial;


//============================================================================
//============================================================================
class cmpsCompassObjectSingle : public cmpsCompassObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmpsCompassObjectSingle(cmpsRenderLayer::RenderLayer i_RenderLayer);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmpsCompassObjectSingle();

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
		//	RayPick returns true if the given ray intersects the cmpsCompassObjectSingle.
		//	If it does, the t value is also returned in o_T and the axis that
		//	was selected is returned in o_Axis
		//--------------------------------------------------------------------
		virtual bool RayPick(	int i_IconLayerIndex,
								const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T );

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

	protected:
		//--------------------------------------------------------------------
		//	Set_Object sets internal object pointer
		//--------------------------------------------------------------------
		void Set_Object( cmpsObjectSimple* i_pObject );

		//--------------------------------------------------------------------
		//	Delete_Object - deletes the current objects held by this class
		//--------------------------------------------------------------------
		void Delete_Object();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline cmpsObjectSimple * GetObject();

	private:
		cmpsObjectSimple* m_pObject;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline cmpsObjectSimple * cmpsCompassObjectSingle::GetObject()
{
	return m_pObject;
}


