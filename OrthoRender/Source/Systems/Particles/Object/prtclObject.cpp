/*****************************************************************************
**  prtclObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Object/prtclObject.hpp"

#include "Systems/Particles/GUI/prtclDialogDataUtil.hpp"
#include "Systems/Particles/Data/prtclDocumentChunk.hpp"
#include "Systems/Particles/Undo/prtclOperations.hpp"
#include "Systems/Particles/GUI/prtclTextureList.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmReferencePosChannel.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

#include "MainApp/mnmApp.hpp"

// library
#include "Core/fs/fsFileUtil.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Graphics/mat/matUVATexture.hpp"
#include "Graphics/prt/prtConeParticleGenerator.hpp"
#include "Graphics/prt/prtSpiralParticleGenerator.hpp"
#include "Graphics/an/an3StateAnimation.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dParticleGenerator.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"



//============================================================================
//============================================================================
namespace
{
	const float l_SphereRadius = 0.8f;
	const float l_PickRadius = 2.0f;	// pick larger than icon
	const maAxisBox l_SphereBox(-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius);
	const maVector3d l_Zero(0,0,0);
	const maVector3d l_One(1,1,1);
	const maRotation l_NoRot;
}


//----------------------------------------------------------------------------
// Ownership for the generator and the template pass to this object.
//----------------------------------------------------------------------------
prtclObject::prtclObject(prtParticleGenerator* i_pGenerator,
						 prtParticleGeneratorTemplate* i_pTemplate )
:	m_WorldBox(l_SphereBox),
	m_pGenerator( i_pGenerator ),
	m_pGeneratorTemplate( i_pTemplate ),
	m_bShowIcons( true ), 
	m_bLayerVisible( true )
{
	// ownership of the generator and template now passes to the api3d object
	m_pObject = new api3dParticleGenerator(i_pGenerator, i_pTemplate);
	api3dScene::AddParticleGenerator(m_pObject);

	ltstLightSetMgr::AddObject(this, m_pObject);
	evmtEnvironmentMgr::AddObject(this, m_pObject);

	//	adjust particle system to point up Y
	//maRotation rotation(0.0f,1.0f,0.0f);
	//m_pObject->SetOrientation(rotation);

	//	create the shape
	//
	//m_pDebugObject = NULL;
	//if ( m_pDebugObject )
	{
		m_pDebugObject = api3dShape::CreateCube( maFloatRGBA(0.66f,0.29f,0.86f,1.0f), l_SphereRadius );
		m_pDebugObject->SetUniformScale( api3dScale::GetGlobalScale() );
		m_pDebugObject->SetPosition(m_Data.m_Position.GetValue());

		api3dScene::AddObject(m_pDebugObject, mnmApp::GetIconsLayerIndex());
	}

	api3dScale::RegisterScaleInterest(this);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Position), "Transform", "Position of the object");
	AddProperty( pPUII );
	prtyVector3dEditUpDownUIInfo *pPVEUDUII  = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Orientation), "Transform", "Orientation of the object");
	pPVEUDUII->SetIncrement(0.5f, 0.5f, 0.5f);
	pPVEUDUII->SetDecimalPlaces(1);
	AddProperty( pPVEUDUII );
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	AddProperty( pPUII );
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Filename), "Asset", "File Name of the object");
	pPUII->SetReadOnly(true);
	AddProperty( pPUII );

	// Visibility is the combination of multiple flags in the user interface.
	//	(Check box on list dialog, layer visibility, channel scripted visibility, etc.)
	// So, I don't think exposing this particluar visible flag is a good idea.
	//AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bEditorVisible), "Editor", "Is the object visible in the editor") );

	//	emitter scale not for point, single value for sphere, and vector for box
	//	TODO how to resolve this?
	//
		//if (   (i_pTemplate->GetEmitterType() == prtParticleGeneratorTemplate::e_Point)
		//enum EmitterType
		//{
		//	e_Point = 0,
		//	e_Block,
		//	e_Circle
		//};
//	pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_EmitterScale), "Generation", "Emitter Scale");
//	AddProperty(pPUII);

	//	base generator
	pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_Rate), "Generation", "Rate of Particle Emission");
	AddProperty(pPUII);
	pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MaxParticles), "Generation", "Maximum Number Of Particles At Any Given Time");
	AddProperty(pPUII);
	pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_LifetimeMin), "Generation", "Particle Lifetime Minimum");
	AddProperty(pPUII);
	pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_LifetimeMax), "Generation", "Particle Lifetime Maximum");
	AddProperty(pPUII);
	pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_PreSimTime), "Generation", "Particle Pre-Sim time");
	AddProperty(pPUII);

	if (   (i_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Static)
		|| (i_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Cone)
		|| (i_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Spiral))
	{
		//	sprite particles
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_ScaleStart), "Generation", "Scale Start");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_ScaleCoefficient), "Generation", "Scale Coefficient");
		AddProperty(pPUII);
		pPUII = new prtyComboBoxUIInfo(&(m_Data.m_ScaleMode), "Generation", "Scale mode");
		AddProperty( pPUII );
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_StartAngleMin), "Generation", "Start Angle Min");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_StartAngleMax), "Generation", "Start Angle Max");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AngularVelocityMin), "Generation", "Angular Velocity Min");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AngularVelocityMax), "Generation", "Angular Velocity Max");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AngularAccelerationMin), "Generation", "Angular Acceleration Min");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AngularAccelerationMax), "Generation", "Angular Acceleration Max");
		AddProperty(pPUII);
//		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_LifetimeMax), "Generation", "Particle Lifetime Maximum");
//		AddProperty(pPUII);

		prtyRangedFloatUIInfo* pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_TextureAlphaStart), "Texture", "Alpha Start");
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		pRFUII->SetNumTicks(200);
		AddProperty(pRFUII);
		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_TextureAlphaMiddle), "Texture", "Alpha Middle");
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		pRFUII->SetNumTicks(200);
		AddProperty(pRFUII);
		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_TextureAlphaEnd), "Texture", "Alpha End");
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		pRFUII->SetNumTicks(200);
		AddProperty(pRFUII);
		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_TextureAlphaMiddlePercentStart), "Texture", "Alpha Middle Start");
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		pRFUII->SetNumTicks(200);
		AddProperty(pRFUII);
		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_TextureAlphaMiddlePercentEnd), "Texture", "Alpha Middle End");
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		pRFUII->SetNumTicks(200);
		AddProperty(pRFUII);
		prtyFileChooserUIInfo* pFCI;
		pFCI = new prtyFileChooserUIInfo(&(m_Data.m_TextureFilename), "Texture", "Texture Filename");
		fsLocator dir;
		dir.Push( gfPaths::GetPath( mnmPaths::e_DataStock ) );
		dir.Push("Effects");
		dir.Push("General");
		dir.Push("Textures");
		pFCI->SetInitialDirectory(dir);
		AddProperty(pFCI);
		prtyNumericUpDownUIInfo* pNUDUII;
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_TextureRows), "Texture", "Number of texture rows");
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(16);
		AddProperty(pNUDUII);
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_TextureCols), "Texture", "Number of texture columns");
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(16);
		AddProperty(pNUDUII);
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bTextureLooping), "Texture", "Playback loops at the end");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bTextureReverse), "Texture", "Playback is reversed");
		AddProperty( pPUII );
		pPUII = new prtyComboBoxUIInfo(&(m_Data.m_TextureUVAMode), "Texture", "Texture animation mode");
		AddProperty( pPUII );
		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_TextureRate), "Texture", "Texture animation frame rate (per second)");
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(120.0f);
		pRFUII->SetNumTicks(200);
		AddProperty(pRFUII);

		// Streak properties
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bRenderStreaks), "Streak", "Render particles as streaks.");
		AddProperty( pPUII );
		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_StreakLength), "Streak", "Streak Length");
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		pRFUII->SetNumTicks(200);
		AddProperty(pRFUII);
		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_StreakTaper), "Streak", "Streak Taper");
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		pRFUII->SetNumTicks(200);
		AddProperty(pRFUII);
		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_StreakFade), "Streak", "Streak Fade");
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		pRFUII->SetNumTicks(200);
		AddProperty(pRFUII);

		// Rendering Properties
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bShowInCubeReflections), "Rendering", "Render particles in cube reflection maps.");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bShowInPlanarReflections), "Rendering", "Render particles in planar reflection maps.");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bCastShadows), "Rendering", "Should particles cast shadows.");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseDitheredShadows), "Rendering", "Cast translucent dithered shadows, when cast shadows is on.");
		AddProperty( pPUII );
		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_ShadowDitherBias), "Rendering", "Increase alpha translucency of the dithering");
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		pRFUII->SetNumTicks(200);
		AddProperty(pRFUII);
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bAdditive), "Rendering", "Use Additive blending instead of multiplicative.");
		AddProperty( pPUII );
	}

	if (i_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Cone)
	{
		// prtConeParticleGenerator
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_ConeAngle), "Generation", "Cone Angle");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MinSpeed), "Generation", "Min Speed");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MaxSpeed), "Generation", "Max Speed");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AccelerationX), "Generation", "Acceleration X");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AccelerationY), "Generation", "Acceleration Y");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AccelerationZ), "Generation", "Acceleration Z");
		AddProperty(pPUII);
	} 
	else if (i_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Spiral)
	{
		// prtSpiralParticleGenerator
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MinEmitSpeed), "Generation", "Min Emit Speed");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MaxEmitSpeed), "Generation", "Max Emit Speed");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_EmitDirectionX), "Generation", "Emit Direction X");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_EmitDirectionY), "Generation", "Emit Direction Y");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_EmitDirectionZ), "Generation", "Emit Direction Z");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MinRotStartAngle), "Generation", "Min Rot Start Angle");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MaxRotStartAngle), "Generation", "Max Rot Start Angle");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MinRotAngularVel), "Generation", "Min Rot Angular Vel");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MaxRotAngularVel), "Generation", "Max Rot Angular Vel");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_RotRadius), "Generation", "Rot Radius");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_RotRadiusScaleRate), "Generation", "Rot Radius Scale Rate");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AccelerationX), "Generation", "Acceleration X");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AccelerationY), "Generation", "Acceleration Y");
		AddProperty(pPUII);
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AccelerationZ), "Generation", "Acceleration Z");
		AddProperty(pPUII);
	}

	//
	// Register callbacks to update particle generator and icons
	// when properties change
	//

	//	object properties
	m_Data.m_Position.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::PositionChanged));
	m_Data.m_Orientation.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::OrientationChanged));
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::NameChanged));

	//	Emitter properties
	m_Data.m_EmitterScale.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::EmitterDataChanged));

	//	base generator properties
	m_Data.m_Rate.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::BaseGeneratorDataChanged));
	m_Data.m_LifetimeMin.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::BaseGeneratorDataChanged));
	m_Data.m_LifetimeMax.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::BaseGeneratorDataChanged));
	m_Data.m_MaxParticles.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::BaseGeneratorDataChanged));
	m_Data.m_PreSimTime.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::BaseGeneratorDataChanged));

	if (   (i_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Static)
		|| (i_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Cone)
		|| (i_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Spiral))
	{
		//	sprite generator properties
		m_Data.m_ScaleStart.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpriteGeneratorDataChanged));
		m_Data.m_ScaleCoefficient.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpriteGeneratorDataChanged));
		m_Data.m_ScaleMode.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpriteGeneratorDataChanged));
		m_Data.m_StartAngleMin.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpriteGeneratorDataChanged));
		m_Data.m_StartAngleMax.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpriteGeneratorDataChanged));
		m_Data.m_AngularVelocityMin.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpriteGeneratorDataChanged));
		m_Data.m_AngularVelocityMax.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpriteGeneratorDataChanged));
		m_Data.m_AngularAccelerationMin.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpriteGeneratorDataChanged));
		m_Data.m_AngularAccelerationMax.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpriteGeneratorDataChanged));

		m_Data.m_TextureAlphaStart.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::TextureAlphaDataChanged));
		m_Data.m_TextureAlphaMiddle.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::TextureAlphaDataChanged));
		m_Data.m_TextureAlphaEnd.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::TextureAlphaDataChanged));
		m_Data.m_TextureAlphaMiddlePercentStart.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::TextureAlphaDataChanged));
		m_Data.m_TextureAlphaMiddlePercentEnd.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::TextureAlphaDataChanged));
		m_Data.m_TextureFilename.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::TextureFilenameDataChanged));
		m_Data.m_TextureRows.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::TextureDataChanged));
		m_Data.m_TextureCols.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::TextureDataChanged));
		m_Data.m_bTextureLooping.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::TextureDataChanged));
		m_Data.m_bTextureReverse.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::TextureDataChanged));
		m_Data.m_TextureRate.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::TextureDataChanged));
		m_Data.m_TextureUVAMode.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::TextureDataChanged));

		// streaking
		m_Data.m_bRenderStreaks.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::StreakDataChanged));
		m_Data.m_StreakLength.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::StreakDataChanged));
		m_Data.m_StreakTaper.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::StreakDataChanged));
		m_Data.m_StreakFade.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::StreakDataChanged));

		// rendering
		m_Data.m_bShowInCubeReflections.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::RenderDataChanged));
		m_Data.m_bShowInPlanarReflections.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::RenderDataChanged));
		m_Data.m_bCastShadows.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::RenderDataChanged));
		m_Data.m_bUseDitheredShadows.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::RenderDataChanged));
		m_Data.m_ShadowDitherBias.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::RenderDataChanged));
		m_Data.m_bAdditive.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::RenderDataChanged));
	}

	if (i_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Cone)
	{
		// prtConeParticleGenerator
		m_Data.m_ConeAngle.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::ConeGeneratorDataChanged));
		m_Data.m_MinSpeed.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::ConeGeneratorDataChanged));
		m_Data.m_MaxSpeed.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::ConeGeneratorDataChanged));
		m_Data.m_AccelerationX.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::ConeGeneratorDataChanged));
		m_Data.m_AccelerationY.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::ConeGeneratorDataChanged));
		m_Data.m_AccelerationZ.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::ConeGeneratorDataChanged));
	}
	else if (i_pTemplate->GetType() == prtParticleGeneratorTemplate::e_Spiral)
	{
		// prtSpiralParticleGenerator
		m_Data.m_MinEmitSpeed.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpiralGeneratorDataChanged));
		m_Data.m_MaxEmitSpeed.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpiralGeneratorDataChanged));
		m_Data.m_EmitDirectionX.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpiralGeneratorDataChanged));
		m_Data.m_EmitDirectionY.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpiralGeneratorDataChanged));
		m_Data.m_EmitDirectionZ.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpiralGeneratorDataChanged));
		m_Data.m_MinRotStartAngle.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpiralGeneratorDataChanged));
		m_Data.m_MaxRotStartAngle.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpiralGeneratorDataChanged));
		m_Data.m_MinRotAngularVel.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpiralGeneratorDataChanged));
		m_Data.m_MaxRotAngularVel.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpiralGeneratorDataChanged));
		m_Data.m_RotRadius.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpiralGeneratorDataChanged));
		m_Data.m_RotRadiusScaleRate.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::SpiralGeneratorDataChanged));
		m_Data.m_AccelerationX.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::ConeGeneratorDataChanged));
		m_Data.m_AccelerationY.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::ConeGeneratorDataChanged));
		m_Data.m_AccelerationZ.AddCallback(new prtyCallbackWrapper<prtclObject>(this, &prtclObject::ConeGeneratorDataChanged));
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtclObject::~prtclObject()
{
	ltstLightSetMgr::RemoveObject(this, m_pObject);
	evmtEnvironmentMgr::RemoveObject(this, m_pObject);
	api3dScene::RemoveParticleGenerator(m_pObject);
	delete m_pObject;

	api3dScale::UnRegisterScaleInterest(this);

	if ( m_pDebugObject )
	{
		api3dScene::RemoveObject(m_pDebugObject, mnmApp::GetIconsLayerIndex());
		delete m_pDebugObject;
	}
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string prtclObject::GetPick3dName() const
{
	return GetName().GetString();
}

//--------------------------------------------------------------------
//	GlobalScaleChanged - notification that global scale has changed.
//--------------------------------------------------------------------
//virtual 
void prtclObject::GlobalScaleChanged( float i_Scale )
{
	m_pDebugObject->SetUniformScale( i_Scale );
	//m_WorldBox = m_pDebugObject->GetWorldBox();
}

//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
const prtclData& prtclObject::GetData() const
{
	return m_Data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void prtclObject::SetData(const prtclData &i_Data)
{
	m_Data = i_Data;
}

//--------------------------------------------------------------------
//	Update the data with the generator and template
//--------------------------------------------------------------------
void prtclObject::UpdateData()
{
	//	Set the data
	//

	//	Emitter
//	m_Data.m_EmitterScale.SetValue(1.0f, prtyProperty::eNoUndo );

	//	Base Generator
	m_Data.m_Rate.SetValue(m_pGeneratorTemplate->GetParameter(prtParticleGenerator::e_ParticleRate),prtyProperty::eNoUndo);
	m_Data.m_MaxParticles.SetValue(m_pGeneratorTemplate->GetParameter(prtParticleGenerator::e_MaxParticles),prtyProperty::eNoUndo);
	m_Data.m_LifetimeMin.SetValue(m_pGeneratorTemplate->GetParameter(prtParticleGenerator::e_MinParticleLifetime),prtyProperty::eNoUndo);
	m_Data.m_LifetimeMax.SetValue(m_pGeneratorTemplate->GetParameter(prtParticleGenerator::e_MaxParticleLifetime),prtyProperty::eNoUndo);
	m_Data.m_PreSimTime.SetValue(m_pGeneratorTemplate->GetParameter(prtParticleGenerator::e_PreSimTime),prtyProperty::eNoUndo);
	if (   (m_pGeneratorTemplate->GetType() == prtParticleGeneratorTemplate::e_Static)
		|| (m_pGeneratorTemplate->GetType() == prtParticleGeneratorTemplate::e_Cone)
		|| (m_pGeneratorTemplate->GetType() == prtParticleGeneratorTemplate::e_Spiral))
	{
		//	Sprite Generator
		m_Data.m_ScaleStart.SetValue(m_pGeneratorTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_InitialScale), prtyProperty::eNoUndo);
		m_Data.m_ScaleCoefficient.SetValue(m_pGeneratorTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_ScaleCoeff), prtyProperty::eNoUndo);
		m_Data.m_ScaleMode.SetValue(m_pGeneratorTemplate->GetScaleMode(), prtyProperty::eNoUndo);
		m_Data.m_StartAngleMin.SetValue(m_pGeneratorTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_MinStartAngle), prtyProperty::eNoUndo);
		m_Data.m_StartAngleMax.SetValue(m_pGeneratorTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_MaxStartAngle), prtyProperty::eNoUndo);
		m_Data.m_AngularVelocityMin.SetValue(m_pGeneratorTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_MinAngularVelocity), prtyProperty::eNoUndo);
		m_Data.m_AngularVelocityMax.SetValue(m_pGeneratorTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularVelocity), prtyProperty::eNoUndo);
		m_Data.m_AngularAccelerationMin.SetValue(m_pGeneratorTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_MinAngularAcceleration), prtyProperty::eNoUndo);
		m_Data.m_AngularAccelerationMax.SetValue(m_pGeneratorTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularAcceleration), prtyProperty::eNoUndo);

		//	texture alpha
		const an3StateAnimation<float>* pKA = dynamic_cast<const an3StateAnimation<float>*>(&(m_pGeneratorTemplate->GetAlphaAnimation()));
		if ( pKA != 0)
		{
			m_Data.m_TextureAlphaStart.SetValue(pKA->GetFirstValue(), prtyProperty::eNoUndo);
			m_Data.m_TextureAlphaMiddle.SetValue(pKA->GetSecondValue(), prtyProperty::eNoUndo);
			m_Data.m_TextureAlphaEnd.SetValue(pKA->GetThirdValue(), prtyProperty::eNoUndo);
			m_Data.m_TextureAlphaMiddlePercentStart.SetValue(pKA->GetMidTimeStart(), prtyProperty::eNoUndo);
			m_Data.m_TextureAlphaMiddlePercentEnd.SetValue(pKA->GetMidTimeEnd(), prtyProperty::eNoUndo);
		}

		m_Data.m_TextureFilename.SetValue(m_pGeneratorTemplate->GetTextureLocator().GetLastName(), prtyProperty::eNoUndo);
		prtSpriteGroupParticleGenerator *pSGPG = dynamic_cast<prtSpriteGroupParticleGenerator*>(m_pGenerator);
		if (pSGPG)
		{
			matTexture* pTexture = m_pGeneratorTemplate->GetTexture();
			if (pTexture != NULL)
			{
				matUVATexture* pUVAT = dynamic_cast<matUVATexture*>(pTexture);
				if (pUVAT != NULL)
				{
					m_Data.m_TextureRows.SetValue( pUVAT->GetNumHeightFrames(), prtyProperty::eNoUndo );
					m_Data.m_TextureCols.SetValue( pUVAT->GetNumWidthFrames(), prtyProperty::eNoUndo );
					m_Data.m_bTextureLooping.SetValue( pUVAT->GetLooping(), prtyProperty::eNoUndo );
					m_Data.m_bTextureReverse.SetValue( pUVAT->GetReversing(), prtyProperty::eNoUndo );
					m_Data.m_TextureRate.SetValue( pUVAT->GetFrameRate(), prtyProperty::eNoUndo );
					m_Data.m_TextureUVAMode.SetValue( pSGPG->GetUVAMode(), prtyProperty::eNoUndo );
				}
			}
		}

		// Streaking
		m_Data.m_bRenderStreaks.SetValue(m_pGeneratorTemplate->GetRenderStreaks(), prtyProperty::eNoUndo );
		m_Data.m_StreakLength.SetValue(m_pGeneratorTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_StreakLength), prtyProperty::eNoUndo);
		m_Data.m_StreakTaper.SetValue(m_pGeneratorTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_StreakTaper), prtyProperty::eNoUndo);
		m_Data.m_StreakFade.SetValue(m_pGeneratorTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_StreakFade), prtyProperty::eNoUndo);
	}

	if (m_pGeneratorTemplate->GetType() == prtParticleGeneratorTemplate::e_Cone)
	{
		// prtConeParticleGenerator
		m_Data.m_ConeAngle.SetValue(m_pGeneratorTemplate->GetParameter(prtConeParticleGenerator::e_ConeAngle), prtyProperty::eNoUndo);
		m_Data.m_MinSpeed.SetValue(m_pGeneratorTemplate->GetParameter(prtConeParticleGenerator::e_MinSpeed), prtyProperty::eNoUndo);
		m_Data.m_MaxSpeed.SetValue(m_pGeneratorTemplate->GetParameter(prtConeParticleGenerator::e_MaxSpeed), prtyProperty::eNoUndo);
		m_Data.m_AccelerationX.SetValue(m_pGeneratorTemplate->GetParameter(prtConeParticleGenerator::e_AccelerationX), prtyProperty::eNoUndo);
		m_Data.m_AccelerationY.SetValue(m_pGeneratorTemplate->GetParameter(prtConeParticleGenerator::e_AccelerationY), prtyProperty::eNoUndo);
		m_Data.m_AccelerationZ.SetValue(m_pGeneratorTemplate->GetParameter(prtConeParticleGenerator::e_AccelerationZ), prtyProperty::eNoUndo);
	}
	else if (m_pGeneratorTemplate->GetType() == prtParticleGeneratorTemplate::e_Spiral)
	{
		// prtSpiralParticleGenerator
		m_Data.m_MinEmitSpeed.SetValue(m_pGeneratorTemplate->GetParameter(prtSpiralParticleGenerator::e_MinEmitSpeed), prtyProperty::eNoUndo);
		m_Data.m_MaxEmitSpeed.SetValue(m_pGeneratorTemplate->GetParameter(prtSpiralParticleGenerator::e_MaxEmitSpeed), prtyProperty::eNoUndo);
		m_Data.m_EmitDirectionX.SetValue(m_pGeneratorTemplate->GetParameter(prtSpiralParticleGenerator::e_EmitDirectionX), prtyProperty::eNoUndo);
		m_Data.m_EmitDirectionY.SetValue(m_pGeneratorTemplate->GetParameter(prtSpiralParticleGenerator::e_EmitDirectionY), prtyProperty::eNoUndo);
		m_Data.m_EmitDirectionZ.SetValue(m_pGeneratorTemplate->GetParameter(prtSpiralParticleGenerator::e_EmitDirectionZ), prtyProperty::eNoUndo);
		m_Data.m_MinRotStartAngle.SetValue(m_pGeneratorTemplate->GetParameter(prtSpiralParticleGenerator::e_MinRotStartAngle), prtyProperty::eNoUndo);
		m_Data.m_MaxRotStartAngle.SetValue(m_pGeneratorTemplate->GetParameter(prtSpiralParticleGenerator::e_MaxRotStartAngle), prtyProperty::eNoUndo);
		m_Data.m_MinRotAngularVel.SetValue(m_pGeneratorTemplate->GetParameter(prtSpiralParticleGenerator::e_MinRotAngularVel), prtyProperty::eNoUndo);
		m_Data.m_MaxRotAngularVel.SetValue(m_pGeneratorTemplate->GetParameter(prtSpiralParticleGenerator::e_MaxRotAngularVel), prtyProperty::eNoUndo);
		m_Data.m_RotRadius.SetValue(m_pGeneratorTemplate->GetParameter(prtSpiralParticleGenerator::e_RotRadius), prtyProperty::eNoUndo);
		m_Data.m_RotRadiusScaleRate.SetValue(m_pGeneratorTemplate->GetParameter(prtSpiralParticleGenerator::e_RotRadiusScaleRate), prtyProperty::eNoUndo);
	}
}

//--------------------------------------------------------------------
// Clear out already created particles
//--------------------------------------------------------------------
void prtclObject::ClearParticles()
{
	m_pGenerator->DeleteAllParticles();
	m_pGenerator->ClearParticleAccumulation();
}

//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName&	prtclObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName&	prtclObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
// Position property access
//--------------------------------------------------------------------
prtyPoint3d&	prtclObject::PropertyPosition()
{
	return m_Data.m_Position;
}
const prtyPoint3d&	prtclObject::GetPropertyPosition() const
{
	return m_Data.m_Position;
}

//--------------------------------------------------------------------
// Orientation property access
//--------------------------------------------------------------------
prtyRotation&	prtclObject::PropertyOrientation()
{
	return m_Data.m_Orientation;
}
const prtyRotation&	prtclObject::GetPropertyOrientation() const
{
	return m_Data.m_Orientation;
}

//--------------------------------------------------------------------
// Rate property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyRate()
{
	return m_Data.m_Rate;
}
const prtyFloat&	prtclObject::GetPropertyRate() const
{
	return m_Data.m_Rate;
}

//--------------------------------------------------------------------
// MaxParticles property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyMaxParticles()
{
	return m_Data.m_MaxParticles;
}
const prtyFloat&	prtclObject::GetPropertyMaxParticles() const
{
	return m_Data.m_MaxParticles;
}

//--------------------------------------------------------------------
// LifetimeMin property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyLifetimeMin()
{
	return m_Data.m_LifetimeMin;
}
const prtyFloat&	prtclObject::GetPropertyLifetimeMin() const
{
	return m_Data.m_LifetimeMin;
}

//--------------------------------------------------------------------
// LifetimeMax property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyLifetimeMax()
{
	return m_Data.m_LifetimeMax;
}
const prtyFloat&	prtclObject::GetPropertyLifetimeMax() const
{
	return m_Data.m_LifetimeMax;
}

//--------------------------------------------------------------------
// ScaleStart property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyScaleStart()
{
	return m_Data.m_ScaleStart;
}
const prtyFloat&	prtclObject::GetPropertyScaleStart() const
{
	return m_Data.m_ScaleStart;
}

//--------------------------------------------------------------------
// ScaleCoefficient property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyScaleCoefficient()
{
	return m_Data.m_ScaleCoefficient;
}
const prtyFloat&	prtclObject::GetPropertyScaleCoefficient() const
{
	return m_Data.m_ScaleCoefficient;
}

//--------------------------------------------------------------------
// StartAngleMin property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyStartAngleMin()
{
	return m_Data.m_StartAngleMin;
}
const prtyFloat&	prtclObject::GetPropertyStartAngleMin() const
{
	return m_Data.m_StartAngleMin;
}

//--------------------------------------------------------------------
// StartAngleMax property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyStartAngleMax()
{
	return m_Data.m_StartAngleMax;
}
const prtyFloat&	prtclObject::GetPropertyStartAngleMax() const
{
	return m_Data.m_StartAngleMax;
}

//--------------------------------------------------------------------
// AngularVelocityMin property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyAngularVelocityMin()
{
	return m_Data.m_AngularVelocityMin;
}
const prtyFloat&	prtclObject::GetPropertyAngularVelocityMin() const
{
	return m_Data.m_AngularVelocityMin;
}

//--------------------------------------------------------------------
// AngularVelocityMax property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyAngularVelocityMax()
{
	return m_Data.m_AngularVelocityMax;
}
const prtyFloat&	prtclObject::GetPropertyAngularVelocityMax() const
{
	return m_Data.m_AngularVelocityMax;
}

//--------------------------------------------------------------------
// AngularAccelerationMin property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyAngularAccelerationMin()
{
	return m_Data.m_AngularAccelerationMin;
}
const prtyFloat&	prtclObject::GetPropertyAngularAccelerationMin() const
{
	return m_Data.m_AngularAccelerationMin;
}

//--------------------------------------------------------------------
// AngularAccelerationMax property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyAngularAccelerationMax()
{
	return m_Data.m_AngularAccelerationMax;
}
const prtyFloat&	prtclObject::GetPropertyAngularAccelerationMax() const
{
	return m_Data.m_AngularAccelerationMax;
}

//--------------------------------------------------------------------
// EmitterScale property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyEmitterScale()
{
	return m_Data.m_EmitterScale;
}
const prtyFloat&	prtclObject::GetPropertyEmitterScale() const
{
	return m_Data.m_EmitterScale;
}

//--------------------------------------------------------------------
// ConeAngle property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyConeAngle()
{
	return m_Data.m_ConeAngle;
}
const prtyFloat&	prtclObject::GetPropertyConeAngle() const
{
	return m_Data.m_ConeAngle;
}

//--------------------------------------------------------------------
// MinSpeed property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyMinSpeed()
{
	return m_Data.m_MinSpeed;
}
const prtyFloat&	prtclObject::GetPropertyMinSpeed() const
{
	return m_Data.m_MinSpeed;
}

//--------------------------------------------------------------------
// MaxSpeed property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyMaxSpeed()
{
	return m_Data.m_MaxSpeed;
}
const prtyFloat&	prtclObject::GetPropertyMaxSpeed() const
{
	return m_Data.m_MaxSpeed;
}

//--------------------------------------------------------------------
// AccelerationX property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyAccelerationX()
{
	return m_Data.m_AccelerationX;
}
const prtyFloat&	prtclObject::GetPropertyAccelerationX() const
{
	return m_Data.m_AccelerationX;
}

//--------------------------------------------------------------------
// AccelerationY property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyAccelerationY()
{
	return m_Data.m_AccelerationY;
}
const prtyFloat&	prtclObject::GetPropertyAccelerationY() const
{
	return m_Data.m_AccelerationY;
}

//--------------------------------------------------------------------
// AccelerationZ property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyAccelerationZ()
{
	return m_Data.m_AccelerationZ;
}
const prtyFloat&	prtclObject::GetPropertyAccelerationZ() const
{
	return m_Data.m_AccelerationZ;
}

//--------------------------------------------------------------------
// MinEmitSpeed property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyMinEmitSpeed()
{
	return m_Data.m_MinEmitSpeed;
}
const prtyFloat&	prtclObject::GetPropertyMinEmitSpeed() const
{
	return m_Data.m_MinEmitSpeed;
}

//--------------------------------------------------------------------
// MaxEmitSpeed property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyMaxEmitSpeed()
{
	return m_Data.m_MaxEmitSpeed;
}
const prtyFloat&	prtclObject::GetPropertyMaxEmitSpeed() const
{
	return m_Data.m_MaxEmitSpeed;
}

//--------------------------------------------------------------------
// EmitDirectionX property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyEmitDirectionX()
{
	return m_Data.m_EmitDirectionX;
}
const prtyFloat&	prtclObject::GetPropertyEmitDirectionX() const
{
	return m_Data.m_EmitDirectionX;
}

//--------------------------------------------------------------------
// EmitDirectionY property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyEmitDirectionY()
{
	return m_Data.m_EmitDirectionY;
}
const prtyFloat&	prtclObject::GetPropertyEmitDirectionY() const
{
	return m_Data.m_EmitDirectionY;
}

//--------------------------------------------------------------------
// EmitDirectionZ property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyEmitDirectionZ()
{
	return m_Data.m_EmitDirectionZ;
}
const prtyFloat&	prtclObject::GetPropertyEmitDirectionZ() const
{
	return m_Data.m_EmitDirectionZ;
}

//--------------------------------------------------------------------
// MinRotStartAngle property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyMinRotStartAngle()
{
	return m_Data.m_MinRotStartAngle;
}
const prtyFloat&	prtclObject::GetPropertyMinRotStartAngle() const
{
	return m_Data.m_MinRotStartAngle;
}

//--------------------------------------------------------------------
// MaxRotStartAngle property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyMaxRotStartAngle()
{
	return m_Data.m_MaxRotStartAngle;
}
const prtyFloat&	prtclObject::GetPropertyMaxRotStartAngle() const
{
	return m_Data.m_MaxRotStartAngle;
}

//--------------------------------------------------------------------
// MinRotAngularVel property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyMinRotAngularVel()
{
	return m_Data.m_MinRotAngularVel;
}
const prtyFloat&	prtclObject::GetPropertyMinRotAngularVel() const
{
	return m_Data.m_MinRotAngularVel;
}

//--------------------------------------------------------------------
// MaxRotAngularVel property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyMaxRotAngularVel()
{
	return m_Data.m_MaxRotAngularVel;
}
const prtyFloat&	prtclObject::GetPropertyMaxRotAngularVel() const
{
	return m_Data.m_MaxRotAngularVel;
}

//--------------------------------------------------------------------
// PreSimTime property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyPreSimTime()
{
	return m_Data.m_PreSimTime;
}
const prtyFloat&	prtclObject::GetPropertyPreSimTime() const
{
	return m_Data.m_PreSimTime;
}

//--------------------------------------------------------------------
// RotRadius property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyRotRadius()
{
	return m_Data.m_RotRadius;
}
const prtyFloat&	prtclObject::GetPropertyRotRadius() const
{
	return m_Data.m_RotRadius;
}

//--------------------------------------------------------------------
// RotRadiusScaleRate property access
//--------------------------------------------------------------------
prtyFloat&	prtclObject::PropertyRotRadiusScaleRate()
{
	return m_Data.m_RotRadiusScaleRate;
}
const prtyFloat&	prtclObject::GetPropertyRotRadiusScaleRate() const
{
	return m_Data.m_RotRadiusScaleRate;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyFloat& prtclObject::PropertyTextureAlphaStart()
{
	return m_Data.m_TextureAlphaStart;
}
const prtyFloat& prtclObject::GetPropertyTextureAlphaStart() const
{
	return m_Data.m_TextureAlphaStart;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyFloat& prtclObject::PropertyTextureAlphaMiddle()
{
	return m_Data.m_TextureAlphaMiddle;
}
const prtyFloat& prtclObject::GetPropertyTextureAlphaMiddle() const
{
	return m_Data.m_TextureAlphaMiddle;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyFloat& prtclObject::PropertyTextureAlphaEnd()
{
	return m_Data.m_TextureAlphaEnd;
}
const prtyFloat& prtclObject::GetPropertyTextureAlphaEnd() const
{
	return m_Data.m_TextureAlphaEnd;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyFloat& prtclObject::PropertyTextureAlphaMiddlePercentStart()
{
	return m_Data.m_TextureAlphaMiddlePercentStart;
}
const prtyFloat& prtclObject::GetPropertyTextureAlphaMiddlePercentStart() const
{
	return m_Data.m_TextureAlphaMiddlePercentStart;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyFloat& prtclObject::PropertyTextureAlphaMiddlePercentEnd()
{
	return m_Data.m_TextureAlphaMiddlePercentEnd;
}
const prtyFloat& prtclObject::GetPropertyTextureAlphaMiddlePercentEnd() const
{
	return m_Data.m_TextureAlphaMiddlePercentEnd;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyFileName& prtclObject::PropertyTextureFilename()
{
	return m_Data.m_TextureFilename;
}
const prtyFileName& prtclObject::GetPropertyTextureFilename() const
{
	return m_Data.m_TextureFilename;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyInt8& prtclObject::PropertyTextureRows()
{
	return m_Data.m_TextureRows;
}
const prtyInt8& prtclObject::GetPropertyTextureRows() const
{
	return m_Data.m_TextureRows;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyInt8& prtclObject::PropertyTextureCols()
{
	return m_Data.m_TextureCols;
}
const prtyInt8& prtclObject::GetPropertyTextureCols() const
{
	return m_Data.m_TextureCols;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyBoolean& prtclObject::PropertyTextureLooping()
{
	return m_Data.m_bTextureLooping;
}
const prtyBoolean& prtclObject::GetPropertyTextureLooping() const
{
	return m_Data.m_bTextureLooping;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyBoolean& prtclObject::PropertyTextureReverse()
{
	return m_Data.m_bTextureReverse;
}
const prtyBoolean& prtclObject::GetPropertyTextureReverse() const
{
	return m_Data.m_bTextureReverse;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyFloat& prtclObject::PropertyTextureRate()
{
	return m_Data.m_TextureRate;
}
const prtyFloat& prtclObject::GetPropertyTextureRate() const
{
	return m_Data.m_TextureRate;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyBoolean& prtclObject::PropertyRenderStreaks()
{
	return m_Data.m_bRenderStreaks;
}
const prtyBoolean& prtclObject::GetPropertyRenderStreaks() const
{
	return m_Data.m_bRenderStreaks;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyFloat& prtclObject::PropertyStreakLength()
{
	return m_Data.m_StreakLength;
}
const prtyFloat& prtclObject::GetPropertyStreakLength() const
{
	return m_Data.m_StreakLength;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyFloat& prtclObject::PropertyStreakTaper()
{
	return m_Data.m_StreakTaper;
}
const prtyFloat& prtclObject::GetPropertyStreakTaper() const
{
	return m_Data.m_StreakTaper;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyFloat& prtclObject::PropertyStreakFade()
{
	return m_Data.m_StreakFade;
}
const prtyFloat& prtclObject::GetPropertyStreakFade() const
{
	return m_Data.m_StreakFade;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyBoolean& prtclObject::PropertyShowInCubeReflections()
{
	return m_Data.m_bShowInCubeReflections;
}
const prtyBoolean& prtclObject::GetPropertyShowInCubeReflections() const
{
	return m_Data.m_bShowInCubeReflections;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyBoolean& prtclObject::PropertyShowInPlanarReflections()
{
	return m_Data.m_bShowInPlanarReflections;
}
const prtyBoolean& prtclObject::GetPropertyShowInPlanarReflections() const
{
	return m_Data.m_bShowInPlanarReflections;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyBoolean& prtclObject::PropertyCastShadows()
{
	return m_Data.m_bCastShadows;
}
const prtyBoolean& prtclObject::GetPropertyCastShadows() const
{
	return m_Data.m_bCastShadows;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyBoolean& prtclObject::PropertyUseDitheredShadows()
{
	return m_Data.m_bUseDitheredShadows;
}
const prtyBoolean& prtclObject::GetPropertyUseDitheredShadows() const
{
	return m_Data.m_bUseDitheredShadows;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyFloat& prtclObject::PropertyShadowDitherBias()
{
	return m_Data.m_ShadowDitherBias;
}
const prtyFloat& prtclObject::GetPropertyShadowDitherBias() const
{
	return m_Data.m_ShadowDitherBias;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyBoolean& prtclObject::PropertyAdditive()
{
	return m_Data.m_bAdditive;
}
const prtyBoolean& prtclObject::GetPropertyAdditive() const
{
	return m_Data.m_bAdditive;
}

//----------------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the prtclObject.
//----------------------------------------------------------------------------
const maAxisBox& prtclObject::GetWorldBox() const
{
	// First try to make the world box based on the particles themselves
	m_WorldBox = m_pGenerator->GetWorldBox();
	
	// If that boy is empty (because no particles have been generated)
	// then use the icon object as the bounding box
	if (m_WorldBox.IsEmpty())
		m_WorldBox = m_pDebugObject->GetWorldBox();

	return m_WorldBox;
}

//----------------------------------------------------------------------------
//	GetLocalBox returns a box which would enclose the prtclObject
//	if its transformations were identity
//----------------------------------------------------------------------------
const maAxisBox& prtclObject::GetLocalBox() const
{
	return l_SphereBox;
}

//--------------------------------------------------------------------
// UpdateName() is called when the gui sets the name of the object,
//	derived classes can set dirty bits and do "undo" operations, etc.
// The default behavior calls SetName()
//--------------------------------------------------------------------
void prtclObject::UpdateName(const std::string& i_Name)
{
	prtclOperations::ChangeName( i_Name );
}


//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void prtclObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
	nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number). The property's check will prevent an infinite
	// loop here.
	m_Data.m_Name = this->GetName();
}

//--------------------------------------------------------------------
//	Position
//--------------------------------------------------------------------
maPoint3d prtclObject::GetPosition() const
{
	return m_Data.m_Position.GetValue();
}
void prtclObject::UpdatePosition(const maPoint3d& i_Position, 
										bool i_bNewOperation)
{
	//prtclOperations::ChangePosition( i_Position );
	m_Data.m_Position.SetValue(i_Position, 
		i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
}


//----------------------------------------------------------------------------
//	Orientation
//----------------------------------------------------------------------------
maRotation prtclObject::GetOrientation() const
{
	return m_Data.m_Orientation.GetQuaternion();
}
void prtclObject::UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation)
{
	//prtclOperations::ChangeOrientation( i_Orientation );
	m_Data.m_Orientation.SetQuaternion(i_Orientation, 
		i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
}

//----------------------------------------------------------------------------
//	Scale
//----------------------------------------------------------------------------
maPoint3d prtclObject::GetScale() const
{
	// Scale cannot change
	return l_One;
}
void prtclObject::UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation)
{
	// no scale changes for particles
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the prtclObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool prtclObject::RayPick(	const maPoint3d& i_RayStart,
							const maPoint3d& i_RayEnd,
							float& o_T)
{	
	if (!m_pObject->GetRenderable())
		return false;

	return geoRayIntersection::IntersectLineSphere(	i_RayStart,
								i_RayEnd - i_RayStart,
								m_Data.m_Position.GetValue(),
								l_PickRadius,
								o_T);
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool prtclObject::MatchPickCode(envType::UInt32 i_PickCode) const
{
	if (!m_pDebugObject->GetRenderable())
		return false;

	return (m_pDebugObject->ContainsPickCode(i_PickCode));
}

//--------------------------------------------------------------------
//  Changes visible state of character based on GUI
//--------------------------------------------------------------------
void  prtclObject::SetEditorVisible(bool i_bVisible)
{
	m_Data.m_bEditorVisible.SetValue(i_bVisible);

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	//bool bRenderable = m_Data.m_bVisible.GetValue() && m_Data.m_bEditorVisible.GetValue() && m_bLayerVisible;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() && m_bLayerVisible;
	m_pObject->SetRenderable(bRenderable);

	// Debug icons only visible if ShowIcons is on and otherwise visible
	m_pDebugObject->SetRenderable(m_bShowIcons && bRenderable);
}
bool prtclObject::GetEditorVisible() const
{
	return m_Data.m_bEditorVisible.GetValue();
}

//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
void prtclObject::SetLayerVisible(bool i_bVisible)
{
	m_bLayerVisible = i_bVisible;

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() && m_bLayerVisible;
	m_pObject->SetRenderable(bRenderable);

	// Debug icons only visible if ShowIcons is on and otherwise visible
	m_pDebugObject->SetRenderable(m_bShowIcons && bRenderable);
}

//--------------------------------------------------------------------
//	GPUPickable sets whether the icons should be rendered
//	in pick renders when doing GPU picking
//--------------------------------------------------------------------
void prtclObject::SetGPUPickable(bool i_Pickable)
{
	m_pDebugObject->SetGPUPickable(i_Pickable);
}

//--------------------------------------------------------------------
//	Wireframe represents if the objects are rendered
//	in a wireframe style.
//--------------------------------------------------------------------
void prtclObject::SetWireframe(bool i_bWireframe)
{
	m_pDebugObject->SetWireframe(i_bWireframe);
	m_pObject->SetWireframe(i_bWireframe);
}

//--------------------------------------------------------------------
//	ShowIcons sets whether the debug shape is visible
//--------------------------------------------------------------------
void prtclObject::ShowIcons(bool i_bVisible)
{
	m_bShowIcons = i_bVisible;
	m_pDebugObject->SetRenderable(m_bShowIcons && m_Data.m_bEditorVisible.GetValue());
}

//----------------------------------------------------------------------------
//	GetDefaultTerrainOffset is the desired offset from
//	the terrain for this object.  This can be altered
//	by the user during placement
//----------------------------------------------------------------------------
float prtclObject::GetDefaultTerrainOffset() const
{
	return 0.0f;
}

//----------------------------------------------------------------------------
// Flags for how object can be modified
//----------------------------------------------------------------------------
mnmObject::RotateFlags	prtclObject::GetRotateFlags()
{
	return mnmObject::e_RotateAll;
}
mnmObject::ScaleFlags	prtclObject::GetScaleFlags()
{
	return mnmObject::e_ScaleNone;
}
mnmObject::TranslateFlags	prtclObject::GetTranslateFlags()
{
	return mnmObject::e_TranslateAll;
}


//--------------------------------------------------------------------
//	Get reference for given name.  The returned pointer is owned
//	by this object.  The object should be retained by the caller
//	to avoid repeated string searches.
//--------------------------------------------------------------------
//api3dReference* prtclObject::GetReference(const char* i_Name)
//{
//	// Attachment is through the "position" channel
//	api3dReference* pRef = new mnmReferencePosChannel(*m_pChannelPos);
//	this->AddReference(pRef);
//	return pRef;
//}

//--------------------------------------------------------------------
// Get list of references for possible attachment within this object.
//--------------------------------------------------------------------
void prtclObject::GetReferenceList(std::vector<std::string> &o_List)
{
//	m_pObject->GetReferenceList(o_List);
}

//----------------------------------------------------------------------------
// Get parent object of the spline in order for selection to trace from
//	this spline back to the controlled object.
//----------------------------------------------------------------------------
//virtual 
pick3dPickObject* prtclObject::GetParentObject() const
{
	return m_pParent;
}
//virtual 
void prtclObject::SetParentObject(pick3dPickObject* i_pParent)
{
	m_pParent = i_pParent;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclObject::Pause(bool i_bPause)
{
	if (i_bPause)
		this->m_pGenerator->Pause();
	else
		this->m_pGenerator->UnPause();
}

//--------------------------------------------------------------------
//	Type of Generator
//--------------------------------------------------------------------
prtParticleGeneratorTemplate::Type prtclObject::GetGeneratorType()
{
	if (this->m_pGeneratorTemplate)
		return m_pGeneratorTemplate->GetType();
	return prtParticleGeneratorTemplate::e_Static;		// should never get here -- throw an exception?
}
prtParticleGeneratorTemplate::EmitterType prtclObject::GetEmitterType()
{
	if (this->m_pGeneratorTemplate)
		return m_pGeneratorTemplate->GetEmitterType();
	return prtParticleGeneratorTemplate::e_Point;		// should never get here -- throw an exception?
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void prtclObject::PositionChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	const maPoint3d& position = m_Data.m_Position.GetValue();

	m_pObject->SetPosition( position );

	m_pDebugObject->SetPosition(position);
	//m_WorldBox = m_pDebugObject->GetWorldBox();

	//DBG_LOG3( "PositionChanged (%6.3f, %6.3f, %6.3f)", position.GetX(), position.GetY(), position.GetZ() );


	if (i_bDirty)
		prtclDocumentChunk::ActiveDataChanged();
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclObject::OrientationChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	const maRotation& orientation = m_Data.m_Orientation.GetQuaternion();

	m_pObject->SetOrientation( orientation );

	if ( m_pDebugObject != 0 )
	{
		m_pDebugObject->SetOrientation( orientation );
	}

	if (i_bDirty)
		prtclDocumentChunk::ActiveDataChanged();
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );

	if (i_bDirty)
	{
		prtclDialogDataUtil::UpdateListDialog();
		prtclDocumentChunk::ActiveDataChanged();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclObject::EmitterDataChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
//	if (i_pProperty->GetPropertyName().compare("EmitterScale") == 0)
//		m_pGenerator->GetEmitter()->SetParameter( prtSpriteGroupParticleGenerator::e_EmitterScale, m_Data.m_EmitterScale.GetValue() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclObject::StreakDataChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// keep generator template in sync with generator (UpdateData uses vals from template)
	this->m_pGeneratorTemplate->SetRenderStreaks(m_Data.m_bRenderStreaks.GetValue());

	prtSpriteGroupParticleGenerator *pSGPG = dynamic_cast<prtSpriteGroupParticleGenerator*>(m_pGenerator);
	if (pSGPG)
	{
		pSGPG->SetRenderStreaks(m_Data.m_bRenderStreaks.GetValue());
		pSGPG->SetParameter( prtSpriteGroupParticleGenerator::e_StreakLength, m_Data.m_StreakLength.GetValue() );
		pSGPG->SetParameter( prtSpriteGroupParticleGenerator::e_StreakTaper, m_Data.m_StreakTaper.GetValue() );
		pSGPG->SetParameter( prtSpriteGroupParticleGenerator::e_StreakFade, m_Data.m_StreakFade.GetValue() );
		pSGPG->MarkAnimDirty(); // Force animation so that streak length change is visible immediately
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclObject::RenderDataChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	prtSpriteGroupParticleGenerator *pSGPG = dynamic_cast<prtSpriteGroupParticleGenerator*>(m_pGenerator);
	if (pSGPG)
	{
		pSGPG->SetCastsShadow( m_Data.m_bCastShadows.GetValue() );
		pSGPG->SetShadowDithering( m_Data.m_bUseDitheredShadows.GetValue(), m_Data.m_ShadowDitherBias.GetValue() );
		pSGPG->SetRenderableInReflections( m_Data.m_bShowInPlanarReflections.GetValue(), 
			m_Data.m_bShowInCubeReflections.GetValue() );
		pSGPG->SetRenderMode( m_Data.m_bAdditive.GetValue() ? 
			prtSpriteGroupParticleGenerator::e_Additive : prtSpriteGroupParticleGenerator::e_Multiplicative );

		pSGPG->MarkAnimDirty(); // Force animation so that streak length change is visible immediately
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclObject::BaseGeneratorDataChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_pProperty->GetPropertyName().compare("Rate") == 0)
		m_pGenerator->SetParameter( prtParticleGenerator::e_ParticleRate, m_Data.m_Rate.GetValue() );
	if (i_pProperty->GetPropertyName().compare("LifetimeMin") == 0)
		m_pGenerator->SetParameter( prtParticleGenerator::e_MinParticleLifetime, m_Data.m_LifetimeMin.GetValue() );
	if (i_pProperty->GetPropertyName().compare("LifetimeMax") == 0)
		m_pGenerator->SetParameter( prtParticleGenerator::e_MaxParticleLifetime, m_Data.m_LifetimeMax.GetValue() );
	if (i_pProperty->GetPropertyName().compare("MaxParticles") == 0)
		m_pGenerator->SetParameter( prtParticleGenerator::e_MaxParticles, m_Data.m_MaxParticles.GetValue() );
	if (i_pProperty->GetPropertyName().compare("PreSimTime") == 0)
		m_pGenerator->SetParameter( prtParticleGenerator::e_PreSimTime, m_Data.m_PreSimTime.GetValue() );

	m_pGenerator->MarkAnimDirty(); // Force animation so that param change is visible immediately
}

//--------------------------------------------------------------------
//prtSpriteGroupParticleGenerator
//--------------------------------------------------------------------
void prtclObject::SpriteGeneratorDataChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_pProperty->GetPropertyName().compare("ScaleStart") == 0)
		m_pGenerator->SetParameter( prtSpriteGroupParticleGenerator::e_InitialScale, m_Data.m_ScaleStart.GetValue() );
	if (i_pProperty->GetPropertyName().compare("ScaleCoefficient") == 0)
		m_pGenerator->SetParameter( prtSpriteGroupParticleGenerator::e_ScaleCoeff, m_Data.m_ScaleCoefficient.GetValue() );
	if (i_pProperty->GetPropertyName().compare("ScaleMode") == 0)
	{
		// keep generator template in sync with generator (UpdateData uses vals from template)
		this->m_pGeneratorTemplate->SetScaleMode((prtSpriteGroupParticleGenerator::ScaleMode)m_Data.m_ScaleMode.GetValue());
		prtSpriteGroupParticleGenerator *pSGPG = dynamic_cast<prtSpriteGroupParticleGenerator*>(m_pGenerator);
		if (pSGPG)
		{
			pSGPG->SetScaleMode((prtSpriteGroupParticleGenerator::ScaleMode)m_Data.m_ScaleMode.GetValue());
		}
	}
	if (i_pProperty->GetPropertyName().compare("StartAngleMin") == 0)
		m_pGenerator->SetParameter( prtSpriteGroupParticleGenerator::e_MinStartAngle, m_Data.m_StartAngleMin.GetValue() );
	if (i_pProperty->GetPropertyName().compare("StartAngleMax") == 0)
		m_pGenerator->SetParameter( prtSpriteGroupParticleGenerator::e_MaxStartAngle, m_Data.m_StartAngleMax.GetValue() );
	if (i_pProperty->GetPropertyName().compare("AngularVelocityMin") == 0)
		m_pGenerator->SetParameter( prtSpriteGroupParticleGenerator::e_MinAngularVelocity, m_Data.m_AngularVelocityMin.GetValue() );
	if (i_pProperty->GetPropertyName().compare("AngularVelocityMax") == 0)
		m_pGenerator->SetParameter( prtSpriteGroupParticleGenerator::e_MaxAngularVelocity, m_Data.m_AngularVelocityMax.GetValue() );
	if (i_pProperty->GetPropertyName().compare("AngularAccelerationMin") == 0)
		m_pGenerator->SetParameter( prtSpriteGroupParticleGenerator::e_MinAngularAcceleration, m_Data.m_AngularAccelerationMin.GetValue() );
	if (i_pProperty->GetPropertyName().compare("AngularAccelerationMax") == 0)
		m_pGenerator->SetParameter( prtSpriteGroupParticleGenerator::e_MaxAngularAcceleration, m_Data.m_AngularAccelerationMax.GetValue() );

	m_pGenerator->MarkAnimDirty(); // Force animation so that param change is visible immediately
}

//--------------------------------------------------------------------
// prtConeParticleGenerator
//--------------------------------------------------------------------
void prtclObject::ConeGeneratorDataChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_pProperty->GetPropertyName().compare("ConeAngle") == 0)
		m_pGenerator->SetParameter( prtConeParticleGenerator::e_ConeAngle, m_Data.m_ConeAngle.GetValue() );
	if (i_pProperty->GetPropertyName().compare("MinSpeed") == 0)
		m_pGenerator->SetParameter( prtConeParticleGenerator::e_MinSpeed, m_Data.m_MinSpeed.GetValue() );
	if (i_pProperty->GetPropertyName().compare("MaxSpeed") == 0)
		m_pGenerator->SetParameter( prtConeParticleGenerator::e_MaxSpeed, m_Data.m_MaxSpeed.GetValue() );
	if (i_pProperty->GetPropertyName().compare("AccelerationX") == 0)
		m_pGenerator->SetParameter( prtConeParticleGenerator::e_AccelerationX, m_Data.m_AccelerationX.GetValue() );
	if (i_pProperty->GetPropertyName().compare("AccelerationY") == 0)
		m_pGenerator->SetParameter( prtConeParticleGenerator::e_AccelerationY, m_Data.m_AccelerationY.GetValue() );
	if (i_pProperty->GetPropertyName().compare("AccelerationZ") == 0)
		m_pGenerator->SetParameter( prtConeParticleGenerator::e_AccelerationZ, m_Data.m_AccelerationZ.GetValue() );

	m_pGenerator->MarkAnimDirty(); // Force animation so that param change is visible immediately
}

//--------------------------------------------------------------------
// prtSpiralParticleGenerator
//--------------------------------------------------------------------
void prtclObject::SpiralGeneratorDataChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_pProperty->GetPropertyName().compare("MinEmitSpeedStart") == 0)
		m_pGenerator->SetParameter( prtSpiralParticleGenerator::e_MinEmitSpeed, m_Data.m_MinEmitSpeed.GetValue() );
	if (i_pProperty->GetPropertyName().compare("MaxEmitSpeedStart") == 0)
		m_pGenerator->SetParameter( prtSpiralParticleGenerator::e_MaxEmitSpeed, m_Data.m_MaxEmitSpeed.GetValue() );
	if (i_pProperty->GetPropertyName().compare("EmitDirectionXStart") == 0)
		m_pGenerator->SetParameter( prtSpiralParticleGenerator::e_EmitDirectionX, m_Data.m_EmitDirectionX.GetValue() );
	if (i_pProperty->GetPropertyName().compare("EmitDirectionYStart") == 0)
		m_pGenerator->SetParameter( prtSpiralParticleGenerator::e_EmitDirectionY, m_Data.m_EmitDirectionY.GetValue() );
	if (i_pProperty->GetPropertyName().compare("EmitDirectionZStart") == 0)
		m_pGenerator->SetParameter( prtSpiralParticleGenerator::e_EmitDirectionZ, m_Data.m_EmitDirectionZ.GetValue() );
	if (i_pProperty->GetPropertyName().compare("MinRotStartAngleStart") == 0)
		m_pGenerator->SetParameter( prtSpiralParticleGenerator::e_MinRotStartAngle, m_Data.m_MinRotStartAngle.GetValue() );
	if (i_pProperty->GetPropertyName().compare("MaxRotStartAngleStart") == 0)
		m_pGenerator->SetParameter( prtSpiralParticleGenerator::e_MaxRotStartAngle, m_Data.m_MaxRotStartAngle.GetValue() );
	if (i_pProperty->GetPropertyName().compare("MinRotAngularVelStart") == 0)
		m_pGenerator->SetParameter( prtSpiralParticleGenerator::e_MinRotAngularVel, m_Data.m_MinRotAngularVel.GetValue() );
	if (i_pProperty->GetPropertyName().compare("MaxRotAngularVelStart") == 0)
		m_pGenerator->SetParameter( prtSpiralParticleGenerator::e_MaxRotAngularVel, m_Data.m_MaxRotAngularVel.GetValue() );
	if (i_pProperty->GetPropertyName().compare("RotRadiusStart") == 0)
		m_pGenerator->SetParameter( prtSpiralParticleGenerator::e_RotRadius, m_Data.m_RotRadius.GetValue() );
	if (i_pProperty->GetPropertyName().compare("RotRadiusScaleRateStart") == 0)
		m_pGenerator->SetParameter( prtSpiralParticleGenerator::e_RotRadiusScaleRate, m_Data.m_RotRadiusScaleRate.GetValue() );

	m_pGenerator->MarkAnimDirty(); // Force animation so that param change is visible immediately
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclObject::TextureAlphaDataChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	prtyFloat* pFloatProperty = dynamic_cast<prtyFloat*>(i_pProperty);
	if (pFloatProperty == 0)
		return;

	// get alpha profile
	//
	const an3StateAnimation<float>* gen_ap = dynamic_cast<const an3StateAnimation<float>*>(m_pGenerator->GetAlphaProfile());

	// copy the alpha profile
	//
	an3StateAnimation<float>* new_gen_ap = 0;
	
	// check property name and based on name, set the appropriate alpha profile
	//
	if (i_pProperty->GetPropertyName().compare("AlphaStart") == 0)
	{
		new_gen_ap = new an3StateAnimation<float>( pFloatProperty->GetValue(),
													gen_ap->GetSecondValue(),
													gen_ap->GetThirdValue(),
													gen_ap->GetMidTimeStart(),
													gen_ap->GetMidTimeEnd(),
													1.0f );
	}
	else if (i_pProperty->GetPropertyName().compare("AlphaMiddle") == 0)
	{
		new_gen_ap = new an3StateAnimation<float>( gen_ap->GetFirstValue(),
													pFloatProperty->GetValue(),
													gen_ap->GetThirdValue(),
													gen_ap->GetMidTimeStart(),
													gen_ap->GetMidTimeEnd(),
													1.0f );
	}
	else if (i_pProperty->GetPropertyName().compare("AlphaEnd") == 0)
	{
		new_gen_ap = new an3StateAnimation<float>( gen_ap->GetFirstValue(),
													gen_ap->GetSecondValue(),
													pFloatProperty->GetValue(),
													gen_ap->GetMidTimeStart(),
													gen_ap->GetMidTimeEnd(),
													1.0f );
	}
	else if (i_pProperty->GetPropertyName().compare("AlphaMiddlePercentStart") == 0)
	{
		new_gen_ap = new an3StateAnimation<float>( gen_ap->GetFirstValue(),
													gen_ap->GetSecondValue(),
													gen_ap->GetThirdValue(),
													pFloatProperty->GetValue(),
													gen_ap->GetMidTimeEnd(),
													1.0f );
	}
	else if (i_pProperty->GetPropertyName().compare("AlphaMiddlePercentEnd") == 0)
	{
		new_gen_ap = new an3StateAnimation<float>( gen_ap->GetFirstValue(),
													gen_ap->GetSecondValue(),
													gen_ap->GetThirdValue(),
													gen_ap->GetMidTimeStart(),
													pFloatProperty->GetValue(),
													1.0f );
	}

	// set the alpha profile
	//
	if (new_gen_ap != 0)
	{
		m_pGenerator->SetAlphaProfile( *new_gen_ap );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclObject::TextureDataChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	prtSpriteGroupParticleGenerator *pSGPG = dynamic_cast<prtSpriteGroupParticleGenerator*>(m_pGenerator);
	if (pSGPG)
	{
		matTexture* pTexture = m_pGeneratorTemplate->GetTexture();
		if (pTexture != NULL)
		{
			matUVATexture* pUVAT = dynamic_cast<matUVATexture*>(pTexture);
			if (pUVAT != NULL)
			{
				// check property name and based on name, set the appropriate alpha profile
				//
				if (i_pProperty->GetPropertyName().compare("TextureRows") == 0)
				{
					prtyInt8* pProperty = dynamic_cast<prtyInt8*>(i_pProperty);
					if (pProperty == 0)	return;

					pUVAT->SetNumHeightFrames(pProperty->GetValue());
					pSGPG->RemoveTextures();
					pSGPG->SetTexture( pUVAT );
				}
				else if (i_pProperty->GetPropertyName().compare("TextureCols") == 0)
				{
					prtyInt8* pProperty = dynamic_cast<prtyInt8*>(i_pProperty);
					if (pProperty == 0)	return;

					pUVAT->SetNumWidthFrames(pProperty->GetValue());
					pSGPG->RemoveTextures();
					pSGPG->SetTexture( pUVAT );
				}
				else if (i_pProperty->GetPropertyName().compare("TextureLoops") == 0)
				{
					prtyBoolean* pProperty = dynamic_cast<prtyBoolean*>(i_pProperty);
					if (pProperty == 0)	return;

					pUVAT->SetLooping(pProperty->GetValue());
					pSGPG->RemoveTextures();
					pSGPG->SetTexture( pUVAT );
				}
				else if (i_pProperty->GetPropertyName().compare("TextureReverse") == 0)
				{
					prtyBoolean* pProperty = dynamic_cast<prtyBoolean*>(i_pProperty);
					if (pProperty == 0)	return;

					pUVAT->SetReversing(pProperty->GetValue());
					pSGPG->RemoveTextures();
					pSGPG->SetTexture( pUVAT );
				}
				else if (i_pProperty->GetPropertyName().compare("TextureRate") == 0)
				{
					prtyFloat* pProperty = dynamic_cast<prtyFloat*>(i_pProperty);
					if (pProperty == 0)	return;

					pUVAT->SetFrameRate(pProperty->GetValue());
					pSGPG->RemoveTextures();
					pSGPG->SetTexture( pUVAT );
				}
				else if (i_pProperty->GetPropertyName().compare("TextureUVAMode") == 0)
				{
					prtyEnum* pProperty = dynamic_cast<prtyEnum*>(i_pProperty);
					if (pProperty == 0)	return;

					// keep generator template in sync with generator (UpdateData uses vals from template)
					m_pGeneratorTemplate->SetUVAMode((prtSpriteGroupParticleGenerator::UVAMode)pProperty->GetValue());
					pSGPG->SetUVAMode((prtSpriteGroupParticleGenerator::UVAMode)pProperty->GetValue());
				}
			}
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclObject::TextureFilenameDataChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	prtyFileName* pFilenameProperty = dynamic_cast<prtyFileName*>(i_pProperty);
	if (pFilenameProperty == NULL)
		return;

	if (m_Data.m_TextureFilename.GetValue().GetLength() == 0)
		return;

	prtSpriteGroupParticleGenerator* pSGPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(m_pGenerator);
	if (pSGPGen != 0)
	{
		//	get the file path
		//
		fsLocator tex_loc;
		fsysFileList file_list;
		prtclTextureList::BuildFileList(file_list);
		file_list.GetFilePath(m_Data.m_TextureFilename.GetValue(), tex_loc);
		tex_loc.Push(m_Data.m_TextureFilename.GetValue());

		// load texture to project (have to make sure this isn't a mip-map
		// texture, or else you get a line at the light's plane.)
		//
		// FIX - the texture filename coming in may be ".dds", but the
		//	texture loader will search for other extensions as well.  The
		//	result is that the filename stored in data fields (and the resource
		//	tracker) may have the wrong extension.  This is a problem when 
		//	trying to replace a file in the resource tracker. [rjk]
		//
//		m_pTexture = matTextureMgr::LoadTexture(tex_loc, false);
		//m_pTexture = matTextureMgrDX9::LoadPlainTexture(tex_loc, 0,0);

		//	replace the filename in the resource tracker
		//
//		fsResourceTracker::ReplaceFile(m_TextureNameLoaded, tex_loc);

		// TODO - MUST FIX THIS HARD-CODED STUFF!!!!!!!!
		//
		fsLocator particle_texture_locator = tex_loc; // = m_pGeneratorTemplate->GetGeometryLocator(); //->GetTextureLocator();
//		particle_texture_locator.Push( gfPaths::GetPath( mnmPaths::e_DataStock ) );
//		particle_texture_locator.Push("Effects");
//		particle_texture_locator.Push("General");
//		particle_texture_locator.Push("Textures");

		std::string texloc;
//		fsFileUtil::LocatorToANSIFilename(particle_texture_locator, texloc);
//		DBG_LOG1("particle texture %s", texloc.c_str());
		//fsFileUtil::LocatorToANSIFilename(pFilenameProperty->GetValue(), texloc);
		//DBG_LOG1("particle property texture %s", texloc.c_str());

//		particle_texture_locator.Pop();
//		particle_texture_locator.Pop();
//		particle_texture_locator.Push( gfPaths::GetSubPath(gfPaths::e_Textures) );
//		particle_texture_locator.Push( pFilenameProperty->GetValue() );

		fsFileUtil::LocatorToANSIFilename(particle_texture_locator, texloc);
		DBG_LOG1("final particle texture %s", texloc.c_str());

		//if ( particle_texture_locator != m_pGeneratorTemplate->GetTextureLocator())
		{
			DBG_LOG0("Changing Texture!");

			//	update the generator template
			m_pGeneratorTemplate->SetTextureLocator(particle_texture_locator);
			m_pGeneratorTemplate->MakeTexture();

			prtclObjectMgr::UpdateTemplateFromData( m_Data, m_pGeneratorTemplate );
			//UpdateData();

			ClearParticles();
			api3dScene::RemoveParticleGenerator(m_pObject);

			//	update the generator
			m_pObject->SetParticleGeneratorTemplate(m_pGeneratorTemplate);
			pSGPGen->RemoveTextures();
			pSGPGen->SetTexture( m_pGeneratorTemplate->GetTexture() );
			api3dScene::AddParticleGenerator(m_pObject);

			// update the ui
			matUVATexture* pUVATexture = dynamic_cast<matUVATexture*>(m_pGeneratorTemplate->GetTexture());
			if (pUVATexture)
			{
				m_Data.m_TextureRows.SetValue(pUVATexture->GetNumHeightFrames());
				m_Data.m_TextureCols.SetValue(pUVATexture->GetNumWidthFrames());
				m_Data.m_bTextureLooping.SetValue( pUVATexture->GetLooping() );
				m_Data.m_bTextureReverse.SetValue( pUVATexture->GetReversing() );
				m_Data.m_TextureRate.SetValue( pUVATexture->GetFrameRate() );
			}
		}
	}
}

//api3dParticleGenerator*		m_pObject;
//prtParticleGenerator*			m_pGenerator;
//	
//pick3dPickObject*				m_pParent;
//prtclData						m_Data;
//prtParticleGeneratorTemplate*	m_pGeneratorTemplate;
