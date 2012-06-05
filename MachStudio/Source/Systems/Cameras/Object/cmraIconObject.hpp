/*****************************************************************************
**	cmraIconObject.hpp
**
**	3D Object that holds an icon for the camera representing its view
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CMRA_ICONOBJECT_HPP
#error cmraIconObject.hpp multiply included
#endif
#define CMRA_ICONOBJECT_HPP

#ifndef API3D_OBJECTGEOM_HPP
#include "Tool/api3d/api3dObjectGeom.hpp"
#endif


//============================================================================
//============================================================================
class icnIconSet;
class camCamera;
class g3dFragment;
class gpxFragment;
class gpxIconSet;

//============================================================================
//============================================================================
class cmraIconObject 
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraIconObject();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraIconObject();

	//--------------------------------------------------------------------
	// Update vertices of morphable fragment match view
	//--------------------------------------------------------------------
	void  Update(const maPoint3d &i_Pos, 
				 const maPoint3d &i_Target,
				 float i_Tilt,
				 float i_FieldOfView,
				 float i_AspectRatio,
				float i_NearBlurDist, float i_NearFocalDist, 
				float i_FarFocalDist, float i_FarBlurDist
				 );

	//--------------------------------------------------------------------
	//	UpdateIconScale - function that sets the icons scale.
	//--------------------------------------------------------------------
	void UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale );

	//----------------------------------------------------------------------------
	//	Set global scale into icon
	//----------------------------------------------------------------------------
	//void SetUniformScale(float i_Scale);

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
	float	GetCameraObjectScale(int i_IconLayerIndex);
	float	GetPickTargetScale(int i_IconLayerIndex);

private:
	g3dFragment* m_pFragment;
	gpxFragment* m_pFragmentProxy;	// thread-safe proxy to fragment

	maPoint3d m_Position;
	maPoint3d m_Target;
	float m_Tilt;
	float m_FieldOfView;
	float m_AspectRatio;
	float m_NearBlurDist;
	float m_NearFocalDist;
	float m_FarFocalDist;
	float m_FarBlurDist;

	icnIconSet		*m_pObject;
	gpxIconSet		*m_pObjectProxy;	// thread-safe proxy to object	
	icnIconSet		*m_pDOFObject[4];
	gpxIconSet		*m_pDOFObjectProxy[4];	// thread-safe proxy to object	
	gpxFragment		*m_pDOFFragmentProxy[4];	// thread-safe proxy to fragment	

	icnIconSet		*m_pPickPosition;
	gpxIconSet		*m_pPickPositionProxy;	// thread-safe proxy to object	
	icnIconSet		*m_pPickTarget;
	gpxIconSet		*m_pPickTargetProxy;	// thread-safe proxy to object	
};
