/*****************************************************************************
**	prjltIconObject.hpp
**
**	Base class for 3D Object that holds an icon for the projected light
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
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
class matTexture;
class matMaterial;
class camCamera;
class g3dFragment;
class maRotation;

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
	virtual void  Update(const maPoint3d &i_Pos, 
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
						 float i_TexturePosition) = 0;

	//--------------------------------------------------------------------
	//	UpdateIconScale - function that sets the icons scale.
	//--------------------------------------------------------------------
	virtual void UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale ) = 0;

	//--------------------------------------------------------------------
	// Set color (emissive, not diffuse) for fragments
	//--------------------------------------------------------------------
	virtual void SetColor(const maFloatRGBA &i_Color) = 0;

	//----------------------------------------------------------------------------
	//	Renderable sets whether the object is visible
	//----------------------------------------------------------------------------
	virtual void SetRenderable(bool i_Renderable) = 0;
	virtual bool GetRenderable() const = 0;

	//----------------------------------------------------------------------------
	//	Pickable sets whether the GPU pick icons should be pickable
	//----------------------------------------------------------------------------
	virtual void SetPickable(bool i_bPickable) = 0;

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	virtual bool PositionContainsPickCode(envType::UInt32 i_PickCode) const = 0;
	virtual bool TargetContainsPickCode(envType::UInt32 i_PickCode) const = 0;

	//----------------------------------------------------------------------------
	// Get current scale values for different views of the icons
	//----------------------------------------------------------------------------
	virtual float	GetPickPositionScale(int i_IconLayerIndex) = 0;
	virtual float	GetPickTargetScale(int i_IconLayerIndex) = 0;

protected:
	//----------------------------------------------------------------------------
	// Create a cone shape with lines in order to represent cone angles
	//----------------------------------------------------------------------------
	g3dFragment* CreateLineCone(float i_BaseRadius, float i_Height, int i_Divisions,
											matMaterial *i_pMaterial);
	//----------------------------------------------------------------------------
	// Create a cone shape in order to represent cone angles
	//----------------------------------------------------------------------------
	g3dFragment* CreateCone(float i_BaseRadius, float i_Height, int i_Divisions,
												matMaterial *i_pMaterial);
};
