/********************************************************************************************\
**  ptclLevel.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "ptclLevel.hpp"

#include "ptclParticleTemplate.hpp"
#include "Graphics/prt/prtGeneratorUtil.hpp"
#include "Graphics/prt/prtImport.hpp"
#include "Graphics/prt/prtVertexAnimation.hpp"

#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dObjectGeom.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dParticleGenerator.hpp"

#include "Core/app/appSimTime.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/mat/matExceptionX.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/prt/prtConeParticleGenerator.hpp"



//------------------------------------------------------------------------
// Local variables and functions
//------------------------------------------------------------------------
namespace
{
	ptclParticleChangedCallback *		l_ChangedCallback = NULL;
	api3dParticleGenerator*				l_pGenerator = NULL;
	prtParticleGenerator*				l_pPrtGenerator = NULL;
	matTexture*							l_pCurrentTexture;
	fsLocator							l_CurrentTextureLocator;
	fsLocator							l_CurrentGeometryLocator;
	prtParticleGeneratorTemplate::Type	l_GeneratorType;
	an3StateAnimation<float>*			l_pAlphaAnim = NULL;
	std::vector<prtVertexAnimKeys>		l_ParticleKeys;
	std::vector<prtVertexFrame*>		l_ParticleFrames;
	prtVertexAnimation*					l_pAnimation = NULL;
	api3dObjectSimple*					l_pGround = NULL;
	g3dDirectionalLight*				l_pLight1 = NULL;
	g3dDirectionalLight*				l_pLight2 = NULL;


//----------------------------------------------------------------------------
// clear out old animation
//----------------------------------------------------------------------------
void delete_animation()
{		
	l_ParticleKeys.clear();
	envSTLHelpers::DeleteContainer(l_ParticleFrames);
	delete l_pAnimation;
	l_pAnimation = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void make_environment()
{
	//	the ground
	//
	fsLocator texture_locator = gfPaths::GetPath( gfPaths::e_ExePath );
	texture_locator.Push("Data");
	texture_locator.Push("ground.png");

	std::string szFilename;			// debug only
	fsFileUtil::LocatorToANSIFilename( texture_locator, szFilename );
	DBG_LOG1("make_environment: %s", szFilename.c_str() );

	l_pGround = api3dShape::CreateTexturedRectangle( texture_locator, 
							maFloatRGBA(0.5f,0.5f,0.5f,0.5f),
							50.0f, 
							50.0f, 
							16,
							16 );
	maRotation rotation;
	rotation.SetValue( maVector3d(1, 0, 0), -maConstants::c_fPI_Div_2 );

	l_pGround->SetPosition( maPoint3d( 0.0f, 0.0f, 0.0f ) );
	l_pGround->SetOrientation( rotation );
	l_pGround->SetScale( maVector3d(1,1,1) );

	api3dScene::AddObject( l_pGround );

	//	set lights
	//
	l_pLight1 = api3dLightMgr::CreateDirectionalLight();
	l_pLight2 = api3dLightMgr::CreateDirectionalLight();

	l_pLight1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	l_pLight2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	l_pLight1->SetIntensity(maFloatRGBA(0.4f, 0.7f, 0.2f, 1.0f));
	l_pLight2->SetIntensity(maFloatRGBA(0.3f, 0.6f, 0.8f, 1.0f));

	l_pLight1->Enable();
	l_pLight2->Enable();

	//api3dLightMgr::SetAmbient(maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void set_geometry(const fsLocator& i_Geometry)
{
	itString FilenameStr(i_Geometry.GetLastName());
	l_CurrentGeometryLocator = i_Geometry;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void load_texture( const fsLocator& i_TextureLoc )
{
	std::string szFilename;
	fsFileUtil::LocatorToANSIFilename( i_TextureLoc, szFilename );
	DBG_LOG1("load_texture: %s", szFilename.c_str() );

	prtParticleGeneratorTemplate * pPGTemplate = (ptclParticleTemplate::GetTemplate());
	pPGTemplate->SetTextureLocator( i_TextureLoc );
	pPGTemplate->MakeTexture();

	l_pCurrentTexture = pPGTemplate->GetTexture();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void set_texture(const fsLocator& i_TextureLoc)
{
	try
	{
		prtSpriteGroupParticleGenerator* sg_gen;
		sg_gen = dynamic_cast<prtSpriteGroupParticleGenerator*>(l_pPrtGenerator);
		if ( !sg_gen )
			return;

		itString FilenameStr(i_TextureLoc.GetLastName());

		//sg_gen->SetTexture(l_pCurrentTexture);

		l_CurrentTextureLocator = i_TextureLoc;
	}
	catch( const matUnknownImageFileTypeX& )
	{
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void refresh_generator()
{
	//l_ParameterPalette->UpdateGenerator(l_pPrtGenerator);

	prtSpriteGroupParticleGenerator* sg_gen;
	sg_gen = dynamic_cast<prtSpriteGroupParticleGenerator*>(l_pPrtGenerator);
	if ( sg_gen )
	{
		prtParticleGeneratorTemplate * pPGTemplate = (ptclParticleTemplate::GetTemplate());
		prtSpriteGroupParticleGenerator::ScaleMode	scale_mode	= pPGTemplate->GetScaleMode();
		prtSpriteGroupParticleGenerator::UVAMode	uva_mode	= pPGTemplate->GetUVAMode();
		prtSpriteGroupParticleGenerator::RenderMode render_mode = pPGTemplate->GetRenderMode();
		sg_gen->SetScaleMode(scale_mode);
		sg_gen->SetRenderMode(render_mode);
		sg_gen->SetUVAMode(uva_mode);
	}

	prtParticleGeneratorTemplate::EmitterType emitter_type = prtParticleGeneratorTemplate::EmitterType(1);
	prtParticleGeneratorTemplate * pGen_template = (ptclParticleTemplate::GetTemplate());
	pGen_template->SetEmitterType(emitter_type);

	maVector3d emitter_scale = maVector3d( 1.0f,1.0f,1.0f );

	pGen_template->SetEmitterScale(emitter_scale);
	l_pPrtGenerator->SetEmitter(pGen_template->MakeEmitter());

	an3StateAnimation<float>* anim = new an3StateAnimation<float>(  0.0f,
																	0.0f,
																	1.0f,
																	0.5f,
																	1.0f );
	delete l_pAlphaAnim;
	l_pAlphaAnim = anim;
	l_pPrtGenerator->SetAlphaProfile(*l_pAlphaAnim);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void create_particlegenerator()
{
	l_pPrtGenerator = prtGeneratorUtil::MakeGenerator( *(ptclParticleTemplate::GetTemplate()), appSimTime::GetTime() );

	l_pPrtGenerator->SetGeneratorLifetime(10000.0f);
	l_pPrtGenerator->SetPosition(maPoint3d(0.0f, 0.0f, 0.0f));
	l_pPrtGenerator->SetCreationTime( 0.0f );

	//	Set the level's alpha var
	//
	if ( l_pPrtGenerator != 0 )
	{
		const an3StateAnimation<float>* pKA = dynamic_cast<const an3StateAnimation<float>*>(l_pPrtGenerator->GetAlphaProfile());
		if ( pKA )
		{
			float begalpha, midalphatimestart, midalphatimeend, midalpha, endalpha;
			begalpha		= pKA->GetFirstValue();
			midalpha		= pKA->GetSecondValue();
			endalpha		= pKA->GetThirdValue();
			midalphatimestart	= pKA->GetMidTimeStart();
			midalphatimeend		= pKA->GetMidTimeEnd();

			ptclLevel::SetParticleGeneratorAlphaProfile(begalpha, 
														midalphatimestart, 
														midalphatimeend, 
														midalpha, 
														endalpha );
		}
		else
		{
			ptclLevel::SetParticleGeneratorAlphaProfile( (l_pPrtGenerator->GetAlphaProfile()) );
		}
	}

	//l_pPrtGenerator->SetOrientation(world_rotation);
	//api3dScene::RegisterGeneratorEventHandler(this, l_pPrtGenerator);

	//	set the data and add it to the object list
	//set_particle_data(Obj, i_Data);

	///l_Objects.push_back(Particle);

	//	add it to the scene
	//
	l_pGenerator = new api3dParticleGenerator( l_pPrtGenerator, NULL );
	l_pGenerator->SetParticleGeneratorTemplate( ptclParticleTemplate::GetTemplate() );

	api3dScene::AddParticleGenerator( l_pGenerator );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cleanup_and_create_particlegenerator()
{
	prtParticleGeneratorTemplate * pPGTemplate = (ptclParticleTemplate::GetTemplate());

	l_CurrentTextureLocator = pPGTemplate->GetTextureLocator();

	if( l_pGenerator )
	{
		api3dScene::RemoveParticleGenerator(l_pGenerator);
		delete l_pGenerator;
		l_pGenerator = NULL;
		l_pPrtGenerator = NULL;
		delete_animation();
	}

	//
	//	set the locator correctly
	//
	std::string dir;
	fsFileUtil::LocatorToANSIFilename( l_CurrentTextureLocator, dir );
	DBG_LOG1( "cleanup_and_create_particlegenerator (%s)", dir.c_str() );

	//	load the texture
	load_texture( l_CurrentTextureLocator );

	create_particlegenerator();

	pPGTemplate = (ptclParticleTemplate::GetTemplate());
	pPGTemplate->SetTextureLocator( l_CurrentTextureLocator );

	//if ( pPGTemplate->GetTexture() )
		set_texture( pPGTemplate->GetTextureLocator() );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void load_generator(const fsLocator& i_Locator)
{
	std::string dir;
	//fsFileUtil::LocatorToANSIFilename( i_Locator, dir );
	//DBG_LOG1( "load_gen file (%s)", dir.c_str() );

	prtParticleGeneratorTemplate * pGen_template = (ptclParticleTemplate::GetTemplate());

	prtParticleGeneratorTemplate::Type gen_type = pGen_template->GetType();

	if (	gen_type == prtParticleGeneratorTemplate::e_Static
		||	gen_type == prtParticleGeneratorTemplate::e_Cone
		||	gen_type == prtParticleGeneratorTemplate::e_Spiral )
	{
		//	try to set up the texture
		//
		fsLocator texture_locator = i_Locator;
		texture_locator.Pop();
		texture_locator.Push( pGen_template->GetTextureLocator().GetLastName() );

		//fsFileUtil::LocatorToANSIFilename( texture_locator, dir );
		//DBG_LOG1( "load_gen texturefile (%s)", dir.c_str() );

		bool found_texture = false;

		if( fsFileUtil::FileExists(texture_locator) )
		{
			found_texture = true;
		}
		else
		{
			//	not in same directory...try "Textures" directory above cur directory
			texture_locator.Pop();
			texture_locator.Pop();
			texture_locator.Push("Textures");

			if( fsFileUtil::DirectoryExists(texture_locator) )
			{
				texture_locator.Push( pGen_template->GetTextureLocator().GetLastName() );

				if ( fsFileUtil::FileExists(texture_locator) )
					found_texture = true;
			}
		}

		if( !found_texture )
		{
			//	ask the user to find it
			//fsLocator initial_dir = i_Locator;
			//initial_dir.Pop();

			//std::string texture_name;
			//fsFileUtil::LocatorToANSIFilename(pGen_template->GetTextureLocator(), texture_name);
			//std::string window_title = "Find texture ";
			//window_title += texture_name;

			//if( !gfPlatformFileDialog::GetOpenFilename(	initial_dir,
			//											"*",
			//											texture_locator,
			//											window_title.c_str()) )
			//	throw itString("Load canceled - couldn't find texture");

			if( !fsFileUtil::FileExists(texture_locator) )
				throw itString("Texture didn't exist");
		}

		//fsFileUtil::LocatorToANSIFilename( texture_locator, dir );
		//DBG_LOG1( "load_gen texture file (%s)", dir.c_str() );

		pGen_template->SetTextureLocator( texture_locator );

		//	now create the new particle and add it to the scene
		//
		cleanup_and_create_particlegenerator();

		//set_texture( texture_locator );

		//maRotation rotation;
		//rotation.SetValue(maVector3d(1, 0, 0), -maConstants::c_fPI_Div_2);
		//l_pPrtGenerator->SetPositionAndOrientation(maPoint3d(0, 0, 0), rotation);

		l_pPrtGenerator->SetGeneratorLifetime(1000000.0f);

		//l_FilenameDirty = false;
		//l_CurFilename = i_Locator;

		// FIX: refresh_data();
	}
	else if ( gen_type == prtParticleGeneratorTemplate::e_Cone3D )
	{
		//	try to load up the geometry
		//
//			fsLocator geometry_locator = i_Locator;
//			geometry_locator.Pop();
//			geometry_locator.Push(pGen_template->GetGeometryLocator());
		fsLocator geometry_locator = gfPaths::GetPath(gfPaths::e_ExePath);
		geometry_locator.Push( pGen_template->GetGeometryLocator().GetLastName() );

		bool found_geometry = false;

		if( fsFileUtil::FileExists(geometry_locator) )
		{
			found_geometry = true;
		}
		else
		{
			//	not in same directory...try "geometry" directory above cur directory
			geometry_locator.Pop();
			geometry_locator.Pop();
			geometry_locator.Push("Model");

			if( fsFileUtil::DirectoryExists(geometry_locator) )
			{
				geometry_locator.Push( pGen_template->GetGeometryLocator().GetLastName() );

				if( fsFileUtil::FileExists(geometry_locator) )
					found_geometry = true;
			}
		}

		if( !found_geometry )
		{
			//	ask the user to find it
			//fsLocator initial_dir = i_Locator;
			//initial_dir.Pop();

			//std::string geometry_name;
			//fsFileUtil::LocatorToANSIFilename(pGen_template->GetGeometryLocator(), geometry_name);
			//std::string window_title = "Find geometry ";
			//window_title += geometry_name;

			//if( !gfPlatformFileDialog::GetOpenFilename(	initial_dir,
			//											"*",
			//											geometry_locator,
			//											window_title.c_str()) )
			//	throw itString("Load canceled - couldn't find geometry");

			if( !fsFileUtil::FileExists(geometry_locator) )
				throw itString("Geometry didn't exist");
		}

		//	At this point we succesfully loaded the generator template and found
		//	the texture, so it's OK to delete the old generator stuff
		cleanup_and_create_particlegenerator();

		//	set-up the geometry location
		//
		set_geometry( geometry_locator );
		pGen_template->SetGeometryLocator( geometry_locator );

		//maRotation rotation;
		//rotation.SetValue(maVector3d(1, 0, 0), -maConstants::c_fPI_Div_2);
		//l_pPrtGenerator->SetPositionAndOrientation(maPoint3d(0, 0, 0), rotation);

		l_pPrtGenerator->SetGeneratorLifetime(1000000.0f);

		//l_FilenameDirty = false;
		//l_CurFilename = i_Locator;
		
		// FIX: refresh_data();

		//l_MainPalette->DisplayFeedbackText( 2, 3 );
	}
	else
	{
		DBG_ASSERT0(false,"Invalid Generator Type" );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void read_and_load_generator(const fsLocator& i_Locator)
{
	prtParticleGeneratorTemplate * pGen_template = (ptclParticleTemplate::GetTemplate());

	//std::string szFilename;
	//fsFileUtil::LocatorToANSIFilename( i_Locator, szFilename );
	//DBG_LOG1("generator file: %s", szFilename.c_str() );

	try
	{
		prtGeneratorUtil::Read(i_Locator, *pGen_template);

		load_generator( i_Locator );
	}
	catch( const fsFileDoesntExistX& )
	{
		//l_MainPalette->DisplayFeedbackText( 3, 3 );
//		l_StatusLine->SetTextColor(l_ErrorText, guiWindow::e_Idle);
//		l_StatusLine->SetText(itString("That file doesn't exist."));
	}
	catch( const itString& ) //i_String )
	{
		//l_MainPalette->DisplayFeedbackText( i_String, true, 3 );
//		l_StatusLine->SetTextColor(l_ErrorText, guiWindow::e_Idle);
//		l_StatusLine->SetText(i_String);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void save_generator(const fsLocator& i_Locator)
{
	try
	{
		//	make a template from the generator, and save it
		if( fsFileUtil::FileExists(i_Locator) )
			fsFileUtil::DeleteFile(i_Locator);

		fsFileUtil::CreateFile(i_Locator);

		prtParticleGeneratorTemplate * pGen_template = (ptclParticleTemplate::GetTemplate());
		prtGeneratorUtil::MakeTemplateFromGenerator( *pGen_template, *l_pPrtGenerator );

		if ( dynamic_cast<prtSpriteGroupParticleGenerator*>(l_pPrtGenerator) )
		{
			//	fill in texture info
			pGen_template->SetTextureLocator(l_CurrentTextureLocator);
		}
		else
		{
			//	fill in geometry info
			pGen_template->SetGeometryLocator(l_CurrentGeometryLocator);
		}
		prtGeneratorUtil::Write( i_Locator, *pGen_template );

		//l_FilenameDirty = false;
		//l_CurFilename = i_Locator;
		//l_MainPalette->DisplayFeedbackText( 4, 3 );
//		l_StatusLine->SetTextColor(l_IdleText, guiWindow::e_Idle);
//		l_StatusLine->SetText(itString("Succesfully saved file."));
	}
	catch( const fsReadOnlyX& )
	{
		//l_MainPalette->DisplayFeedbackText( 5, 3 );
//		l_StatusLine->SetTextColor(l_ErrorText, guiWindow::e_Idle);
//		l_StatusLine->SetText(itString("That file is read-only."));
	}
}

//
void deinitialize_objects()
{
	if ( l_pGround != 0 )
	{
		api3dScene::RemoveObject(l_pGround);
		delete l_pGround;
		l_pGround = 0;
	}

	if( l_pPrtGenerator )
	{
		api3dScene::RemoveParticleGenerator(l_pGenerator);
		delete l_pGenerator;
		l_pGenerator = NULL;
		l_pPrtGenerator = NULL;
		delete_animation();
	}

	if ( l_pLight1 )
		api3dLightMgr::DestroyLight( l_pLight1 );
	if ( l_pLight2 )
		api3dLightMgr::DestroyLight( l_pLight2 );
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void make_default_generator()
{
	//
	//	the new way of building a default particle generator.   This way
	//	a user can create their own default generator.
	//
	if( l_pPrtGenerator )
	{
		api3dScene::RemoveParticleGenerator(l_pGenerator);
		delete l_pGenerator;
		l_pGenerator = NULL;
		l_pPrtGenerator = NULL;
		delete_animation();
	}

	fsLocator filename = gfPaths::GetPath(gfPaths::e_ExePath);
	filename.Push( "Data" );
	filename.Push( "default.tpr" );
	ptclLevel::Load( filename );
	return;
}

}	// namespace


//------------------------------------------------------------------------
//	Initialize()
//------------------------------------------------------------------------
void
ptclLevel::Initialize()
{
	ptclParticleTemplate::Init();

	make_environment();

	make_default_generator();
}


//------------------------------------------------------------------------
//	DeInitialize()
//------------------------------------------------------------------------
void
ptclLevel::DeInitialize()
{
	ptclParticleTemplate::CleanUp();

	deinitialize_objects();

	Clear();
}


//----------------------------------------------------------------------------
//	SetParticleChangedCallback
//----------------------------------------------------------------------------
void	ptclLevel::SetParticleChangedCallback( ptclParticleChangedCallback *i_Callback )
{
	l_ChangedCallback = i_Callback;
}


//----------------------------------------------------------------------------
//	Think - Handle material animation timing
//----------------------------------------------------------------------------
void ptclLevel::Think()
{
	prtParticleGeneratorTemplate * pGen_template = (ptclParticleTemplate::GetTemplate());

	//refresh_generator();
}

//------------------------------------------------------------------------
//	Clear
//------------------------------------------------------------------------
void
ptclLevel::Clear()
{
	// Call user callback to update interface
	//if (l_ChangedCallback)
	//	l_ChangedCallback->ModelChange();
}


//------------------------------------------------------------------------
//	Save saves a level to a given locator
//------------------------------------------------------------------------
void
ptclLevel::Save(const fsLocator& i_Locator)
{
	prtGeneratorUtil::Write( i_Locator, *(ptclParticleTemplate::GetTemplate()) );
}



//------------------------------------------------------------------------
//	Load()	
//------------------------------------------------------------------------
void
ptclLevel::Load(const fsLocator& i_Locator)
{
	read_and_load_generator( i_Locator );
}

//------------------------------------------------------------------------
//	LoadAnimation()
//------------------------------------------------------------------------
void ptclLevel::LoadAnimation(const fsLocator& i_Locator)
{
	if (l_pPrtGenerator)
	{
		float anim_fps = g3dConstants::c_fDefaultFrameRate;

		// clear out old animation
		l_pPrtGenerator->ClearAnimation();
		delete_animation();

		// Load in animation data
		prtImport::LoadVertexAnimation( i_Locator,
							l_ParticleKeys,
							l_ParticleFrames,
							anim_fps);

		if (!l_ParticleKeys.empty())
		{
			// Create animation and give it to particle generator
			l_pAnimation = new prtVertexAnimation(l_ParticleKeys[0], anim_fps);
			l_pAnimation->SetLooping( true );
			// start animation time slightly before current time
			l_pPrtGenerator->SetAnimation(*l_pAnimation, appSimTime::GetTime()); 
		}
	}

}

//------------------------------------------------------------------------
//	FocusCamera() - put camera target at particle center
//------------------------------------------------------------------------
void ptclLevel::FocusCamera()
{
	if (l_pPrtGenerator)
	{
		maAxisBox bbox = l_pPrtGenerator->GetWorldBox();
		cam3dMgr::FocusCamera(bbox);
	}
}

//------------------------------------------------------------------------
//	Set the alpha profile values
//------------------------------------------------------------------------
void ptclLevel::SetParticleGeneratorTemplateAlphaProfile(float i_fBeginAlpha, 
														 float i_fMiddleAlphaTimeStart, 
														 float i_fMiddleAlphaTimeEnd, 
														 float i_fMiddleAlpha, 
														 float i_fEndAlpha )
{
	an3StateAnimation<float>* anim = new an3StateAnimation<float>( i_fBeginAlpha,
																	i_fMiddleAlpha,
																	i_fEndAlpha,
																	i_fMiddleAlphaTimeStart,
																	i_fMiddleAlphaTimeEnd,
																	1.0f );

	// HACK:! [rjk] if the stateanimation is no the same type BAD things will happen.
	//	right now we only use one type.
	//
	//	this should be: delete the pointer and assign the pointer anim to l_pAlphaAnim
	//
	if ( l_pAlphaAnim == 0 )
	{
		l_pAlphaAnim = anim;
	}
	else
	{
		//delete l_pAlphaAnim;
		*l_pAlphaAnim = *anim;
	}

	prtParticleGeneratorTemplate * pPGTemplate = (ptclParticleTemplate::GetTemplate());
	pPGTemplate->SetAlphaAnimation( *l_pAlphaAnim );
}


//------------------------------------------------------------------------
//	return the particle generator
//------------------------------------------------------------------------
prtParticleGenerator* ptclLevel::GetParticleGenerator()
{
	return l_pPrtGenerator;
}

//------------------------------------------------------------------------
//	replace the current generator based on the particle template
//------------------------------------------------------------------------
void  ptclLevel::ReplaceGenerator()
{
	cleanup_and_create_particlegenerator();
}

//------------------------------------------------------------------------
//	Get the alpha profile values
//------------------------------------------------------------------------
void ptclLevel::GetParticleGeneratorAlphaProfile(float& o_fBeginAlpha, 
												 float& o_fMiddleAlphaTimeStart, 
												 float& o_fMiddleAlphaTimeEnd, 
												 float& o_fMiddleAlpha, 
												 float& o_fEndAlpha )
{
	const an3StateAnimation<float>* pAnim = dynamic_cast<const an3StateAnimation<float>*>(l_pPrtGenerator->GetAlphaProfile());
	if ( pAnim )
	{
		o_fBeginAlpha		= pAnim->GetFirstValue();
		o_fMiddleAlpha		= pAnim->GetSecondValue();
		o_fEndAlpha			= pAnim->GetThirdValue();
		o_fMiddleAlphaTimeStart	= pAnim->GetMidTimeStart();
		o_fMiddleAlphaTimeEnd	= pAnim->GetMidTimeEnd();
	}

	//
	//DBG_LOG0("ParticleDialog constructor");
	//DBG_LOG1( "  1) %6.3f", o_fBeginAlpha );
	//DBG_LOG1( "  2) %6.3f", o_fMiddleAlpha );
	//DBG_LOG1( "  3) %6.3f", o_fEndAlpha );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
const anTypedAnimation<float>* ptclLevel::GetParticleGeneratorAlphaProfile()
{
	return l_pAlphaAnim;
}

//------------------------------------------------------------------------
//	Set the alpha profile values
//------------------------------------------------------------------------
void ptclLevel::SetParticleGeneratorAlphaProfile(float i_fBeginAlpha, 
												 float i_fMiddleAlphaTimeStart, 
												 float i_fMiddleAlphaTimeEnd, 
												 float i_fMiddleAlpha, 
												 float i_fEndAlpha )
{
	an3StateAnimation<float>* anim = new an3StateAnimation<float>( i_fBeginAlpha,
																	i_fMiddleAlpha,
																	i_fEndAlpha,
																	i_fMiddleAlphaTimeStart,
																	i_fMiddleAlphaTimeEnd,
																	1.0f );

	// HACK:! [rjk] if the stateanimation is no the same type BAD things will happen.
	//	right now we only use one type.
	//
	//	this should be: delete the pointer and assign the pointer anim to l_pAlphaAnim
	//
	if ( l_pAlphaAnim == 0 )
	{
		l_pAlphaAnim = anim;
	}
	else
	{
		//delete l_pAlphaAnim;
		*l_pAlphaAnim = *anim;
	}

	l_pPrtGenerator->SetAlphaProfile( *l_pAlphaAnim );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ptclLevel::SetParticleGeneratorAlphaProfile(const anTypedAnimation<float>* i_pAlpha)
{
	DBG_ASSERT0( i_pAlpha != 0, "no alpha profile" );

	delete l_pAlphaAnim;
	l_pAlphaAnim = dynamic_cast<an3StateAnimation<float>*>(i_pAlpha->Clone());

	l_pPrtGenerator->SetAlphaProfile( *l_pAlphaAnim );
}

//------------------------------------------------------------------------
//	Set the texture scale mode
//------------------------------------------------------------------------
void ptclLevel::SetParticleGeneratorTextureScaleMode( int i_ScaleMode )
{
	prtSpriteGroupParticleGenerator* pSg_gen;
	pSg_gen = dynamic_cast<prtSpriteGroupParticleGenerator*>(l_pPrtGenerator);
	if ( pSg_gen )
	{
		pSg_gen->SetScaleMode( (prtSpriteGroupParticleGenerator::ScaleMode)i_ScaleMode );
	}
}


//------------------------------------------------------------------------
//	Set the texture alpha blending render mode
//------------------------------------------------------------------------
void ptclLevel::SetParticleGeneratorTextureAlphaBlendingMode( int i_AlphaBlendingRenderMode )
{
	prtSpriteGroupParticleGenerator* pSg_gen;
	pSg_gen = dynamic_cast<prtSpriteGroupParticleGenerator*>(l_pPrtGenerator);
	if ( pSg_gen )
	{
		pSg_gen->SetRenderMode( (prtSpriteGroupParticleGenerator::RenderMode)i_AlphaBlendingRenderMode );
	}
}

//------------------------------------------------------------------------
//	Set the texture UVA mode
//------------------------------------------------------------------------
void ptclLevel::SetParticleGeneratorTextureUVAMode( int i_UVAMode )
{
	prtSpriteGroupParticleGenerator* pSg_gen;
	pSg_gen = dynamic_cast<prtSpriteGroupParticleGenerator*>(l_pPrtGenerator);
	if ( pSg_gen )
	{
		pSg_gen->SetUVAMode( (prtSpriteGroupParticleGenerator::UVAMode)i_UVAMode );
	}
}


//------------------------------------------------------------------------
//	Set the particle generator texture.
//
//	input: the string should be the entire path plus filename.
//
//	Note: this will unload the current texture and load the new one
//------------------------------------------------------------------------
void ptclLevel::SetParticleGeneratorTexture( std::string& i_TextureFilename )
{
	DBG_ASSERT0( i_TextureFilename.size() > 0, "Cannot set texture to an emptry filename" );

	fsLocator textureFilename;
	fsFileUtil::ANSIFilenameToLocator( i_TextureFilename, textureFilename );

	load_texture( textureFilename );
	set_texture( textureFilename );

	prtSpriteGroupParticleGenerator* pSg_gen;
	pSg_gen = dynamic_cast<prtSpriteGroupParticleGenerator*>(l_pPrtGenerator);
	if ( pSg_gen )
	{
		pSg_gen->RemoveTextures();
		pSg_gen->SetTexture( l_pCurrentTexture );
	}
}


//------------------------------------------------------------------------
//	show the ground or not.
//------------------------------------------------------------------------
void ptclLevel::ShowGround( bool i_bRenderGround )
{
	l_pGround->SetRenderable( i_bRenderGround );
}
bool ptclLevel::IsShowGround()
{
	return l_pGround->GetRenderable();
}

