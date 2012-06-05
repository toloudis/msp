/*****************************************************************************
**  cmpsCompassObjectTranslate.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassObjectTranslate.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maFloatRGBA.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Support/cmps/private/cmpsObjectSimple.hpp"
#include "Graphics/g3d/g3dScene.hpp"

#include <algorithm>

namespace
{
	float l_fConeHeight = 1;
	float l_fConeRadius = 0.3f;
	float l_fSphereRadius = 0.1f;
	float l_fLineLength = 5.0f;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectTranslate::cmpsCompassObjectTranslate(int i_RenderLayer)
:	cmpsCompassObject3Axis(i_RenderLayer)
{
	maRotation rot;

	// create x object and line
	cmpsObjectSimple* pObject = Create_Cone(maFloatRGBA( 1.0f, 0.0f, 0.0f, 1.0f ));
	Add_Object(pObject);
	cmpsObjectSimple* pLine = Create_Line(cmpsCompassObject3Axis::e_AxisX, maFloatRGBA( 1.0f, 0.0f, 0.0f, 1.0f ));
	Add_Line(pLine);
	pObject->SetPosition( maPoint3d(l_fLineLength,0.0f,0.0f));
	rot.SetValue( maVector3d(0, 1, 0), maVector3d(1, 0, 0));
	pObject->SetOrientation(rot);
	//pLine->SetOrientation(rot);

	// create y object and line
	pObject = Create_Cone(maFloatRGBA( 0.0f, 1.0f, 0.0f, 1.0f ));
	Add_Object(pObject);
	pLine = Create_Line(cmpsCompassObject3Axis::e_AxisY, maFloatRGBA( 0.0f, 1.0f, 0.0f, 1.0f ));
	Add_Line(pLine);
	pObject->SetPosition( maPoint3d(0.0f,l_fLineLength,0.0f));

	// create y object and line
	pObject = Create_Cone(maFloatRGBA( 0.0f, 0.0f, 1.0f, 1.0f ));
	Add_Object(pObject);
	pLine = Create_Line(cmpsCompassObject3Axis::e_AxisZ, maFloatRGBA( 0.0f, 0.0f, 1.0f, 1.0f ));
	Add_Line(pLine);
	pObject->SetPosition( maPoint3d(0.0f,0.0f,l_fLineLength));
	rot.SetValue( maVector3d(0, 1, 0), maVector3d(0, 0, 1));
	pObject->SetOrientation(rot);
	//pLine->SetOrientation(rot);

	// create the center sphere
	Add_Center(Create_Center(maFloatRGBA( 1.0f, 1.0f, 0.0f, 1.0f )));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectTranslate::~cmpsCompassObjectTranslate()
{
}

//----------------------------------------------------------------------------
//	GetDefaultSize() - get the default size of the shape
//----------------------------------------------------------------------------
//virtual
float cmpsCompassObjectTranslate::GetDefaultSize() const
{
	return l_fLineLength;
}

//--------------------------------------------------------------------
//	Create_Cone creates a object
//--------------------------------------------------------------------
cmpsObjectSimple* cmpsCompassObjectTranslate::Create_Cone( const maFloatRGBA& i_Color )
{
	g3dFragment* pFragment = g3dPrimitiveFragmentUtil::CreateCone( l_fConeRadius, l_fConeHeight, 15 );
	cmpsObjectSimple* pObject = new cmpsObjectSimple( pFragment, 
		new matMaterial("Phong.fx"));
	pObject->SetColor( i_Color );
	pObject->ModifyEmissive( lc_CompassEmissiveNormal );
	pObject->SetRenderable( true );
	return pObject;
}

