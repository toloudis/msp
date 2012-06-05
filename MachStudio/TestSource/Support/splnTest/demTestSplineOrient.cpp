/*****************************************************************************
**  demTestSplineOrient.cpp
**
**		This mode tests spline code from MachStudio
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "demTestSplineOrient.hpp"

#include "demModeManager.hpp"

#include "splnSpline.hpp"
#include "splnCurveMgr.hpp"

#include "api3dImport.hpp"
#include "api3dLightMgr.hpp"
#include "api3dObjectSimple.hpp"
#include "api3dScene.hpp"
#include "appCharEvent.hpp"
#include "appSimTime.hpp"
#include "appTime.hpp"
#include "cam3dMgr.hpp"
#include "dbgLog.hpp"
#include "envSTLHelpers.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "fsResourceFinderDir.hpp"
#include "g2dRGBColor.hpp"
#include "g3dFragment.hpp"
#include "g3dFragmentCreate.hpp"
#include "g3dViewer.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dPointLight.hpp"
#include "gfPaths.hpp"
#include "inDeviceMgr.hpp"
#include "matMaterial.hpp"
#include "matTextureMgr.hpp"
#include "matTexture.hpp"
#include "maConstants.hpp"
#include "mayExceptionX.hpp"
#include "tma3dCursorMgr.hpp"


namespace
{
	const float c_SplineDelta = 0.01f;
	const float c_SplineTime = 10.0f;

	//--------------------------------------------------------------------
	// calculate the orientation based on the tangent
	//--------------------------------------------------------------------
	maRotation calculate_orientation_goal( maPoint3d& i_Tangent )
	{
		maRotation orientation;

		// calculate the orientation based on the tangent
		//
		//	Get "pitch" and "yaw" from the tangent using atan2, etc. and 
		//	then reform a rotation from these.  This will keep the object 
		//	upright (like a bird), and you can enforce certain limits on 
		//	the pitch if you want.  This decays if the tangent points 
		//	straight up however.
		//

		float yaw, pitch, roll;
		yaw		= atan2( i_Tangent.GetX(), i_Tangent.GetZ() );
		pitch	= asin( -i_Tangent.GetY() );
		roll	= 0.0f;
		orientation.SetEuler( pitch, yaw, roll );

		return orientation;
	}

}

//====================================================================
//====================================================================
demTestSplineOrient::demTestSplineOrient(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer),
	m_pObject(NULL), m_SplinePerc(0), m_bAnimate(true)
{
}

//====================================================================
//====================================================================
demTestSplineOrient::~demTestSplineOrient()
{
}

//====================================================================
//====================================================================
void demTestSplineOrient::Initialize()
{
	Camera().SetClip(1.0f, 16.0f);
	demTestMode::Initialize();

	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push("Pony.chx");


	try
	{
		// Load Model 
		m_pObject = api3dImport::LoadObject(locator);

		std::string loc_fname;
		fsFileUtil::LocatorToANSIFilename(locator, loc_fname);
		DBG_ASSERT1(m_pObject, "Error loading file %s", loc_fname.c_str());

		api3dScene::AddObject(m_pObject);
	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		DBG_WARNING1("File not found: %s", filename.c_str());
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
	}
	catch( const mayInvalidModelFileX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		DBG_WARNING1("Invalid Model File: %s", filename.c_str());
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
	}
	catch( ... )
	{
		DBG_WARNING0("Unknown error");
		throw;
	}


	//api3dLightMgr::SetAmbient(maFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f));

	m_pDirLight = api3dLightMgr::CreateDirectionalLight();
	m_pDirLight->SetDirection(maVector3d(0.0f, -1.0f, -1.0f));
	m_pDirLight->Enable();


	// Make the spline
	m_pCurve = new splnSpline();
	const int num_pts = 5;
	maPoint3d pts[5];
	pts[0].Set(54, 0, -9);
	pts[1].Set(13.6f, 13, 20.6f);
	pts[2].Set(-40, 20, 80);
	pts[3].Set(-40, 20, 85);
	pts[4].Set(-40, 20, 90);
	m_pCurve->SetPoints(pts, num_pts);
	splnCurveMgr::AddCurve("Test", m_pCurve, NULL);

	// Make a fragment to display the fragment
	matMaterial *pMaterial = new matMaterial();
	pMaterial->SetEmissive(1.0f, 1.0f, 0.0f, 1.0f);
	pMaterial->SetDiffuse(0.0f, 0.0f, 0.0f, 1.0f);
	pMaterial->SetAmbient(0.0f, 0.0f, 0.0f, 1.0f);
	maPoint3d lverts[2];
	lverts[0].Set(0,0,0);
	lverts[1].Set(0,1,0);
	maVector3d lnorms[2];
	lnorms[0].Set(0,1,0);
	lnorms[1].Set(0,1,0);
	unsigned short indices[2];
	indices[0] = 0;
	indices[1] = 1;
	const bool morphable = true;
	m_pLineFrag = g3dFragmentCreate::CreateLineList( lverts,
						 	lnorms, 2, indices, 2, pMaterial, morphable );
	m_pTangentObj = new api3dObjectSimple(m_pLineFrag, pMaterial);
	api3dScene::AddObject(m_pTangentObj);

}

//====================================================================
//	Think
//====================================================================
void demTestSplineOrient::Think()
{
	demTestMode::Think();

	float frame_time = appTime::GetTime();

	inDeviceMgr::Think();
	tma3dCursorMgr::Think();


	// Orient object to spline
	if (m_bAnimate)
	{
		float delta = appSimTime::GetDelta() / c_SplineTime;
		m_SplinePerc += delta;
		while (m_SplinePerc > 1.0f) m_SplinePerc -= 1.0f;

		//float anim_time = ::fmodf(frame_time, c_SplineTime);
		//m_SplinePerc = anim_time / c_SplineTime;
	}
	maVector3d tangent;
	maPoint3d pos = m_pCurve->Evaluate(m_SplinePerc, &tangent);
	maRotation orient = calculate_orientation_goal(tangent);

	m_pObject->SetPosition(pos);
	m_pObject->SetOrientation(orient);

	maPoint3d new_verts[2];
	new_verts[0] = pos;
	new_verts[1] = pos + tangent * 4.0f;
	m_pLineFrag->UpdateVertices(2, new_verts);

	cam3dMgr::GetCameraManip()->SetTarget(pos);


	//api3dScene::Think( frame_time );
	cam3dMgr::Think();

	//	render
	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demTestSplineOrient::DeInitialize()
{	
	splnCurveMgr::DeleteCurve(m_pCurve);

	api3dScene::RemoveObject(m_pObject);
	delete m_pObject;

	api3dScene::RemoveObject(m_pTangentObj);
	delete m_pTangentObj;	// owns material and fragment

	api3dLightMgr::DestroyLight( m_pDirLight );

	demTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demTestSplineOrient::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
	case itString::CharType('f'):
	case itString::CharType('F'):
		m_bAnimate = false;
		m_SplinePerc += c_SplineDelta;
		break;
	case itString::CharType('b'):
	case itString::CharType('B'):
		m_bAnimate = false;
		m_SplinePerc -= c_SplineDelta;
		break;
	case itString::CharType('r'):
	case itString::CharType('R'):
		m_bAnimate = true;
		break;
	case itString::CharType('s'):
	case itString::CharType('S'):
		m_bAnimate = false;
		break;
	}

	demTestMode::ReceiveCharEvent(i_Event);
}

