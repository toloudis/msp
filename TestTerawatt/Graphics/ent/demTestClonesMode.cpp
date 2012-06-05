/*****************************************************************************
**  demTestClonesMode.cpp
**
**		This mode tests creating more than one entity from a template
**	and making sure animations run independently.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demTestClonesMode.hpp"

#include "demModeManager.hpp"

#include "appCharEvent.hpp"
#include "appTime.hpp"
#include "entAnimation.hpp"
//#include "entAnimMaterialData.hpp"
#include "entEntity.hpp"
#include "entEntityTemplate.hpp"
#include "entImport.hpp"
#include "entSceneWorld.hpp"
//#include "entMaterialEvent.hpp"
#include "envSTLHelpers.hpp"
#include "fsResourceFinderDir.hpp"
#include "g2dRGBColor.hpp"
#include "g2dWindow.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightMgr.hpp"
#include "g3dViewer.hpp"
#include "gfPaths.hpp"
#include "gfPlatformFileDialog.hpp"
#include "maConstants.hpp"
#include "matMaterial.hpp"

#include <algorithm>

namespace
{

bool		l_bShowInstructions = true;
itString	l_ModelName;
itString	l_AnimName;

enum Anims
{
	e_Basic,
	e_NumAnims
};

}


//====================================================================
//====================================================================
demTestClonesMode::demTestClonesMode(g3dViewer &i_Viewer)
: demViewerMode(i_Viewer), m_Viewer(i_Viewer)
{

}

//====================================================================
//====================================================================
demTestClonesMode::~demTestClonesMode()
{
}

//====================================================================
//====================================================================
void demTestClonesMode::Initialize()
{
	demViewerMode::Initialize();

	m_Scene = new entSceneWorld();
	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	m_Viewer.SetTextMessage(0, itString("Clone Test"));

	this->SetPitch(30.0f * maConstants::c_fAngleToRad);
	this->SetYaw(270.0f * maConstants::c_fAngleToRad);
	this->SetRadius(50.0f);

	// Init for file selection dialog
	m_DataPath = gfPaths::GetGamePath(gfPaths::e_ExePath);
	m_DataPath.Push(itString("Data"));

	// all textures are in data directory
	fsResourceFinderDir finder(m_DataPath);

	// Load dragon model and anims
	//
	fsLocator locator = m_DataPath;
	locator.Push(itString("dr1dr.jnx"));

	entModelTemplate *mdl_template = entImport::LoadGeometry(locator, finder);
	entEntityTemplate *ent_template = dynamic_cast<entEntityTemplate*>(mdl_template);
	m_EntityTemplates.push_back(ent_template);

	locator.Pop();
	locator.Push(itString("dr1dr100.jna"));
	entAnimKeys *anim_keys = entImport::LoadAnimKeys(locator);
	ent_template->AddAnimKeys(anim_keys, locator);
	entAnimation* animation = entImport::CreateAnimation(*anim_keys);
	ent_template->AddAnimation(animation, 0);

	// Add material animation event to animation
	//entAnimMaterialData mat_data; // default will activate all anims
	//mat_data.SetFrame(46.0f);
	//mat_data.SetLifetime(1.0f);
	//entMaterialEvent *mat_event = new entMaterialEvent(mat_data);
	//animation->AddAnimEvent(mat_event, mat_data.GetFrame());

	// Load hierarchical model and anims
	//
	locator.Pop();
	locator.Push(itString("dr1ht.mhx"));

	mdl_template = entImport::LoadGeometry(locator, finder);
	ent_template = dynamic_cast<entEntityTemplate*>(mdl_template);
	m_EntityTemplates.push_back(ent_template);

	locator.Pop();
	locator.Push(itString("dr1ht.mha"));
	anim_keys = entImport::LoadAnimKeys(locator);
	ent_template->AddAnimKeys(anim_keys, locator);
	ent_template->AddAnimation(entImport::CreateAnimation(*anim_keys), 0);


	// Set up clones
	entEntity *entity = new entEntity(*m_EntityTemplates[0]);
	entity->SetAnimation(0, appTime::GetTime());
	m_Scene->AddEntity(entity);
	m_Entities.push_back(entity);

	entity = new entEntity(*m_EntityTemplates[1]);
	entity->SetPosition(maPoint3d(80,0,0));
	entity->SetAnimation(0, appTime::GetTime());
	m_Scene->AddEntity(entity);
	m_Entities.push_back(entity);

	entity = new entEntity(*m_EntityTemplates[0]);
	entity->SetPosition(maPoint3d(0,0,80));
	entity->SetAnimation(0, appTime::GetTime() - 1.5f); // 1.5 second lag
	m_Scene->AddEntity(entity);
	m_Entities.push_back(entity);

	entity = new entEntity(*m_EntityTemplates[1]);
	entity->SetPosition(maPoint3d(80,0,80));
	//entity->SetAnimation(0, appTime::GetTime());	// no anim
	m_Scene->AddEntity(entity);
	int num_mats = entity->GetUniqueMaterials().size();
	for (int m=1; m<num_mats; m++) // leave first alone for variety
	{
		entity->GetUniqueMaterials()[m]->SetEmissive(maFloatRGBA(0,0,1,1));
	}
	m_Entities.push_back(entity);

	reset_camera();

	//	set lights
	m_Light1 = g3dLightMgr::CreateDirectionalLight();
	m_Light2 = g3dLightMgr::CreateDirectionalLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(0.35f, 0.35f, 0.3f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.3f, 0.3f, 0.35f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();

	g3dLightMgr::SetAmbient(maFloatRGBA(0.1f, 0.1f, 0.1f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demTestClonesMode::Think()
{
	demViewerMode::Think();

	float frame_time = appTime::GetTime();

	// animate
	m_Scene->Think(frame_time);

	//	render
	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demTestClonesMode::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	envSTLHelpers::DeleteContainer(m_Entities);
	envSTLHelpers::DeleteContainer(m_EntityTemplates);

	m_Viewer.ClearTextMessages();
	delete m_Scene;

	demViewerMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demTestClonesMode::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType('r'):
		case itString::CharType('R'):
			this->reset_camera();
		break;

		default:
			demViewerMode::ReceiveCharEvent(i_Event);
			break;
	}
}

//====================================================================
//====================================================================
void demTestClonesMode::reset_camera()
{
	maAxisBox bbox, total_bbox;
	for (int i=0; i<m_Entities.size(); i++)
	{
		m_Entities[i]->ComputeWorldBox(bbox);
		total_bbox.Union(bbox);
	}

	this->SetTarget( total_bbox.GetCenter() );
	float radius = total_bbox.GetRadius();
	this->SetRadius( radius * 4.0f );
}
