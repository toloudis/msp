/*****************************************************************************
**  cmpsCompassObjectScale.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassObjectScale.hpp"

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
	float l_fCubeHeight = 1;
	float l_fSphereRadius = 0.1f;
	float l_cfLineLength = 5.0f;
	float l_cfMinScale	= 0.5f;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectScale::cmpsCompassObjectScale(int i_RenderLayer)
:	cmpsCompassObject3Axis(i_RenderLayer)
{
	//	for scale, the commented out code removes the visual lines
	//	this is to differentiate them from the other compasses.

	maRotation rot;
	cmpsObjectSimple* pObject;

	// create x object and line
	pObject = Create_Cylinder(maFloatRGBA( 1.0f, 0.0f, 0.0f, 1.0f ));
	Add_Object(pObject);
	cmpsObjectSimple* pLine = Create_Line(cmpsCompassObject3Axis::e_AxisX, maFloatRGBA( 1.0f, 0.0f, 0.0f, 1.0f ));
	Add_Line(pLine);
	pObject->SetPosition( maPoint3d(l_cfLineLength,0.0f,0.0f));
	rot.SetValue( maVector3d(0, 1, 0), maVector3d(1, 0, 0));
	pObject->SetOrientation(rot);
	//pLine->SetOrientation(rot);

	// create y object and line
	pObject = Create_Cylinder(maFloatRGBA( 0.0f, 1.0f, 0.0f, 1.0f ));
	Add_Object(pObject);
	pLine = Create_Line(cmpsCompassObject3Axis::e_AxisY, maFloatRGBA( 0.0f, 1.0f, 0.0f, 1.0f ));
	Add_Line(pLine);
	pObject->SetPosition( maPoint3d(0.0f,l_cfLineLength,0.0f));

	// create z object and line
	pObject = Create_Cylinder(maFloatRGBA( 0.0f, 0.0f, 1.0f, 1.0f ));
	Add_Object(pObject);
	pLine = Create_Line(cmpsCompassObject3Axis::e_AxisZ, maFloatRGBA( 0.0f, 0.0f, 1.0f, 1.0f ));
	Add_Line(pLine);
	pObject->SetPosition( maPoint3d(0.0f,0.0f,l_cfLineLength));
	rot.SetValue( maVector3d(0, 1, 0), maVector3d(0, 0, 1));
	pObject->SetOrientation(rot);
	//pLine->SetOrientation(rot);

	// create the center sphere
	Add_Center(Create_Center(maFloatRGBA( 1.0f, 1.0f, 0.0f, 1.0f )));

	//
	//SetParts( cmpsCompass::e_Y );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectScale::~cmpsCompassObjectScale()
{
}

//----------------------------------------------------------------------------
//	GetDefaultSize() - get the default size of the shape
//----------------------------------------------------------------------------
//virtual
float cmpsCompassObjectScale::GetDefaultSize() const
{
	return l_cfLineLength;
}

//----------------------------------------------------------------------------
//	SetScale changes the scale of the compass.
//----------------------------------------------------------------------------
void cmpsCompassObjectScale::SetScale(const maPoint3d& i_Scale)
{
	if ( GetLockScale() )
		return;

	cmpsCompassObject::SetScale( i_Scale );

	std::vector<cmpsObjectSimple*>::iterator it;

	// Scale cylinders uniformly so they don't deform
	float uniform_scale = (i_Scale.m_X + i_Scale.m_Y + i_Scale.m_Z) / 3.0f;
	for ( it = m_Objects.begin(); it != m_Objects.end(); ++it)
	{
		//(*it)->SetScale(i_Scale);
		(*it)->SetUniformScale(uniform_scale);
	}
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		(*it)->SetScale(i_Scale);
	}

	if (m_pCenter)
	{
		m_pCenter->SetScale(GetScale());
	}

	//DBG_LOG1( "Scale = %5.2f", GetScale().GetX() );

	// reset the object positions
	SetPosition(GetPosition());
}

//--------------------------------------------------------------------
//	Create_Cylinder creates a object
//--------------------------------------------------------------------
cmpsObjectSimple* cmpsCompassObjectScale::Create_Cylinder( const maFloatRGBA& i_Color )
{
	g3dFragment* pFragment = g3dPrimitiveFragmentUtil::CreateCylinder( 0.5f, 0.5f, 1.0f, 10 );
	cmpsObjectSimple* pObject = new cmpsObjectSimple( pFragment, 
		new matMaterial("Phong.fx") );
	pObject->SetColor( i_Color );
	pObject->ModifyEmissive( lc_CompassEmissiveNormal );
	pObject->SetRenderable( true );
	return pObject;
}

//--------------------------------------------------------------------
//	using the latest compass settings, modify the object's shape.
//--------------------------------------------------------------------
void cmpsCompassObjectScale::ResizeObject(const maPoint3d& i_CameraPos)
{
	// A rotation will be around the object's pivot position, 
	// so it is necessary for the rotation compass to be centered on that origin.
	// So, if the object's bbox is far from its origin, the rotate compass
	// will be large; but unfortunately, we don't have any choice.
	//
	//maPoint3d origin = this->GetPosition();
	maPoint3d origin = this->GetPivot();

	//	set the center of the compass to the pivot point
	this->SetPosition( origin );

	maAxisBox box	= GetBounds();
	box.Union(origin); 
	// Also negate all points around the origin 
	// and add them to the bbox
	maVector3d pts[8];
	box.GetBoxPoints(pts);
	for (int i=0; i<8; i++)
	{
		pts[i] -= origin;
		pts[i] *= -1.0f;
		pts[i] += origin;
	}
	box.Union(pts, 8);

	// Compute Camera Scale
	float dist = (i_CameraPos - origin).Length();
	const float c_Modifier = 0.15f;
	const float default_size = GetDefaultSize();
	float camera_scale = dist * c_Modifier / default_size; // could be some formula of distance

	// This whole function is based on the world box, but scaling is based on
	// the local axes. So, this won't set the scale of the axes correctly. 
	// Try to correct for this, by doing a uniform scale.

	//	scale the object accordingly
	//float scalex = box.GetDiffX() / default_size;
	//if (scalex < camera_scale) scalex = camera_scale;
	//float scaley = box.GetDiffY() / default_size;
	//if (scaley < camera_scale) scaley = camera_scale;
	//float scalez = box.GetDiffZ() / default_size;
	//if (scalez < camera_scale) scalez = camera_scale;

	//if (scalex < l_cfMinScale) scalex = l_cfMinScale;
	//if (scaley < l_cfMinScale) scaley = l_cfMinScale;
	//if (scalez < l_cfMinScale) scalez = l_cfMinScale;
	////DBG_LOG3( "select compass scale (%6.3f,%6.3f,%6.3f)", scalex, scaley, scalez );

	//SetScale( maVector3d( scalex, scaley, scalez ) );
	
	// uniform scale
	float scale = box.GetRadius() / default_size;
	if (scale < camera_scale) scale = camera_scale;
	if (scale < l_cfMinScale) scale = l_cfMinScale;
	SetScale( maVector3d(scale, scale, scale) );
}

