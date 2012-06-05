/*****************************************************************************
**	scBillboard.cpp
**
**		scBillboard represents a two poly model with a texture that faces
**		the camera
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/sc/scBillboard.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"
//#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effBillboardData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"

#include <list>


//====================================================================
// Anonymous Namespace for local variables and functions
//====================================================================
namespace
{
	// list of billboards to update when camera moves
	std::list<scBillboard*> l_Billboards;
}

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
scBillboard::scBillboard()
:	m_pFragment( NULL ), m_bOrientToCamera(true), m_bSnapToCamera(false), m_DistToCamera(0.0f)
{

	// Vertices
	maPoint3d vertices[4];
	vertices[0].Set( -0.5f,  0.5f, 0.0f );
	vertices[1].Set(  0.5f,  0.5f, 0.0f );
	vertices[2].Set( -0.5f, -0.5f, 0.0f );
	vertices[3].Set(  0.5f, -0.5f, 0.0f );

	// Normals
	maPoint3d normals[4];
	normals[0].Set( 0, 0, 1 );
	normals[1].Set( 0, 0, 1 );
	normals[2].Set( 0, 0, 1 );
	normals[3].Set( 0, 0, 1 );

	// Texture vertices
	maPoint2d textureVertices[4];
	textureVertices[0].Set( 0, 0 );
	textureVertices[1].Set( 1, 0 );
	textureVertices[2].Set( 0, 1 );
	textureVertices[3].Set( 1, 1 );

	// Indices
	unsigned short indices[6];
	indices[0] = 2;
	indices[1] = 1;
	indices[2] = 0;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 1;

	// Set up material
	m_pMaterial = new matMaterial("Billboard.fx");

	m_pMaterial->TypedData<effTexturedData>()->m_Color = maFloatRGBA( 1, 1, 1, 1 );
	m_pMaterial->SetHasSpecular( false );

	// Create the fragment
	m_pFragment = g3dFragmentCreate::CreateFragment(vertices,
											normals,
											textureVertices,
											4,
											indices,
											6,
											m_pMaterial );
	
	GetBase()->SetFragment( m_pFragment );
	m_pFragment->SetReceivesGI(false);
	m_pFragment->SetReceivesOcclusion(false);

	l_Billboards.push_back(this);
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
scBillboard::~scBillboard()
{
	envSTLHelpers::RemoveOneValue( l_Billboards, this);

	delete m_pFragment;
	delete m_pMaterial;
}


//--------------------------------------------------------------------
//	SetCameraPosition - updates all billboards before rendering
//--------------------------------------------------------------------
//static 
void scBillboard::SetCameraPosition( const camCamera &i_Camera )
{
	maPoint3d cameraPos = i_Camera.GetPosition();
	std::list<scBillboard*>::iterator it, end = l_Billboards.end();
	for (it = l_Billboards.begin(); it != end; ++it)
	{
		if ((*it)->m_bSnapToCamera)
		{
			(*it)->SnapBillboard();
		}
		else if((*it)->m_bOrientToCamera)
		{
			(*it)->OrientBillboard(cameraPos);
		}
	}
}

//--------------------------------------------------------------------
//	OrientBillboard - turns this billboard to face 
//		this camera position
//--------------------------------------------------------------------
void scBillboard::OrientBillboard( const maPoint3d &i_CameraPos )
{
	g3dSceneNode* pBase = GetBase();

	maPoint3d world_position( 0.0f, 0.0f, 0.0f );
	const maMatrix4x4& model_transform = pBase->GetTotalTransform();
	model_transform.Transform( world_position );	

	// Calculate forward vector
	maVector3d forward = i_CameraPos - world_position;
	forward.Normalize();

	// Calculate left vector
	maVector3d straight_up( 0, 1, 0 );
	maVector3d left = straight_up.Cross( forward ); 
	if (!left.Normalize())
		left.Set(1, 0, 0);

	// Calculate up vector
	maVector3d up = forward.Cross( left );
	up.Normalize();

	// Construct rotation matrix
	maMatrix4x4 rotation_matrix;	
	rotation_matrix( 0, 0 ) = left.m_X;
	rotation_matrix( 0, 1 ) = left.m_Y;
	rotation_matrix( 0, 2 ) = left.m_Z;

	rotation_matrix( 1, 0 ) = up.m_X;
	rotation_matrix( 1, 1 ) = up.m_Y;
	rotation_matrix( 1, 2 ) = up.m_Z;

	rotation_matrix( 2, 0 ) = forward.m_X;
	rotation_matrix( 2, 1 ) = forward.m_Y;
	rotation_matrix( 2, 2 ) = forward.m_Z;

	// Construct our transform matrix
	maMatrix4x4 transform;

	// Scale
	const maPoint3d& scale = GetScale();
	transform.MakeScale( scale.m_X, scale.m_Y, scale.m_Z );

	// Orient
	transform *= rotation_matrix;

	// Remove parent orientation to make it model space
	maMatrix4x4 parent_inverse_orient;
	const g3dSceneNode* pParent = pBase->GetParent();
	if( !pBase->GetIgnoreParentTransform() && pParent )
	{
		maPoint3d parent_pos( 0, 0, 0 );
		pParent->GetTotalTransform().Transform( parent_pos );

		parent_inverse_orient = pParent->GetTotalTransform().GetSubMatrix( 3, 3 );
		parent_inverse_orient.Invert();
	}
	transform *= parent_inverse_orient;

	// Translate
	const maPoint3d& position = GetPosition();
	transform.TranslateBy( position.m_X, position.m_Y, position.m_Z );

	GetBase()->SetTransform( transform );

	// Make sure the total transform and bounding box are updated also,
	// since we might need to update the billboard per camera in a multi-panel display.
	GetBase()->UpdateTotalTransform();
}

//--------------------------------------------------------------------
//	OrientBillboard - turns this billboard to face 
//		this camera position
//--------------------------------------------------------------------
void scBillboard::SnapBillboard()
{
	if (!m_Camera.get())
		return;

	g3dSceneNode* pBase = GetBase();

	// Calculate forward vector
	maVector3d forward = (m_Camera->GetPosition() - m_Camera->GetTarget());
	forward.Normalize();

	maPoint3d world_position = m_Camera->GetPosition() - forward * (m_Camera->GetNearClip() + m_DistToCamera + 0.5f);

	// Calculate left vector
	maVector3d straight_up = m_Camera->GetUp();
	straight_up.Normalize();
	maVector3d left = straight_up.Cross( forward ); 
	if (!left.Normalize())
		left.Set(1, 0, 0);

	// Calculate up vector
	/*maVector3d up = forward.Cross( left );
	up.Normalize();*/

	// Construct rotation matrix
	maMatrix4x4 rotation_matrix;	
	rotation_matrix( 0, 0 ) = left.m_X;
	rotation_matrix( 0, 1 ) = left.m_Y;
	rotation_matrix( 0, 2 ) = left.m_Z;

	rotation_matrix( 1, 0 ) = straight_up.m_X;
	rotation_matrix( 1, 1 ) = straight_up.m_Y;
	rotation_matrix( 1, 2 ) = straight_up.m_Z;

	rotation_matrix( 2, 0 ) = forward.m_X;
	rotation_matrix( 2, 1 ) = forward.m_Y;
	rotation_matrix( 2, 2 ) = forward.m_Z;

	// Construct our transform matrix
	maMatrix4x4 transform;

	// Calculate scale
	float dist = (m_Camera->GetPosition() - world_position).Length();
	//float t = i_Camera.GetFOV();
	//float wscale = dist * tan(i_Camera.GetFOV());
	//float hscale = wscale / i_Camera.GetAspect();

	float wscale = dist * tan(m_Camera->GetFOV() * maConstants::c_fAngleToRad * 0.5f) * 2.0f;
	float hscale = wscale / m_Camera->GetAspect();
	maPoint3d scale = maPoint3d(wscale, hscale, 1.0f);
	transform.MakeScale( scale.m_X, scale.m_Y, scale.m_Z );

	// Orient
	transform *= rotation_matrix;

	// Remove parent orientation to make it model space
	maMatrix4x4 parent_inverse_orient;
	const g3dSceneNode* pParent = pBase->GetParent();
	if( !pBase->GetIgnoreParentTransform() && pParent )
	{
		maPoint3d parent_pos( 0, 0, 0 );
		pParent->GetTotalTransform().Transform( parent_pos );

		parent_inverse_orient = pParent->GetTotalTransform().GetSubMatrix( 3, 3 );
		parent_inverse_orient.Invert();
	}
	transform *= parent_inverse_orient;

	// Translate
	//const maPoint3d& position = i_Camera.GetTarget();
	transform.TranslateBy( world_position.m_X, world_position.m_Y, world_position.m_Z );

	GetBase()->SetTransform( transform );

	// Make sure the total transform and bounding box are updated also,
	// since we might need to update the billboard per camera in a multi-panel display.
	GetBase()->UpdateTotalTransform();
}

//--------------------------------------------------------------------
// If OrientToCamera is true (the default) the billboard will
// rotate to face the camera.
//--------------------------------------------------------------------
void scBillboard::SetOrientToCamera(bool i_bOrient)
{
	m_bOrientToCamera = i_bOrient;
}

//--------------------------------------------------------------------
// If SnapToCamera is true the billboard will snap to camera view
// and scale to the camera aspect
//--------------------------------------------------------------------
void scBillboard::SetSnapToCamera(bool i_bSnap)
{
	m_bSnapToCamera = i_bSnap;
}

//--------------------------------------------------------------------
// Set/Get billboard distance to camera
//--------------------------------------------------------------------
void scBillboard::SetDistToCamera(float i_Dist)
{
	m_DistToCamera = i_Dist;
}

//--------------------------------------------------------------------
// Set attached camera
//--------------------------------------------------------------------
void scBillboard::SetCamera(shared_ptr<camCamera> i_Cam)
{
	m_Camera = i_Cam;
}