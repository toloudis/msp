/*****************************************************************************
**	cmpsObjectSimple.hpp
**
**	Derived class, represents an element of geometry for a compass part. 
**	Adds on some convenience functions to the api3dObjectSimple api 
**	for setting color states on the compass part.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_OBJECTSIMPLE_HPP
#error cmpsObjectSimple.hpp multiply included
#endif
#define CMPS_OBJECTSIMPLE_HPP

#ifndef API3D_OBJECTSIMPLE_HPP
#include "Tool/api3d/api3dObjectSimple.hpp"
#endif


//============================================================================
//============================================================================
const float lc_CompassEmissiveNormal	= 0.5f;
const float lc_CompassEmissiveHighlight	= 1.0f;


//============================================================================
//============================================================================
class cmpsObjectSimple : public api3dObjectSimple
{
public:
	//--------------------------------------------------------------------
	// constructor - this object assumes ownership of the arguments.
	//	It will also assign the material to the fragment.
	//--------------------------------------------------------------------
	cmpsObjectSimple(g3dFragment* i_pFragment, matMaterial *i_pMaterial);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmpsObjectSimple();

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

private:
	maFloatRGBA m_Color;
	float m_Factor;
	bool m_bActive;
};
