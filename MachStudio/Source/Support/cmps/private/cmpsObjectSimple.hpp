/*****************************************************************************
**	cmpsObjectSimple.hpp
**
**	Derived class, represents an element of geometry for a compass part. 
**	Adds on some convenience functions to the api3dObjectSimple api 
**	for setting color states on the compass part.
**
**	Now controls an api3dObjectSimple through a proxy class and does not
**	derive from it directly anymore.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_OBJECTSIMPLE_HPP
#error cmpsObjectSimple.hpp multiply included
#endif
#define CMPS_OBJECTSIMPLE_HPP

#ifndef GPX_EFFECTDATA_HPP
#include "Tool/gpx/gpxEffectData.hpp"
#endif 
#ifndef EFF_SOLIDDATA_HPP
#include "Graphics/Eff/effSolidData.hpp"
#endif 
#ifndef MA_ROTATION_HPP
#include "Core/Ma/maRotation.hpp"
#endif 
#ifndef CMPS_RENDERLAYER_HPP
#include "Support/cmps/cmpsRenderLayer.hpp"
#endif 


//============================================================================
//============================================================================
class g3dFragment;
class gpxIconSet;
class icnIconSet;
class api3dObjectSimple;

//============================================================================
//============================================================================
const float lc_CompassEmissiveNormal	= 0.5f;
const float lc_CompassEmissiveHighlight	= 1.0f;


//============================================================================
//============================================================================
class cmpsObjectSimple 
{
public:
	//--------------------------------------------------------------------
	// constructor - this object assumes ownership of the arguments.
	//	It will also assign the material to the fragment.
	//--------------------------------------------------------------------
	cmpsObjectSimple(g3dFragment* i_pFragment, 
					 matMaterial *i_pMaterial,
					 cmpsRenderLayer::RenderLayer i_RenderLayer);

	//--------------------------------------------------------------------
	// constructor variation offsets fragment by transformation
	//--------------------------------------------------------------------
	cmpsObjectSimple(const maMatrix4x4 &i_Transform,
				     g3dFragment* i_pFragment, 
				     matMaterial *i_pMaterial,
				     cmpsRenderLayer::RenderLayer i_RenderLayer);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~cmpsObjectSimple();

	//--------------------------------------------------------------------
	// Add or remove object from api3dScene
	//--------------------------------------------------------------------
	//void AddToScene(int i_Layer);
	//void RemoveFromScene(int i_Layer);

	//--------------------------------------------------------------------
	// Position
	//--------------------------------------------------------------------
	maPoint3d  GetPosition() const;
	void  SetPosition(const maPoint3d &i_Position);

	//--------------------------------------------------------------------
	// Orientation
	//--------------------------------------------------------------------
	maRotation GetOrientation() const;
	void  SetOrientation(const maRotation &i_Rotation);

	//--------------------------------------------------------------------
	// Scale
	//--------------------------------------------------------------------
	maVector3d GetScale() const;
	void  SetScale(const maVector3d& i_Scale);

	//----------------------------------------------------------------------------
	//	Renderable sets whether the object is visible
	//----------------------------------------------------------------------------
	void SetRenderable(bool i_Renderable);
	bool GetRenderable() const;

	//--------------------------------------------------------------------
	//	GetWorldBox returns the bounding box
	//--------------------------------------------------------------------
	const maAxisBox& GetWorldBox() const;

	//----------------------------------------------------------------------------
	// Is the pick code given within the high and low pick codes assigned
	// to this node?
	//----------------------------------------------------------------------------
	bool ContainsPickCode(envType::UInt32 i_PickCode) const;

	//--------------------------------------------------------------------
	// Set color for fragment only (don't set the base color)
	//--------------------------------------------------------------------
	void ModifyColor(const maFloatRGBA &i_Color);

	//--------------------------------------------------------------------
	// Set color for fragment and base color
	//--------------------------------------------------------------------
	void SetColor(const maFloatRGBA &i_Color);

	//--------------------------------------------------------------------
	// Get base color
	//--------------------------------------------------------------------
	const maFloatRGBA& GetColor();

	//----------------------------------------------------------------------------
	// ModifyEmissive - alters emissive color by given factor, used
	//	to highlight parts.
	//----------------------------------------------------------------------------
	void ModifyEmissive( const float i_EmissiveFactor );

	//--------------------------------------------------------------------
	//	DisplayActive changes the color of the icon to represent
	//		when the object is active or disabled
	//--------------------------------------------------------------------
	void DisplayActive(bool i_bActive);

	//--------------------------------------------------------------------
	//	SetLayerScale changes the scale of the compass for only a 
	// given icon layer (a render panel)
	//--------------------------------------------------------------------
	void SetLayerScale(int i_IconLayerIndex, const maPoint3d& i_Scale);
	maPoint3d GetLayerScale(int i_IconLayerIndex) const;

	//--------------------------------------------------------------------
	//	SetLayerRenderable sets the compass visibility for only a given icon 
	// layer (a render panel)
	//--------------------------------------------------------------------
	void SetLayerRenderable(int i_IconLayerIndex, bool i_bRender);

private:
	//--------------------------------------------------------------------
	// shared constructor code
	//--------------------------------------------------------------------
	void Init(api3dObjectSimple *i_pBaseObj, 
			  matMaterial *i_pMaterial,
			  cmpsRenderLayer::RenderLayer i_RenderLayer);

	icnIconSet* m_pObject;
	gpxIconSet* m_pObjectProxy;
	effSolidData m_ColorData;
	gpxSolidData* m_pMaterialProxy;
	maFloatRGBA m_Color;
	float m_Factor;
	bool m_bActive;
};
