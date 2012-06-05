/*****************************************************************************
**	prtclObjectMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Object/prtclObjectMgr.hpp"

#include "Systems/Particles/GUI/prtclGeomList.hpp"
#include "Systems/Particles/GUI/prtclTextureList.hpp"

#include "Support/fsys/fsysFileUtil.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Graphics/an/an3StateAnimation.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/prt/prtConeParticleGenerator.hpp"
#include "Graphics/prt/prtGeneratorUtil.hpp"
#include "Graphics/prt/prtSpiralParticleGenerator.hpp"
#include "Tool/api3d/api3dImport.hpp"
#include "Tool/api3d/api3dParticleGenerator.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace
{
	int		l_ObjectCounter = 0;
	bool	l_bPaused = false;


	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void create_default_name( const itString& i_Filename, nameString& o_NameString )
	{
		char strName[64];
		char filename_base[64];
		char *filename;
		strcpy( filename_base, itStringUtil::GetStdString( i_Filename ).c_str() );
		filename = strtok( filename_base, "." );

		do
		{
			sprintf( strName, "%s-%03d", filename, l_ObjectCounter );
			o_NameString.SetString( strName );
			l_ObjectCounter++;
		} while ( !prtclObjectMgr::VerifyNodupeName(strName) );

		DBG_LOG1( "Added -- name (%s)", strName );
	}


}	// end of namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtclScriptObject* prtclObjectCreator::Create(const prtclScriptData &i_Data)
{
	// create actual point Particle

	//
	//	grab the filename + load the object
	fsLocator file_loc;
	//	get the file path
	//
	fsysFileList file_list;
	prtclGeomList::BuildFileList(file_list);
	file_list.GetFilePath(i_Data.m_BaseData.m_Filename.GetValue(), file_loc);
	file_loc.Push(i_Data.m_BaseData.m_Filename.GetValue());

	//api3dObject* Obj = api3dImport::LoadObject(file_loc);
	//
	prtParticleGenerator* pParticleGenerator;
	prtParticleGeneratorTemplate* pParticleGeneratorTemplate = new prtParticleGeneratorTemplate();

	fsResourceTracker::MarkBegin(file_loc);

	//	read the particle file and store as the template
	//
	prtGeneratorUtil::Read(file_loc, *pParticleGeneratorTemplate);
	fsLocator particle_texture_locator;
	// Old method looks in ..\Textures
	//particle_texture_locator = file_loc;
	//particle_texture_locator.Pop();
	//particle_texture_locator.Pop();
	//particle_texture_locator.Push(gfPaths::GetSubPath(gfPaths::e_Textures));
	//particle_texture_locator.Push( pParticleGeneratorTemplate->GetTextureLocator() );
	// New method searches Effects directory (no Textures dir)
	fsysFileUtil::GetFilePath(  prtclTextureList::GetSystemDirName(),
								itString(gfPaths::GetSubPath( gfPaths::e_Textures )), 
								pParticleGeneratorTemplate->GetTextureLocator().GetLastName(), 
								particle_texture_locator);
	pParticleGeneratorTemplate->SetTextureLocator(particle_texture_locator);
	pParticleGeneratorTemplate->MakeTexture();

	//	set the template to be updated with information based on the particle data
	//
	//	HACK - the data shouldn't get updated if it is an "old" scene (i.e. the data 
	//	is empty) so check max particles first.
	//
	if (i_Data.m_BaseData.m_MaxParticles.GetValue() != 0)
		prtclObjectMgr::UpdateTemplateFromData( i_Data.m_BaseData, pParticleGeneratorTemplate );

	//	make the particle generator
	pParticleGenerator = prtGeneratorUtil::MakeGenerator( *pParticleGeneratorTemplate, appSimTime::GetTime() );

	fsResourceTracker::MarkEnd(file_loc);

	pParticleGenerator->SetGeneratorLifetime(10000.0f);
	pParticleGenerator->Pause();
	pParticleGenerator->SetCreationTime( 0.0f );

	// create object for 3d icon, this object takes ownership
	// of the pointers we created here.
	prtclScriptObject *pParticle = new prtclScriptObject( pParticleGenerator, pParticleGeneratorTemplate );
	pParticle->SetScriptData( i_Data );
	pParticle->UpdateData();

	// Maintain the current icon visibility
	pParticle->ShowIcons(prtclObjectMgr::IconsVisible());

	if (i_Data.m_BaseData.m_Name.GetString().empty())
	{
		//	set a default name for the prop
		nameString objName;
		create_default_name( i_Data.m_BaseData.m_Filename.GetValue(), objName );
		pParticle->SetName( objName );
	}
	return pParticle;
}

//--------------------------------------------------------------------
// Init
//--------------------------------------------------------------------
void  prtclObjectMgr::Init()
{
	//prtclScriptDataMgr::AddDataChangedCallback(&l_DataChangedObj);
}

//--------------------------------------------------------------------
//  Clean up
//--------------------------------------------------------------------
void  prtclObjectMgr::CleanUp()
{
	//prtclScriptDataMgr::RemoveDataChangedCallback(&l_DataChangedObj);
}

//--------------------------------------------------------------------
// Clear out already created particles
//--------------------------------------------------------------------
void prtclObjectMgr::ResetGenerators()
{
	for (int i=0; i<sm_Objects.size(); i++)
	{
		sm_Objects[i]->ClearParticles();
	}
}

//--------------------------------------------------------------------
//	Pause/Unpause all the generators
//--------------------------------------------------------------------
//static 
bool prtclObjectMgr::Paused()
{
	return l_bPaused;
}

//static 
void prtclObjectMgr::Pause( bool i_bPause )
{
	l_bPaused = i_bPause;

	const int num_objects = sm_Objects.size();
	for (int i=0; i<num_objects; i++)
	{
		sm_Objects[i]->Pause(l_bPaused);
		if (l_bPaused)
			sm_Objects[i]->ClearParticles();	// get rid of already made particles
	}
}


//--------------------------------------------------------------------
//	Update a template with new particle data
//--------------------------------------------------------------------
void prtclObjectMgr::UpdateTemplateFromData( const prtclData &i_Data, prtParticleGeneratorTemplate* io_pTemplate )
{
	//	Base Generator
	io_pTemplate->SetParameter(prtParticleGenerator::e_ParticleRate, i_Data.m_Rate.GetValue());
	io_pTemplate->SetParameter(prtParticleGenerator::e_MaxParticles, i_Data.m_MaxParticles.GetValue());
	io_pTemplate->SetParameter(prtParticleGenerator::e_MinParticleLifetime, i_Data.m_LifetimeMin.GetValue());
	io_pTemplate->SetParameter(prtParticleGenerator::e_MaxParticleLifetime, i_Data.m_LifetimeMax.GetValue());
	io_pTemplate->SetParameter(prtParticleGenerator::e_PreSimTime, i_Data.m_PreSimTime.GetValue());


	if (   (io_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Static)
		|| (io_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Cone)
		|| (io_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Spiral))
	{
		//	Sprite Generator
		io_pTemplate->SetParameter(prtSpriteGroupParticleGenerator::e_InitialScale, i_Data.m_ScaleStart.GetValue());
		io_pTemplate->SetParameter(prtSpriteGroupParticleGenerator::e_ScaleCoeff, i_Data.m_ScaleCoefficient.GetValue());
		io_pTemplate->SetParameter(prtSpriteGroupParticleGenerator::e_MinStartAngle, i_Data.m_StartAngleMin.GetValue());
		io_pTemplate->SetParameter(prtSpriteGroupParticleGenerator::e_MaxStartAngle, i_Data.m_StartAngleMax.GetValue());
		io_pTemplate->SetParameter(prtSpriteGroupParticleGenerator::e_MinAngularVelocity, i_Data.m_AngularVelocityMin.GetValue());
		io_pTemplate->SetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularVelocity, i_Data.m_AngularVelocityMax.GetValue());
		io_pTemplate->SetParameter(prtSpriteGroupParticleGenerator::e_MinAngularAcceleration, i_Data.m_AngularAccelerationMin.GetValue());
		io_pTemplate->SetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularAcceleration, i_Data.m_AngularAccelerationMax.GetValue());

		//	texture alpha
		an3StateAnimation<float> new_gen_ap( i_Data.m_TextureAlphaStart.GetValue(),
													i_Data.m_TextureAlphaMiddle.GetValue(),
													i_Data.m_TextureAlphaEnd.GetValue(),
													i_Data.m_TextureAlphaMiddlePercentStart.GetValue(),
													i_Data.m_TextureAlphaMiddlePercentEnd.GetValue(),
													1.0f );
		io_pTemplate->SetAlphaAnimation(new_gen_ap);

		//	texture
		io_pTemplate->SetTextureLocator(i_Data.m_TextureFilename.GetValue());

		//	streaks
		io_pTemplate->SetRenderStreaks( i_Data.m_bRenderStreaks.GetValue());
		io_pTemplate->SetParameter(prtSpriteGroupParticleGenerator::e_StreakLength, i_Data.m_StreakLength.GetValue());
		io_pTemplate->SetParameter(prtSpriteGroupParticleGenerator::e_StreakTaper, i_Data.m_StreakTaper.GetValue());
		io_pTemplate->SetParameter(prtSpriteGroupParticleGenerator::e_StreakFade, i_Data.m_StreakFade.GetValue());
	}

	if (io_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Cone)
	{
		// prtConeParticleGenerator
		io_pTemplate->SetParameter(prtConeParticleGenerator::e_ConeAngle, i_Data.m_ConeAngle.GetValue());
		io_pTemplate->SetParameter(prtConeParticleGenerator::e_MinSpeed, i_Data.m_MinSpeed.GetValue());
		io_pTemplate->SetParameter(prtConeParticleGenerator::e_MaxSpeed, i_Data.m_MaxSpeed.GetValue());
		io_pTemplate->SetParameter(prtConeParticleGenerator::e_AccelerationX, i_Data.m_AccelerationX.GetValue());
		io_pTemplate->SetParameter(prtConeParticleGenerator::e_AccelerationY, i_Data.m_AccelerationY.GetValue());
		io_pTemplate->SetParameter(prtConeParticleGenerator::e_AccelerationZ, i_Data.m_AccelerationZ.GetValue());
	}
	else if (io_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Spiral)
	{
		// prtSpiralParticleGenerator
		io_pTemplate->SetParameter(prtSpiralParticleGenerator::e_MinEmitSpeed, i_Data.m_MinEmitSpeed.GetValue());
		io_pTemplate->SetParameter(prtSpiralParticleGenerator::e_MaxEmitSpeed, i_Data.m_MaxEmitSpeed.GetValue());
		io_pTemplate->SetParameter(prtSpiralParticleGenerator::e_EmitDirectionX, i_Data.m_EmitDirectionX.GetValue());
		io_pTemplate->SetParameter(prtSpiralParticleGenerator::e_EmitDirectionY, i_Data.m_EmitDirectionY.GetValue());
		io_pTemplate->SetParameter(prtSpiralParticleGenerator::e_EmitDirectionZ, i_Data.m_EmitDirectionZ.GetValue());
		io_pTemplate->SetParameter(prtSpiralParticleGenerator::e_MinRotStartAngle, i_Data.m_MinRotStartAngle.GetValue());
		io_pTemplate->SetParameter(prtSpiralParticleGenerator::e_MaxRotStartAngle, i_Data.m_MaxRotStartAngle.GetValue());
		io_pTemplate->SetParameter(prtSpiralParticleGenerator::e_MinRotAngularVel, i_Data.m_MinRotAngularVel.GetValue());
		io_pTemplate->SetParameter(prtSpiralParticleGenerator::e_MaxRotAngularVel, i_Data.m_MaxRotAngularVel.GetValue());
		io_pTemplate->SetParameter(prtSpiralParticleGenerator::e_RotRadius, i_Data.m_RotRadius.GetValue());
		io_pTemplate->SetParameter(prtSpiralParticleGenerator::e_RotRadiusScaleRate, i_Data.m_RotRadiusScaleRate.GetValue());
	}
}
