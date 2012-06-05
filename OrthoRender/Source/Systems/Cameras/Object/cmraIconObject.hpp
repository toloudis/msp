/*****************************************************************************
**	cmraIconObject.hpp
**
**	3D Object that holds an icon for the camera representing its view
**
**	Extra Large Technology
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
class api3dObjectSimple;
class g3dFragment;


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

	//----------------------------------------------------------------------------
	//	Set global scale into icon
	//----------------------------------------------------------------------------
	void SetUniformScale(float i_Scale);

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

private:
	g3dFragment* m_pFragment;

	maPoint3d m_Position;
	maPoint3d m_Target;
	float m_Tilt;
	float m_FieldOfView;
	float m_AspectRatio;
	float m_NearBlurDist;
	float m_NearFocalDist;
	float m_FarFocalDist;
	float m_FarBlurDist;

	api3dObjectSimple *m_pObject;
	api3dObjectSimple *m_pDOFObject[4];

	api3dObjectSimple *m_pPickPosition;
	api3dObjectSimple *m_pPickTarget;
};
