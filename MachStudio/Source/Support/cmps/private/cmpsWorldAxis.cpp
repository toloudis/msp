/*****************************************************************************
**  cmpsWorldAxis.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/cmps/cmpsWorldAxis.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/Eff/effSolidData.hpp"
#include "Graphics/ent/entModelTemplate.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/G3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/Sc/scObject.hpp"
#include "Tool/api3d/api3dObjectGeom.hpp"
#include "Tool/gpx/gpxSceneObject.hpp"


//============================================================================
//============================================================================
namespace
{
	enum PartsIndex
	{
		e_XIndex = 0,
		e_YIndex = 1,
		e_ZIndex = 2
	};

	float l_fConeHeight = 1;
	float l_fConeRadius = 0.3f;
	float l_fSphereRadius = 0.1f;
	float l_fLineLength = 5.0f;

	const maFloatRGBA c_Black(0.0f, 0.0f, 0.0f, 1.0f);

	matMaterial* create_material(const maFloatRGBA& i_Color)
	{
		matMaterial* pMaterial = new matMaterial("Solid.fx");
		effSolidData* pData = dynamic_cast<effSolidData*>(pMaterial->GetEffectData());
		if (pData != NULL) {
			DBG_ASSERT(pData != NULL, "cmpsWorldAxis not using effSolid");

			pData->m_Color = i_Color;
		}
		return pMaterial;
	}
	
	g3dSceneNode* create_cone(matMaterial *i_pMaterial,
							  entModelTemplate* io_pTemplate)
	{
		g3dFragment* pFragment = g3dPrimitiveFragmentUtil::CreateCone( l_fConeRadius, l_fConeHeight, 15 );	
		pFragment->SetMaterial(i_pMaterial);
		io_pTemplate->Fragments().push_back(pFragment);

		g3dSceneNode* pSceneNode = new g3dSceneNode(pFragment);

		maMatrix4x4 transform;
		transform.MakeTranslate(0.0f,l_fLineLength,0.0f);
		pSceneNode->SetTransform(transform);

		return pSceneNode;
	}

	g3dSceneNode* create_line(matMaterial *i_pMaterial,
							  entModelTemplate* io_pTemplate)
	{

		maPoint3d line_vertices[2];
		line_vertices[0].Set( 0, 0, 0 );
		line_vertices[1].Set( 0, l_fLineLength, 0 );

		maPoint3d line_normals[2];
		line_normals[0].Set(0,1,0);
		line_normals[1].Set(0,1,0);

		unsigned short indices[2];
		indices[0] = 0;
		indices[1] = 1;

		g3dFragment* pFragment = g3dFragmentCreate::CreateLineList( line_vertices,
						 									 line_normals,
															 2,
															 indices,
															 2,
															 i_pMaterial );
		io_pTemplate->Fragments().push_back(pFragment);

		g3dSceneNode* pSceneNode = new g3dSceneNode(pFragment);
		return pSceneNode;
	}

	g3dSceneNode* create_center(matMaterial *i_pMaterial,
							  entModelTemplate* io_pTemplate)
	{
		g3dFragment* pFragment = g3dPrimitiveFragmentUtil::CreateSphere( l_fSphereRadius, 15, 15 );	
		pFragment->SetMaterial(i_pMaterial);
		io_pTemplate->Fragments().push_back(pFragment);

		g3dSceneNode* pSceneNode = new g3dSceneNode(pFragment);
		return pSceneNode;
	}
	
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmpsWorldAxis::cmpsWorldAxis()
:	m_pObject(NULL)
{
	std::unique_ptr<entModelTemplate> model_template(new entModelTemplate());

	// Create pure color materials
	matMaterial *pRedMaterial = create_material(maFloatRGBA( 1.0f, 0.0f, 0.0f, 1.0f ));
	model_template->Materials().push_back(pRedMaterial);
	matMaterial *pGreenMaterial = create_material(maFloatRGBA( 0.0f, 1.0f, 0.0f, 1.0f ));
	model_template->Materials().push_back(pGreenMaterial);
	matMaterial *pBlueMaterial = create_material(maFloatRGBA( 0.0f, 0.0f, 1.0f, 1.0f ));
	model_template->Materials().push_back(pBlueMaterial);
	matMaterial *pCenterMaterial = create_material(maFloatRGBA( 1.0f, 1.0f, 0.0f, 1.0f ));
	model_template->Materials().push_back(pCenterMaterial);

	// Construct whole structure under a single root node
	m_pRootNode = new g3dSceneNode;

	// create x object and line
	g3dSceneNode *pXAxisNode = new g3dSceneNode;
	pXAxisNode->AddChild( create_cone(pRedMaterial, model_template.get()) );
	pXAxisNode->AddChild( create_line(pRedMaterial, model_template.get()) );
	maRotation x_rot;
	x_rot.SetValue( maVector3d(0, 1, 0), maVector3d(1, 0, 0));
	pXAxisNode->SetTransform(x_rot.GetMatrix());
	m_pRootNode->AddChild(pXAxisNode);

	// create y object and line
	g3dSceneNode *pYAxisNode = new g3dSceneNode;
	pYAxisNode->AddChild( create_cone(pGreenMaterial, model_template.get()) );
	pYAxisNode->AddChild( create_line(pGreenMaterial, model_template.get()) );
	m_pRootNode->AddChild(pYAxisNode);

	// create z object and line
	g3dSceneNode *pZAxisNode = new g3dSceneNode;
	pZAxisNode->AddChild( create_cone(pBlueMaterial, model_template.get()) );
	pZAxisNode->AddChild( create_line(pBlueMaterial, model_template.get()) );
	maRotation z_rot;
	z_rot.SetValue( maVector3d(0, 1, 0), maVector3d(0, 0, 1));
	pZAxisNode->SetTransform(z_rot.GetMatrix());
	m_pRootNode->AddChild(pZAxisNode);

	// create the center sphere
	m_pRootNode->AddChild( create_center(pCenterMaterial, model_template.get()) );

	scObject *pObject = new scObject(m_pRootNode);
	m_pObject = new api3dObjectGeom(model_template.release(), NULL, pObject);

	m_pObjectProxy = new gpxSceneObject(*m_pObject);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmpsWorldAxis::~cmpsWorldAxis()
{
	delete m_pObjectProxy;
	delete m_pObject;
	m_pObject = NULL;
}

//--------------------------------------------------------------------
// Get root scene node in order to add and remove it from layers
//--------------------------------------------------------------------
g3dSceneNode* cmpsWorldAxis::GetSceneNode()
{
	return m_pRootNode;
}

//----------------------------------------------------------------------------
//	SetPosition sets the position of the compass.
//----------------------------------------------------------------------------
void cmpsWorldAxis::SetPosition(const maPoint3d& i_Position)
{
	if (m_pObjectProxy->GetPosition() != i_Position)
		m_pObjectProxy->SetPosition( i_Position );
}

//----------------------------------------------------------------------------
//	SetScale changes the scale of the compass.
//----------------------------------------------------------------------------
void cmpsWorldAxis::SetScale(const maPoint3d& i_Scale)
{
	//if ( GetLockScale() )
	//	return;

	if (m_pObjectProxy->GetScale() != i_Scale)
		m_pObjectProxy->SetScale(i_Scale);
}

//----------------------------------------------------------------------------
//	SetOrientation changes the orientation of the compass.
//----------------------------------------------------------------------------
//virtual
void cmpsWorldAxis::SetOrientation( const maRotation& i_Orientation )
{
	if (m_pObjectProxy->GetOrientation() != i_Orientation)
		m_pObjectProxy->SetOrientation(i_Orientation);
}

//----------------------------------------------------------------------------
//	SetRenderable turns display of the compass on and off.
//----------------------------------------------------------------------------
void cmpsWorldAxis::SetRenderable(bool i_bRender)
{
	if (m_pObjectProxy->GetRenderable() != i_bRender)
		m_pObjectProxy->SetRenderable( i_bRender );
}

