/*****************************************************************************
**	prjltIconSpot.hpp
**
**	3D Object that holds an icon for a spot light
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef PRJLT_ICONSPOT_HPP
#error prjltIconSpot.hpp multiply included
#endif
#define PRJLT_ICONSPOT_HPP

#ifndef PRJLT_ICONOBJECT_HPP
#include "Systems/PrjLt/Object/prjltIconObject.hpp"
#endif


//============================================================================
//============================================================================
class icnIconSet;
class g3dFragment;
class gpxFragment;
class gpxIconSet;

//============================================================================
//============================================================================
class prjltIconSpot : public prjltIconObject
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	prjltIconSpot();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~prjltIconSpot();

	//--------------------------------------------------------------------
	// Update vertices of morphable fragment match view
	//--------------------------------------------------------------------
	void  Update(const maPoint3d &i_Pos, 
				 const maPoint3d &i_Target,
				 const maRotation &i_Orientation,
				 float i_Scale,
				 float i_Tilt,
				 bool  i_bDirectional,
				 float i_Angle,
				 float i_InnerAngle,
				 float i_AspectRatio,
				 float i_Range,
				 matTexture* i_pTexture,
				 float i_TexturePosition);

	//--------------------------------------------------------------------
	//	GlobalScaleChanged - notification that global scale has changed.
	//--------------------------------------------------------------------
	//virtual void GlobalScaleChanged( float i_Scale );

	//--------------------------------------------------------------------
	//	UpdateIconScale - function that sets the icons scale.
	//--------------------------------------------------------------------
	void UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale );

	//--------------------------------------------------------------------
	// Set color (emissive, not diffuse) for fragments
	//--------------------------------------------------------------------
	void SetColor(const maFloatRGBA &i_Color);

	//----------------------------------------------------------------------------
	//	Renderable sets whether the object is visible
	//----------------------------------------------------------------------------
	void SetRenderable(bool i_Renderable);
	bool GetRenderable() const;

	//----------------------------------------------------------------------------
	//	Pickable sets whether the GPU pick icons should be pickable
	//----------------------------------------------------------------------------
	void SetPickable(bool i_bPickable);

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	virtual bool PositionContainsPickCode(envType::UInt32 i_PickCode) const;
	virtual bool TargetContainsPickCode(envType::UInt32 i_PickCode) const;

	//----------------------------------------------------------------------------
	// Get current scale values for different views of the icons
	//----------------------------------------------------------------------------
	float	GetPickPositionScale(int i_IconLayerIndex);
	float	GetPickTargetScale(int i_IconLayerIndex);

private:

	icnIconSet *m_pCone;
	gpxIconSet	  *m_pConeProxy;	// thread-safe proxy to object
	icnIconSet *m_pPickPosition;
	gpxIconSet	  *m_pPickPositionProxy;	// thread-safe proxy to object	
	icnIconSet *m_pPickTarget;
	gpxIconSet	  *m_pPickTargetProxy;	// thread-safe proxy to object	

	// Showing the frustrum is handy for debugging
#define SHOW_FRUSTRUM 0
#if SHOW_FRUSTRUM
	g3dFragment* m_pFragment;
	gpxFragment* m_pFragmentProxy;
	icnIconSet *m_pObject;
	gpxIconSet	  *m_pObjectProxy;	// thread-safe proxy to object
#endif
};
