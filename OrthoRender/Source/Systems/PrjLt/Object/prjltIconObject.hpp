/*****************************************************************************
**	prjltIconObject.hpp
**
**	3D Object that holds an icon for the projected light
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PRJLT_ICONOBJECT_HPP
#error prjltIconObject.hpp multiply included
#endif
#define PRJLT_ICONOBJECT_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif


//============================================================================
//============================================================================
class api3dObjectSimple;
class g3dFragment;
class matTexture;


//============================================================================
//============================================================================
class prjltIconObject 
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	prjltIconObject();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~prjltIconObject();

	//--------------------------------------------------------------------
	// Update vertices of morphable fragment match view
	//--------------------------------------------------------------------
	void  Update(const maPoint3d &i_Pos, 
				 const maPoint3d &i_Target,
				 float i_Scale,
				 float i_Tilt,
				 float i_Angle,
				 float i_AspectRatio,
				 float i_Range,
				 matTexture* i_pTexture);

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

private:
	g3dFragment* m_pFragment;
	g3dFragment* m_pTextureFrag;
	api3dObjectSimple *m_pObject;
	api3dObjectSimple *m_pTextureObject;
	api3dObjectSimple *m_pPickPosition;
	api3dObjectSimple *m_pPickTarget;
};
