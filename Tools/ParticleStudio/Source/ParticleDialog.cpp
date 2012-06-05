#include "StdAfx.h"
#include "ParticleDialog.h"

#include "ptclApp.hpp"
#include "ptclParticleTemplate.hpp"
#include "ptclLevel.hpp"

//	library
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/prt/prtConeParticleGenerator.hpp"
#include "Graphics/prt/prtSpiralParticleGenerator.hpp"
#include "Graphics/prt/prtGeneratorUtil.hpp"

//	tool includes
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiCustomDocHandler.hpp"
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"


//============================================================================
//============================================================================
using namespace ParticleStudio;
using namespace System::Windows;


//============================================================================
//============================================================================
ParticleDialog::ParticleDialog()
{
	m_bOurChange = false;
	m_bDisableNotify = true;

	InitializeComponent();

	// Dialog memory remembers size, location, visiblity of dialog 
	m_pMemory = gcnew tmaDialogMemory( this );

	//	First fill all the type-specific tab pages in with default information
	//	then update the controls with the "default particle generator" information.
	//	
	Initialize_TabPages();

	prtParticleGeneratorTemplate * pPGTemplate = (ptclParticleTemplate::GetTemplate());
	ptclLevel::SetParticleGeneratorAlphaProfile( &(pPGTemplate->GetAlphaAnimation()) );

	Update_Controls( pPGTemplate );

	m_bDisableNotify = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::button_open_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	guiCustomDocHandler::Open();

	//prtParticleGeneratorTemplate * pPGTemplate = (ptclParticleTemplate::GetTemplate());

	ForceReplace_ParticleGenerator();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::button_save_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	//	fill-in the template based on the tab pages
	prtParticleGeneratorTemplate * pPGTemplate = (ptclParticleTemplate::GetTemplate());
	if ( !pPGTemplate ) return;

	Update_ParticleTemplate( pPGTemplate );

	//	save the template
	guiCustomDocHandler::Save();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::button_saveas_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	//	fill-in the template based on the tab pages
	prtParticleGeneratorTemplate * pPGTemplate = (ptclParticleTemplate::GetTemplate());
	if ( !pPGTemplate ) return;

	Update_ParticleTemplate( pPGTemplate );

	//	save the template
	guiCustomDocHandler::SaveAs();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::radioButton_assettype_1texture_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::radioButton_assettype_geometry_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::fileChooser_texture_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	if ( fileChooser_texture->Fullpath )
	{
		if ( fileChooser_texture->Fullpath->Length > 0 )
		{
			std::string filename;
			tmaManagedStringUtils::ManagedStringToStdString( fileChooser_texture->Fullpath, filename );
			ptclLevel::SetParticleGeneratorTexture( filename );
		}

		update_controls_texturepath_changed();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::radioButton_texture_alphablending_add_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	if ( radioButton_texture_alphablending_add->Checked )
	{
		ptclLevel::SetParticleGeneratorTextureAlphaBlendingMode( prtSpriteGroupParticleGenerator::e_Additive );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::radioButton_texture_alphablending_multiply_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	if ( radioButton_texture_alphablending_multiply->Checked )
	{
		ptclLevel::SetParticleGeneratorTextureAlphaBlendingMode( prtSpriteGroupParticleGenerator::e_Exponential );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::rangedFloat_texture_alphalevel_middlepercentstart_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	if (rangedFloat_texture_alphalevel_middlepercentstart->Value > rangedFloat_texture_alphalevel_middlepercentend->Value)
	{
		m_bDisableNotify = true;
		rangedFloat_texture_alphalevel_middlepercentstart->Value = rangedFloat_texture_alphalevel_middlepercentend->Value;
		m_bDisableNotify = false;
	}

	Update_ParticleGenerator_AlphaProfile();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::rangedFloat_texture_alphalevel_end_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	Update_ParticleGenerator_AlphaProfile();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::rangedFloat_texture_alphalevel_middle_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	Update_ParticleGenerator_AlphaProfile();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::rangedFloat_texture_alphalevel_start_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	Update_ParticleGenerator_AlphaProfile();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::radioButton_texture_scaling_linear_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	if ( radioButton_texture_scaling_linear->Checked )
	{
		ptclLevel::SetParticleGeneratorTextureScaleMode( prtSpriteGroupParticleGenerator::e_Linear );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::radioButton_texture_scaling_exponential_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	if ( radioButton_texture_scaling_exponential->Checked )
	{
		ptclLevel::SetParticleGeneratorTextureScaleMode( prtSpriteGroupParticleGenerator::e_Exponential );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::radioButton_texture_UVA_Lifetime_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	if ( radioButton_texture_UVA_Lifetime->Checked )
	{
		ptclLevel::SetParticleGeneratorTextureUVAMode( prtSpriteGroupParticleGenerator::e_ScaleToLifetime );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::radioButton_texture_UVA_random_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	if ( radioButton_texture_UVA_random->Checked )
	{
		ptclLevel::SetParticleGeneratorTextureUVAMode( prtSpriteGroupParticleGenerator::e_RandomFrame );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::radioButton_texture_UVA_TUV_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if ( m_bDisableNotify ) return;

	if ( radioButton_texture_UVA_TUV->Checked )
	{
		ptclLevel::SetParticleGeneratorTextureUVAMode( prtSpriteGroupParticleGenerator::e_TUV );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::fileChooser_geometry_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	//if ( fileChooser_texture->get_Text() )
	//{
	//	if ( fileChooser_texture->get_Text()->get_Length() > 0 )
	//	{
	//		std::string filename;
	//		tmaManagedStringUtils::ManagedStringToStdString( fileChooser_texture->get_Text(), filename );
	//		ptclLevel::SetParticleGeneratorTexture( filename );
	//	}
	//}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_emitterscale_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_generic_particlelifetime_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtParticleGenerator* pPGen = ptclLevel::GetParticleGenerator();
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_MinParticleLifetime, (float)floatEdit_particledata_generic_particlelifetime_min->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_generic_particlelifetime_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtParticleGenerator* pPGen = ptclLevel::GetParticleGenerator();
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_MaxParticleLifetime, (float)floatEdit_particledata_generic_particlelifetime_max->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_generic_particlerate_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtParticleGenerator* pPGen = ptclLevel::GetParticleGenerator();
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_ParticleRate, (float)floatEdit_particledata_generic_particlerate->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_generic_maxparticles_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtParticleGenerator* pPGen = ptclLevel::GetParticleGenerator();
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_MaxParticles, (float)floatEdit_particledata_generic_maxparticles->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_generic_particlescale_start_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpriteGroupParticleGenerator* pPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_InitialScale, (float)floatEdit_particledata_generic_particlescale_start->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_generic_particlescalecoefficient_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpriteGroupParticleGenerator* pPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_ScaleCoeff, (float)floatEdit_particledata_generic_particlescalecoefficient->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_generic_particlestartangle_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpriteGroupParticleGenerator* pPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_MinStartAngle, (float)floatEdit_particledata_generic_particlestartangle_min->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_generic_particlestartangle_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpriteGroupParticleGenerator* pPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_MaxStartAngle, (float)floatEdit_particledata_generic_particlestartangle_max->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_generic_particleangularvelocity_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpriteGroupParticleGenerator* pPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_MinAngularVelocity, (float)floatEdit_particledata_generic_particleangularvelocity_min->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_generic_particleangularvelocity_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpriteGroupParticleGenerator* pPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_MaxAngularVelocity, (float)floatEdit_particledata_generic_particleangularvelocity_max->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_generic_particleangularacceleration_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpriteGroupParticleGenerator* pPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_MinAngularAcceleration, (float)floatEdit_particledata_generic_particleangularacceleration_min->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_generic_particleangularacceleration_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpriteGroupParticleGenerator* pPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_MaxAngularAcceleration, (float)floatEdit_particledata_generic_particleangularacceleration_max->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_generic_presimtime_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpriteGroupParticleGenerator* pPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_PreSimTime, (float)floatEdit_particledata_generic_presimtime->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_cone_coneangle_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtConeParticleGenerator* pPGen = dynamic_cast<prtConeParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtConeParticleGenerator::e_ConeAngle, (float)floatEdit_particledata_cone_coneangle->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_cone_speed_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtConeParticleGenerator* pPGen = dynamic_cast<prtConeParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtConeParticleGenerator::e_MinSpeed, (float)floatEdit_particledata_cone_speed_min->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_cone_speed_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtConeParticleGenerator* pPGen = dynamic_cast<prtConeParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtConeParticleGenerator::e_MaxSpeed, (float)floatEdit_particledata_cone_speed_max->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::vector3Edit_particledata_cone_acceleration_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtConeParticleGenerator* pPGen = dynamic_cast<prtConeParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtConeParticleGenerator::e_AccelerationX, (float)vector3Edit_particledata_cone_acceleration->ValueX );
	pPGen->SetParameter( prtConeParticleGenerator::e_AccelerationY, (float)vector3Edit_particledata_cone_acceleration->ValueY );
	pPGen->SetParameter( prtConeParticleGenerator::e_AccelerationZ, (float)vector3Edit_particledata_cone_acceleration->ValueZ );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_spiral_speed_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpiralParticleGenerator* pPGen = dynamic_cast<prtSpiralParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_MinEmitSpeed, (float)floatEdit_particledata_spiral_speed_min->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_spiral_speed_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpiralParticleGenerator* pPGen = dynamic_cast<prtSpiralParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_MaxEmitSpeed, (float)floatEdit_particledata_spiral_speed_max->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::vector3Edit_particledata_spiral_emitdirection_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpiralParticleGenerator* pPGen = dynamic_cast<prtSpiralParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_EmitDirectionX, (float)vector3Edit_particledata_spiral_emitdirection->ValueX );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_EmitDirectionY, (float)vector3Edit_particledata_spiral_emitdirection->ValueY );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_EmitDirectionZ, (float)vector3Edit_particledata_spiral_emitdirection->ValueZ );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::vector3Edit_particledata_spiral_acceleration_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpiralParticleGenerator* pPGen = dynamic_cast<prtSpiralParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_AccelerationX, (float)vector3Edit_particledata_spiral_acceleration->ValueX );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_AccelerationY, (float)vector3Edit_particledata_spiral_acceleration->ValueY );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_AccelerationZ, (float)vector3Edit_particledata_spiral_acceleration->ValueZ );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_spiral_rotationangle_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpiralParticleGenerator* pPGen = dynamic_cast<prtSpiralParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_MinRotStartAngle, (float)floatEdit_particledata_spiral_rotationangle_min->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_spiral_rotationangle_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpiralParticleGenerator* pPGen = dynamic_cast<prtSpiralParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_MaxRotStartAngle, (float)floatEdit_particledata_spiral_rotationangle_max->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_spiral_rotationangular_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpiralParticleGenerator* pPGen = dynamic_cast<prtSpiralParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_MinRotAngularVel, (float)floatEdit_particledata_spiral_rotationangular_min->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_spiral_rotationangular_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpiralParticleGenerator* pPGen = dynamic_cast<prtSpiralParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_MaxRotAngularVel, (float)floatEdit_particledata_spiral_rotationangular_max->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_spiral_rotationradius_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpiralParticleGenerator* pPGen = dynamic_cast<prtSpiralParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_RotRadius, (float)floatEdit_particledata_spiral_rotationradius->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::floatEdit_particledata_spiral_radiusscalerate_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtSpiralParticleGenerator* pPGen = dynamic_cast<prtSpiralParticleGenerator*>(ptclLevel::GetParticleGenerator());
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
	pPGen->SetParameter( prtSpiralParticleGenerator::e_RotRadiusScaleRate, (float)floatEdit_particledata_spiral_radiusscalerate->Value );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::checkedListBox_generatortype_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	checkedlistbox_keep_one_checked( checkedListBox_generatortype, checkedListBox_generatortype->SelectedIndex );

	if ( m_bDisableNotify ) return;

	Replace_ParticleGenerator();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::checkedListBox_emittertype_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	checkedlistbox_keep_one_checked( checkedListBox_emittertype, checkedListBox_emittertype->SelectedIndex );

	if ( m_bDisableNotify ) return;

	prtParticleGenerator* pPGen = ptclLevel::GetParticleGenerator();
	DBG_ASSERT0( pPGen != 0, "No particle generator" );

	String^ pString = checkedListBox_emittertype->SelectedItem->ToString();
	if (pString == "Block")
	{
		prtGeneratorUtil::ReplaceGeneratorEmitter( pPGen, (prtParticleGeneratorTemplate::e_Block) );
	}
	else if (pString == "Circle")
	{
		prtGeneratorUtil::ReplaceGeneratorEmitter( pPGen, (prtParticleGeneratorTemplate::e_Circle) );
	}
	else if (pString == "Point")
	{
		prtGeneratorUtil::ReplaceGeneratorEmitter( pPGen, (prtParticleGeneratorTemplate::e_Point) );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void ParticleDialog::rangedFloat_texture_alphalevel_middlepercentend_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	if (rangedFloat_texture_alphalevel_middlepercentend->Value < rangedFloat_texture_alphalevel_middlepercentstart->Value)
	{
		m_bDisableNotify = true;
		rangedFloat_texture_alphalevel_middlepercentend->Value = rangedFloat_texture_alphalevel_middlepercentstart->Value;
		m_bDisableNotify = false;
	}

	Update_ParticleGenerator_AlphaProfile();
}


System::Void ParticleDialog::floatEdit_texture_starttime_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtParticleGenerator* pPGen = ptclLevel::GetParticleGenerator();
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
//	pPGen->SetParameter( prtParticleGenerator::e_ParticleRate, (float)floatEdit_texture_starttime->Value );
}

System::Void ParticleDialog::floatEdit_texture_endtime_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtParticleGenerator* pPGen = ptclLevel::GetParticleGenerator();
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
//	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_ParticleRate, (float)floatEdit_texture_endtime->Value );
}

System::Void ParticleDialog::numericUpDown_texture_frames_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtParticleGenerator* pPGen = ptclLevel::GetParticleGenerator();
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
//	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_ParticleRate, (float)numericUpDown_texture_frames->Value );
}

System::Void ParticleDialog::numericUpDown_texture_framerate_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtParticleGenerator* pPGen = ptclLevel::GetParticleGenerator();
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
//	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_ParticleRate, (float)numericUpDown_texture_framerate->Value );
}

System::Void ParticleDialog::checkBox_texture_looping_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e)
{
	if ( m_bDisableNotify ) return;

	prtParticleGenerator* pPGen = ptclLevel::GetParticleGenerator();
	DBG_ASSERT0( pPGen != 0, "No particle generator" );
//	pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_ParticleRate, (float)checkBox_texture_looping->Value );
}

System::Void ParticleDialog::checkBox_streaking_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if ( m_bDisableNotify ) return;

	prtSpriteGroupParticleGenerator* pPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(ptclLevel::GetParticleGenerator());
	if (pPGen)
		pPGen->SetRenderStreaks( checkBox_streaking->Checked ); 
}

System::Void ParticleDialog::rangedFloat_streak_length_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if ( m_bDisableNotify ) return;
	
	prtSpriteGroupParticleGenerator* pPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(ptclLevel::GetParticleGenerator());
	if (pPGen)
	{
		//float time_in_seconds = (float) rangedFloat_streak_length->Value / 24.0f;
		float time_in_seconds = (float) rangedFloat_streak_length->Value;
		pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_StreakLength, time_in_seconds);
	}
}
System::Void ParticleDialog::rangedFloat_streak_taper_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if ( m_bDisableNotify ) return;

	prtSpriteGroupParticleGenerator* pPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(ptclLevel::GetParticleGenerator());
	if (pPGen)
		pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_StreakTaper, (float)rangedFloat_streak_taper->Value );
	 
}
System::Void ParticleDialog::rangedFloat_streak_fade_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if ( m_bDisableNotify ) return;

	prtSpriteGroupParticleGenerator* pPGen = dynamic_cast<prtSpriteGroupParticleGenerator*>(ptclLevel::GetParticleGenerator());
	if (pPGen)
		pPGen->SetParameter( prtSpriteGroupParticleGenerator::e_StreakFade, (float)rangedFloat_streak_fade->Value );	 
}

//----------------------------------------------------------------------------
//	update the particle generator alpha profile
//----------------------------------------------------------------------------
void ParticleDialog::Update_ParticleGenerator_AlphaProfile()
{
	float beginalpha, middlealphatimestart, middlealphatimeend, middlealpha, endalpha;
	beginalpha				= (float) rangedFloat_texture_alphalevel_start->Value;
	middlealpha				= (float) rangedFloat_texture_alphalevel_middle->Value;
	endalpha				= (float) rangedFloat_texture_alphalevel_end->Value;
	middlealphatimestart	= (float) rangedFloat_texture_alphalevel_middlepercentstart->Value;
	middlealphatimeend		= (float) rangedFloat_texture_alphalevel_middlepercentend->Value;

	ptclLevel::SetParticleGeneratorAlphaProfile(beginalpha, 
												middlealphatimestart, 
												middlealphatimeend, 
												middlealpha, 
												endalpha );

	prtParticleGeneratorTemplate * pPGTemplate = (ptclParticleTemplate::GetTemplate());
	if ( pPGTemplate )
		pPGTemplate->SetAlphaAnimation( *(ptclLevel::GetParticleGeneratorAlphaProfile()) );
}


//----------------------------------------------------------------------------
//	replace the particle generator
//----------------------------------------------------------------------------
void ParticleDialog::Replace_ParticleGenerator()
{
	prtParticleGeneratorTemplate * pPGTemplate = (ptclParticleTemplate::GetTemplate());
	if ( !pPGTemplate ) return;

	//bool bActive = ptclApp::IsActive();
	//ptclApp::SetActive(false);

	//
	//DBG_LOG0("Replace_ParticleGenerator Alpha Profile");
	//DBG_LOG1( "  1) %6.3f", pPGTemplate->GetAlphaAnimation().GetValue( 0.0f ) );
	//DBG_LOG1( "  2) %6.3f", pPGTemplate->GetAlphaAnimation().GetValue( 0.5f ) );
	//DBG_LOG1( "  3) %6.3f", pPGTemplate->GetAlphaAnimation().GetValue( 1.0f ) );

	prtParticleGeneratorTemplate::Type PGTType = pPGTemplate->GetType();

	Update_ParticleTemplate( pPGTemplate );

	if ( PGTType != pPGTemplate->GetType() )
	{
		prtGeneratorUtil::SetTemplateToDefault( *(pPGTemplate), true );
		
		ptclParticleTemplate::GetTemplate()->SetTextureLocator( pPGTemplate->GetTextureLocator() );

		ptclLevel::ReplaceGenerator();

		Update_Controls( pPGTemplate );
	}

	//ptclApp::SetActive(bActive);
}

//----------------------------------------------------------------------------
//	replace the particle generator no matter what.
//----------------------------------------------------------------------------
void ParticleDialog::ForceReplace_ParticleGenerator()
{
	prtParticleGeneratorTemplate * pPGTemplate = (ptclParticleTemplate::GetTemplate());
	if ( !pPGTemplate ) return;

	//	ReplaceGenerator *must* go before the Updatng of the controls
	//	because some controls grab information from the new generator
	//
	ptclLevel::ReplaceGenerator();

	Update_Controls( pPGTemplate );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_TabPage_Generator( const prtParticleGeneratorTemplate * i_pPGTemplate )
{
	//	Generator Type
	//
	prtParticleGeneratorTemplate::Type PGType = i_pPGTemplate->GetType();
	switch (PGType)
	{
		case prtParticleGeneratorTemplate::e_Static:
		{
			tabPage_particletype_cone->Enabled = false;
			tabPage_particletype_spiral->Enabled = false;
			tabPage_particlestarttexture->Enabled = true;

			radioButton_assettype_1texture->Checked = true;

			int index = checkedListBox_generatortype->Items->IndexOf("Static");
			checkedlistbox_keep_one_checked( checkedListBox_generatortype, index );

			this->tabControl_particledata->Controls->Remove( tabPage_particletype_cone );
			this->tabControl_particledata->Controls->Remove( tabPage_particletype_spiral );
			this->tabControl_particledata->SelectedTab = this->tabPage_generator;
			break;
		}
		case prtParticleGeneratorTemplate::e_Cone:
		{
			tabPage_particletype_cone->Enabled = true;
			tabPage_particletype_spiral->Enabled = false;
			tabPage_particlestarttexture->Enabled = true;

			radioButton_assettype_1texture->Checked = true;

			int index = checkedListBox_generatortype->Items->IndexOf("Cone");
			checkedlistbox_keep_one_checked( checkedListBox_generatortype, index );

//			this->tabControl_particledata->Controls->Add( tabPage_particletype_cone );
//			this->tabControl_particledata->Controls->Remove( tabPage_particletype_spiral );
//			this->tabControl_particledata->SelectedTab =( this->tabPage_generator );
			break;
		}
		case prtParticleGeneratorTemplate::e_Spiral:
		{
			tabPage_particletype_spiral->Enabled = true;
			tabPage_particletype_cone->Enabled = false;
			tabPage_particlestarttexture->Enabled = true;

			radioButton_assettype_1texture->Checked = true;

			int index = checkedListBox_generatortype->Items->IndexOf("Spiral");
			checkedlistbox_keep_one_checked( checkedListBox_generatortype, index );

//			this->tabControl_particledata->Controls->Remove( tabPage_particletype_cone );
//			this->tabControl_particledata->Controls->Add( tabPage_particletype_spiral );
//			this->tabControl_particledata->SelectedTab =( this->tabPage_generator );
			break;
		}
		case prtParticleGeneratorTemplate::e_Cone3D:
		{
			tabPage_particletype_cone->Enabled = false;
			tabPage_particletype_spiral->Enabled = false;
			tabPage_particlestarttexture->Enabled = false;
			
			radioButton_assettype_geometry->Checked = true;

			int index = checkedListBox_generatortype->Items->IndexOf("Cone");
			checkedlistbox_keep_one_checked( checkedListBox_generatortype, index );

//			this->tabControl_particledata->Controls->Add( tabPage_particletype_cone );
//			this->tabControl_particledata->Controls->Remove( tabPage_particletype_spiral );
//			this->tabControl_particledata->SelectedTab =( this->tabPage_generator );
			break;
		}
		default:
		{
			DBG_ASSERT0(false, "Invalid Generator Type" );
			break;
		}
	}

	//	Emitter Type
	prtParticleGeneratorTemplate::EmitterType EType = i_pPGTemplate->GetEmitterType();
	switch (EType)
	{
		case prtParticleGeneratorTemplate::e_Circle:
		{
			floatEdit_emitterscale->Enabled = true;
			const maVector3d& escale = i_pPGTemplate->GetEmitterScale();
			floatEdit_emitterscale->Value = escale.GetX();

			int index = checkedListBox_emittertype->Items->IndexOf("Circle");
			checkedlistbox_keep_one_checked( checkedListBox_emittertype, index );
			break;
		}
		case prtParticleGeneratorTemplate::e_Block:
		{
			floatEdit_emitterscale->Enabled = true;
			const maVector3d& escale = i_pPGTemplate->GetEmitterScale();
			floatEdit_emitterscale->Value = escale.GetX();

			int index = checkedListBox_emittertype->Items->IndexOf("Block");
			checkedlistbox_keep_one_checked( checkedListBox_emittertype, index );
			break;
		}
		case prtParticleGeneratorTemplate::e_Point:
		{
			floatEdit_emitterscale->Enabled = false;

			int index = checkedListBox_emittertype->Items->IndexOf("Point");
			checkedlistbox_keep_one_checked( checkedListBox_emittertype, index );
			break;
		}
		default:
		{
			DBG_ASSERT0(false, "Invalid Emitter Type" );
			break;
		}
	}
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_TabPage_Streaking( const prtParticleGeneratorTemplate * i_pPGTemplate )
{	
	//	Generator Type
	//
	prtParticleGeneratorTemplate::Type PGType = i_pPGTemplate->GetType();
	switch (PGType)
	{
		case prtParticleGeneratorTemplate::e_Static:
		case prtParticleGeneratorTemplate::e_Cone:
		case prtParticleGeneratorTemplate::e_Spiral:
		{
			if (!this->tabControl_particledata->Controls->Contains(tabPage_streaking))
				this->tabControl_particledata->Controls->Add( tabPage_streaking );
			this->tabControl_particledata->SelectedTab = this->tabPage_generator;

			checkBox_streaking->Checked = i_pPGTemplate->GetRenderStreaks();
			float time_in_seconds = i_pPGTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_StreakLength);
			rangedFloat_streak_length->Value = time_in_seconds; // or (time_in_seconds * 24.0f); ?
			rangedFloat_streak_taper->Value = i_pPGTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_StreakTaper);
			rangedFloat_streak_fade->Value = i_pPGTemplate->GetParameter(prtSpriteGroupParticleGenerator::e_StreakFade);

			break;
		}
		case prtParticleGeneratorTemplate::e_Cone3D:
		{
			this->tabControl_particledata->Controls->Remove( this->tabPage_streaking );
			this->tabControl_particledata->SelectedTab = this->tabPage_generator;
			break;
		}
		default:
		{
			DBG_ASSERT0(false, "Invalid Generator Type" );
			break;
		}
	}

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_TabPage_Generator_TabControl( const prtParticleGeneratorTemplate * i_pPGTemplate )
{
	//	Generator Type
	//
	prtParticleGeneratorTemplate::Type PGType = i_pPGTemplate->GetType();
	switch (PGType)
	{
		case prtParticleGeneratorTemplate::e_Static:
		{
			this->tabControl_particledata->Controls->Remove( tabPage_particletype_cone );
			this->tabControl_particledata->Controls->Remove( tabPage_particletype_spiral );
			this->tabControl_particledata->SelectedTab = this->tabPage_generator;
			break;
		}
		case prtParticleGeneratorTemplate::e_Cone:
		{
			if (!this->tabControl_particledata->Controls->Contains(tabPage_particletype_cone))
				this->tabControl_particledata->Controls->Add( tabPage_particletype_cone );
			this->tabControl_particledata->Controls->Remove( tabPage_particletype_spiral );
			this->tabControl_particledata->SelectedTab = this->tabPage_generator;
			break;
		}
		case prtParticleGeneratorTemplate::e_Spiral:
		{
			this->tabControl_particledata->Controls->Remove( tabPage_particletype_cone );
			if (!this->tabControl_particledata->Controls->Contains(tabPage_particletype_spiral))
				this->tabControl_particledata->Controls->Add( tabPage_particletype_spiral );
			this->tabControl_particledata->SelectedTab = this->tabPage_generator;
			break;
		}
		case prtParticleGeneratorTemplate::e_Cone3D:
		{
			if (!this->tabControl_particledata->Controls->Contains(tabPage_particletype_cone))
				this->tabControl_particledata->Controls->Add( tabPage_particletype_cone );
			this->tabControl_particledata->Controls->Remove( tabPage_particletype_spiral );
			this->tabControl_particledata->SelectedTab = this->tabPage_generator;
			break;
		}
		default:
		{
			DBG_ASSERT0(false, "Invalid Generator Type" );
			break;
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_TabPage_ParticleTexture( const prtParticleGeneratorTemplate * i_pPGTemplate )
{
	prtParticleGeneratorTemplate::Type PGType = i_pPGTemplate->GetType();

	if ( i_pPGTemplate->GetScaleMode() == prtSpriteGroupParticleGenerator::e_Linear )
	{
		radioButton_texture_scaling_linear->Checked = true;
	}
	else if ( i_pPGTemplate->GetScaleMode() == prtSpriteGroupParticleGenerator::e_Exponential )
	{
		radioButton_texture_scaling_exponential->Checked = true;
	}

	if ( i_pPGTemplate->GetRenderMode() == prtSpriteGroupParticleGenerator::e_Multiplicative )
	{
		radioButton_texture_alphablending_multiply->Checked = true;
	}
	else if ( i_pPGTemplate->GetRenderMode() == prtSpriteGroupParticleGenerator::e_Additive )
	{
		radioButton_texture_alphablending_add->Checked = true;
	}

	// update the path
	fileChooser_texture->Fullpath = tmaManagedStringUtils::LocatorToManagedString( i_pPGTemplate->GetTextureLocator() );

	// UVA related controls
	if ( i_pPGTemplate->GetUVAMode() == prtSpriteGroupParticleGenerator::e_TUV )
	{
		radioButton_texture_UVA_TUV->Checked = true;
	}
	else if ( i_pPGTemplate->GetUVAMode() == prtSpriteGroupParticleGenerator::e_RandomFrame )
	{
		radioButton_texture_UVA_random->Checked = true;
	}
	else if ( i_pPGTemplate->GetUVAMode() == prtSpriteGroupParticleGenerator::e_ScaleToLifetime )
	{
		radioButton_texture_UVA_Lifetime->Checked = true;
	}
	update_controls_texturepath_changed();

	// update alpha values
	float beginalpha, middlealphatimestart, middlealphatimeend, middlealpha, endalpha;
	ptclLevel::GetParticleGeneratorAlphaProfile( beginalpha, middlealphatimestart, middlealphatimeend, middlealpha, endalpha );

	rangedFloat_texture_alphalevel_start->Value = beginalpha;
	rangedFloat_texture_alphalevel_middle->Value = middlealpha;
	rangedFloat_texture_alphalevel_end->Value = endalpha;
	rangedFloat_texture_alphalevel_middlepercentstart->Value = middlealphatimestart;
	rangedFloat_texture_alphalevel_middlepercentend->Value = middlealphatimeend;

	// TODO - implement this when this fields are active
	//
//	floatEdit_texture_starttime
//	floatEdit_texture_endtime
//	numericUpDown_texture_frames
//	numericUpDown_texture_framerate
//	checkBox_texture_looping

	//std::string szFilename;
	//fsFileUtil::LocatorToANSIFilename( i_pPGTemplate->GetTextureLocator(), szFilename );
	//DBG_LOG1("texture set: %s", szFilename.c_str() );
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_TabPage_ParticleTypeGeneric( const prtParticleGeneratorTemplate * i_pPGTemplate )
{
	prtParticleGeneratorTemplate::Type PGType = i_pPGTemplate->GetType();

	switch (PGType)
	{
		case prtParticleGeneratorTemplate::e_Cone3D:
			fileChooser_geometry->Fullpath = tmaManagedStringUtils::LocatorToManagedString( i_pPGTemplate->GetGeometryLocator() );
		case prtParticleGeneratorTemplate::e_Cone:
		case prtParticleGeneratorTemplate::e_Spiral:
		case prtParticleGeneratorTemplate::e_Static:
		{
			floatEdit_particledata_generic_particlelifetime_min->Value = i_pPGTemplate->GetParameter( prtSpriteGroupParticleGenerator::e_MinParticleLifetime );
			floatEdit_particledata_generic_particlelifetime_max->Value = i_pPGTemplate->GetParameter( prtSpriteGroupParticleGenerator::e_MaxParticleLifetime );
			floatEdit_particledata_generic_particlerate->Value = i_pPGTemplate->GetParameter( prtSpriteGroupParticleGenerator::e_ParticleRate );
			floatEdit_particledata_generic_maxparticles->Value = i_pPGTemplate->GetParameter( prtSpriteGroupParticleGenerator::e_MaxParticles );
			floatEdit_particledata_generic_particlescale_start->Value = i_pPGTemplate->GetParameter( prtSpriteGroupParticleGenerator::e_InitialScale );
			floatEdit_particledata_generic_particlescalecoefficient->Value = i_pPGTemplate->GetParameter( prtSpriteGroupParticleGenerator::e_ScaleCoeff );
			floatEdit_particledata_generic_particlestartangle_min->Value = i_pPGTemplate->GetParameter( prtSpriteGroupParticleGenerator::e_MinStartAngle );
			floatEdit_particledata_generic_particlestartangle_max->Value = i_pPGTemplate->GetParameter( prtSpriteGroupParticleGenerator::e_MaxStartAngle );
			floatEdit_particledata_generic_particleangularvelocity_min->Value = i_pPGTemplate->GetParameter( prtSpriteGroupParticleGenerator::e_MinAngularVelocity );
			floatEdit_particledata_generic_particleangularvelocity_max->Value = i_pPGTemplate->GetParameter( prtSpriteGroupParticleGenerator::e_MaxAngularVelocity );
			floatEdit_particledata_generic_particleangularacceleration_min->Value = i_pPGTemplate->GetParameter( prtSpriteGroupParticleGenerator::e_MinAngularAcceleration );
			floatEdit_particledata_generic_particleangularacceleration_max->Value = i_pPGTemplate->GetParameter( prtSpriteGroupParticleGenerator::e_MaxAngularAcceleration );
			floatEdit_particledata_generic_presimtime->Value = i_pPGTemplate->GetParameter( prtSpriteGroupParticleGenerator::e_PreSimTime );
			break;
		}
		default:
		{
			DBG_ASSERT0(false, "Invalid Generator Type" );
			break;
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_TabPage_ParticleTypeCone( const prtParticleGeneratorTemplate * i_pPGTemplate )
{
	prtParticleGeneratorTemplate::Type PGType = i_pPGTemplate->GetType();

	switch (PGType)
	{
		case prtParticleGeneratorTemplate::e_Spiral:
		case prtParticleGeneratorTemplate::e_Static:
		case prtParticleGeneratorTemplate::e_Cone3D:
			break;
		case prtParticleGeneratorTemplate::e_Cone:
		{
			floatEdit_particledata_cone_coneangle->Value = (float)i_pPGTemplate->GetParameter( prtConeParticleGenerator::e_ConeAngle );
			floatEdit_particledata_cone_speed_min->Value = (float)i_pPGTemplate->GetParameter( prtConeParticleGenerator::e_MinSpeed );
			floatEdit_particledata_cone_speed_max->Value = (float)i_pPGTemplate->GetParameter( prtConeParticleGenerator::e_MaxSpeed );
			vector3Edit_particledata_cone_acceleration->ValueX = (float)i_pPGTemplate->GetParameter( prtConeParticleGenerator::e_AccelerationX );
			vector3Edit_particledata_cone_acceleration->ValueY = (float)i_pPGTemplate->GetParameter( prtConeParticleGenerator::e_AccelerationY );
			vector3Edit_particledata_cone_acceleration->ValueZ = (float)i_pPGTemplate->GetParameter( prtConeParticleGenerator::e_AccelerationZ );
			break;
		}
		default:
		{
			DBG_ASSERT0(false, "Invalid Generator Type" );
			break;
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_TabPage_ParticleTypeSpiral( const prtParticleGeneratorTemplate * i_pPGTemplate )
{
	prtParticleGeneratorTemplate::Type PGType = i_pPGTemplate->GetType();

	switch (PGType)
	{
		case prtParticleGeneratorTemplate::e_Static:
		case prtParticleGeneratorTemplate::e_Cone:
		case prtParticleGeneratorTemplate::e_Cone3D:
			break;
		case prtParticleGeneratorTemplate::e_Spiral:
		{
			floatEdit_particledata_spiral_speed_min->Value = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_MinEmitSpeed );
			floatEdit_particledata_spiral_speed_max->Value = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_MaxEmitSpeed );
			vector3Edit_particledata_spiral_emitdirection->ValueX = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_EmitDirectionX );
			vector3Edit_particledata_spiral_emitdirection->ValueY = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_EmitDirectionY );
			vector3Edit_particledata_spiral_emitdirection->ValueZ = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_EmitDirectionZ );
			vector3Edit_particledata_spiral_acceleration->ValueX = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_AccelerationX );
			vector3Edit_particledata_spiral_acceleration->ValueY = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_AccelerationY );
			vector3Edit_particledata_spiral_acceleration->ValueZ = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_AccelerationZ );
			floatEdit_particledata_spiral_rotationangle_min->Value = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_MinRotStartAngle );
			floatEdit_particledata_spiral_rotationangle_max->Value = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_MaxRotStartAngle );
			floatEdit_particledata_spiral_rotationangular_min->Value = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_MinRotAngularVel );
			floatEdit_particledata_spiral_rotationangular_max->Value = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_MaxRotAngularVel );
			floatEdit_particledata_spiral_rotationradius->Value = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_RotRadius );
			floatEdit_particledata_spiral_radiusscalerate->Value = (float)i_pPGTemplate->GetParameter( prtSpiralParticleGenerator::e_RotRadiusScaleRate );
			break;
		}
		default:
		{
			DBG_ASSERT0(false, "Invalid Generator Type" );
			break;
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_Controls( const prtParticleGeneratorTemplate * i_pPGTemplate )
{
	if ( !i_pPGTemplate ) return;

	m_bDisableNotify = true;

	Update_TabPage_Generator( i_pPGTemplate );
	Update_TabPage_Streaking( i_pPGTemplate );
	Update_TabPage_ParticleTypeGeneric( i_pPGTemplate );
	Update_TabPage_ParticleTypeCone( i_pPGTemplate );
	Update_TabPage_ParticleTypeSpiral( i_pPGTemplate );
	Update_TabPage_ParticleTexture( i_pPGTemplate );
	Update_TabPage_Generator_TabControl( i_pPGTemplate );

	m_bDisableNotify = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_ParticleTemplate_From_TabPage_Generator( prtParticleGeneratorTemplate * io_pPGTemplate )
{
	//	store current type so code can check if type changed
	prtParticleGeneratorTemplate::Type PGTType = io_pPGTemplate->GetType();

	//	Type
	if (checkedListBox_generatortype->GetItemChecked(checkedListBox_generatortype->Items->IndexOf("Static")))
	{
		io_pPGTemplate->SetType( prtParticleGeneratorTemplate::e_Static );
	}
	else if (checkedListBox_generatortype->GetItemChecked(checkedListBox_generatortype->Items->IndexOf("Cone")))
	{
		if ( this->radioButton_assettype_1texture->Checked )
		{
			io_pPGTemplate->SetType( prtParticleGeneratorTemplate::e_Cone );
		}
		else if ( this->radioButton_assettype_geometry->Checked )
		{
			io_pPGTemplate->SetType( prtParticleGeneratorTemplate::e_Cone3D );
		}
	}
	else if (checkedListBox_generatortype->GetItemChecked(checkedListBox_generatortype->Items->IndexOf("Spiral")))
	{
		io_pPGTemplate->SetType( prtParticleGeneratorTemplate::e_Spiral );
	}

	//
	if ( PGTType != io_pPGTemplate->GetType() )
		prtGeneratorUtil::ResizeTemplateParameters( *io_pPGTemplate );

	//	emitter shape
	if (checkedListBox_emittertype->GetItemChecked(checkedListBox_emittertype->Items->IndexOf("Circle")))
	{
		io_pPGTemplate->SetEmitterType( prtParticleGeneratorTemplate::e_Circle );
	}
	else if (checkedListBox_emittertype->GetItemChecked(checkedListBox_emittertype->Items->IndexOf("Block")))
	{
		io_pPGTemplate->SetEmitterType( prtParticleGeneratorTemplate::e_Block );
	}
	else if (checkedListBox_emittertype->GetItemChecked(checkedListBox_emittertype->Items->IndexOf("Point")))
	{
		io_pPGTemplate->SetEmitterType( prtParticleGeneratorTemplate::e_Point );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_ParticleTemplate_From_TabPage_Streaking( prtParticleGeneratorTemplate * io_pPGTemplate )
{
	prtParticleGeneratorTemplate::Type PGType = io_pPGTemplate->GetType();

	switch (PGType)
	{
		case prtParticleGeneratorTemplate::e_Cone3D:
			break;
		case prtParticleGeneratorTemplate::e_Cone:
		case prtParticleGeneratorTemplate::e_Spiral:
		case prtParticleGeneratorTemplate::e_Static:
		{
			io_pPGTemplate->SetRenderStreaks( checkBox_streaking->Checked );

			float time_in_seconds = (float)rangedFloat_streak_length->Value; // or (time_in_seconds / 24.0f); ?
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_StreakLength, time_in_seconds);
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_StreakTaper, (float)rangedFloat_streak_taper->Value );
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_StreakFade, (float)rangedFloat_streak_fade->Value );	 

			break;
		}
		default:
		{
			DBG_ASSERT0(false, "Invalid Generator Type" );
			break;
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_ParticleTemplate_From_TabPage_ParticleTexture( prtParticleGeneratorTemplate * io_pPGTemplate )
{
	prtParticleGeneratorTemplate::Type PGType = io_pPGTemplate->GetType();

	switch (PGType)
	{
		case prtParticleGeneratorTemplate::e_Cone3D:
			break;
		case prtParticleGeneratorTemplate::e_Cone:
		case prtParticleGeneratorTemplate::e_Spiral:
		case prtParticleGeneratorTemplate::e_Static:
		{
			if ( radioButton_texture_scaling_linear->Checked )
			{
				io_pPGTemplate->SetScaleMode( prtSpriteGroupParticleGenerator::e_Linear );
			}
			else if ( radioButton_texture_scaling_exponential->Checked )
			{
				io_pPGTemplate->SetScaleMode( prtSpriteGroupParticleGenerator::e_Exponential );
			}

			if ( radioButton_texture_alphablending_multiply->Checked )
			{
				io_pPGTemplate->SetRenderMode( prtSpriteGroupParticleGenerator::e_Multiplicative );
			}
			else if ( radioButton_texture_alphablending_add->Checked )
			{
				io_pPGTemplate->SetRenderMode( prtSpriteGroupParticleGenerator::e_Additive );
			}

			if ( radioButton_texture_UVA_TUV->Checked )
			{
				io_pPGTemplate->SetUVAMode( prtSpriteGroupParticleGenerator::e_TUV );
			}
			else if ( radioButton_texture_UVA_random->Checked )
			{
				io_pPGTemplate->SetUVAMode( prtSpriteGroupParticleGenerator::e_RandomFrame );
			}
			else if ( radioButton_texture_UVA_Lifetime->Checked )
			{
				io_pPGTemplate->SetUVAMode( prtSpriteGroupParticleGenerator::e_ScaleToLifetime );
			}

			float beginalpha, middlealphatimestart, middlealphatimeend, middlealpha, endalpha;
			beginalpha		= (float) rangedFloat_texture_alphalevel_start->Value;
			middlealpha		= (float) rangedFloat_texture_alphalevel_middle->Value;
			endalpha		= (float) rangedFloat_texture_alphalevel_end->Value;
			middlealphatimestart= (float) rangedFloat_texture_alphalevel_middlepercentstart->Value;
			middlealphatimeend	= (float) rangedFloat_texture_alphalevel_middlepercentend->Value;

			// TODO - implement this when this fields are active
			//
			//floatEdit_texture_starttime
			//floatEdit_texture_endtime
			//numericUpDown_texture_frames
			//numericUpDown_texture_framerate
			//checkBox_texture_looping

			ptclLevel::SetParticleGeneratorTemplateAlphaProfile(beginalpha, 
																middlealphatimestart, 
																middlealphatimeend, 
																middlealpha, 
																endalpha );
			fsLocator filename;
			tmaManagedStringUtils::ManagedStringToLocator( fileChooser_texture->Fullpath, filename );

			io_pPGTemplate->SetTextureLocator( filename );
			break;
		}
		default:
		{
			DBG_ASSERT0(false, "Invalid Generator Type" );
			break;
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_ParticleTemplate_From_TabPage_ParticleTypeGeneric( prtParticleGeneratorTemplate * io_pPGTemplate )
{
	prtParticleGeneratorTemplate::Type PGType = io_pPGTemplate->GetType();

	//	set parameters
	//
	switch (PGType)
	{
		case prtParticleGeneratorTemplate::e_Cone3D:
		{
			fsLocator filename;
			tmaManagedStringUtils::ManagedStringToLocator( fileChooser_geometry->Fullpath, filename );
			io_pPGTemplate->SetGeometryLocator( filename );
		}
		case prtParticleGeneratorTemplate::e_Cone:
		case prtParticleGeneratorTemplate::e_Spiral:
		case prtParticleGeneratorTemplate::e_Static:
		{
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_MinParticleLifetime, (float)floatEdit_particledata_generic_particlelifetime_min->Value );
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_MaxParticleLifetime, (float)floatEdit_particledata_generic_particlelifetime_max->Value );
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_ParticleRate, (float)floatEdit_particledata_generic_particlerate->Value );
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_MaxParticles, (float)floatEdit_particledata_generic_maxparticles->Value );
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_InitialScale, (float)floatEdit_particledata_generic_particlescale_start->Value );
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_ScaleCoeff, (float)floatEdit_particledata_generic_particlescalecoefficient->Value );
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_MinStartAngle, (float)floatEdit_particledata_generic_particlestartangle_min->Value );
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_MaxStartAngle, (float)floatEdit_particledata_generic_particlestartangle_max->Value );
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_MinAngularVelocity, (float)floatEdit_particledata_generic_particleangularvelocity_min->Value );
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_MaxAngularVelocity, (float)floatEdit_particledata_generic_particleangularvelocity_max->Value );
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_MinAngularAcceleration, (float)floatEdit_particledata_generic_particleangularacceleration_min->Value );
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_MaxAngularAcceleration, (float)floatEdit_particledata_generic_particleangularacceleration_max->Value );
			io_pPGTemplate->SetParameter( prtSpriteGroupParticleGenerator::e_PreSimTime, (float)floatEdit_particledata_generic_presimtime->Value );
			break;
		}
		default:
		{
			DBG_ASSERT0(false, "Invalid Generator Type" );
			break;
		}
	}

	prtParticleGeneratorTemplate::EmitterType EType = io_pPGTemplate->GetEmitterType();
	switch (EType)
	{
		case prtParticleGeneratorTemplate::e_Circle:
		case prtParticleGeneratorTemplate::e_Block:
		{
			maVector3d escale( (float)floatEdit_emitterscale->Value, (float)floatEdit_emitterscale->Value, (float)floatEdit_emitterscale->Value );
			io_pPGTemplate->SetEmitterScale( escale );
			break;
		}
		case prtParticleGeneratorTemplate::e_Point:
			break;
		default:
		{
			DBG_ASSERT0(false, "Invalid Emitter Type" );
			break;
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_ParticleTemplate_From_TabPage_ParticleTypeCone( prtParticleGeneratorTemplate * io_pPGTemplate )
{
	prtParticleGeneratorTemplate::Type PGType = io_pPGTemplate->GetType();

	switch (PGType)
	{
		case prtParticleGeneratorTemplate::e_Spiral:
		case prtParticleGeneratorTemplate::e_Static:
		case prtParticleGeneratorTemplate::e_Cone3D:
			break;
		case prtParticleGeneratorTemplate::e_Cone:
		{
			io_pPGTemplate->SetParameter( prtConeParticleGenerator::e_ConeAngle, (float)floatEdit_particledata_cone_coneangle->Value );
			io_pPGTemplate->SetParameter( prtConeParticleGenerator::e_MinSpeed, (float)floatEdit_particledata_cone_speed_min->Value );
			io_pPGTemplate->SetParameter( prtConeParticleGenerator::e_MaxSpeed, (float)floatEdit_particledata_cone_speed_max->Value );
			io_pPGTemplate->SetParameter( prtConeParticleGenerator::e_AccelerationX, (float)vector3Edit_particledata_cone_acceleration->ValueX );
			io_pPGTemplate->SetParameter( prtConeParticleGenerator::e_AccelerationY, (float)vector3Edit_particledata_cone_acceleration->ValueY );
			io_pPGTemplate->SetParameter( prtConeParticleGenerator::e_AccelerationZ, (float)vector3Edit_particledata_cone_acceleration->ValueZ );
			break;
		}
		default:
		{
			DBG_ASSERT0(false, "Invalid Generator Type" );
			break;
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_ParticleTemplate_From_TabPage_ParticleTypeSpiral( prtParticleGeneratorTemplate * io_pPGTemplate )
{
	prtParticleGeneratorTemplate::Type PGType = io_pPGTemplate->GetType();

	switch (PGType)
	{
		case prtParticleGeneratorTemplate::e_Static:
		case prtParticleGeneratorTemplate::e_Cone:
		case prtParticleGeneratorTemplate::e_Cone3D:
			break;
		case prtParticleGeneratorTemplate::e_Spiral:
		{
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_MinEmitSpeed, (float)floatEdit_particledata_spiral_speed_min->Value );
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_MaxEmitSpeed, (float)floatEdit_particledata_spiral_speed_max->Value );
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_EmitDirectionX, (float)vector3Edit_particledata_spiral_emitdirection->ValueX );
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_EmitDirectionY, (float)vector3Edit_particledata_spiral_emitdirection->ValueY );
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_EmitDirectionZ, (float)vector3Edit_particledata_spiral_emitdirection->ValueZ );
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_AccelerationX, (float)vector3Edit_particledata_spiral_acceleration->ValueX );
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_AccelerationY, (float)vector3Edit_particledata_spiral_acceleration->ValueY );
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_AccelerationZ, (float)vector3Edit_particledata_spiral_acceleration->ValueZ );
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_MinRotStartAngle, (float)floatEdit_particledata_spiral_rotationangle_min->Value );
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_MaxRotStartAngle, (float)floatEdit_particledata_spiral_rotationangle_max->Value );
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_MinRotAngularVel, (float)floatEdit_particledata_spiral_rotationangular_min->Value );
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_MaxRotAngularVel, (float)floatEdit_particledata_spiral_rotationangular_max->Value );
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_RotRadius, (float)floatEdit_particledata_spiral_rotationradius->Value );
			io_pPGTemplate->SetParameter( prtSpiralParticleGenerator::e_RotRadiusScaleRate, (float)floatEdit_particledata_spiral_radiusscalerate->Value );
			break;
		}
		default:
		{
			DBG_ASSERT0(false, "Invalid Generator Type" );
			break;
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::Update_ParticleTemplate( prtParticleGeneratorTemplate * io_pPGTemplate )
{
	if ( !io_pPGTemplate ) return;

	Update_ParticleTemplate_From_TabPage_Generator( io_pPGTemplate );
	Update_ParticleTemplate_From_TabPage_Streaking( io_pPGTemplate );
	Update_ParticleTemplate_From_TabPage_ParticleTexture( io_pPGTemplate );
	Update_ParticleTemplate_From_TabPage_ParticleTypeGeneric( io_pPGTemplate );
	Update_ParticleTemplate_From_TabPage_ParticleTypeCone( io_pPGTemplate );
	Update_ParticleTemplate_From_TabPage_ParticleTypeSpiral( io_pPGTemplate );
}

//struct list_data
//{
//	int		m_Value;
//	String* m_Text;
//};
//----------------------------------------------------------------------------
//	set-up the tab pages with default values for each type of particle type
//----------------------------------------------------------------------------
void ParticleDialog::Initialize_TabPages()
{
	checkedListBox_emittertype->Items->Add("Block",Forms::CheckState::Unchecked);
	checkedListBox_emittertype->Items->Add("Circle",Forms::CheckState::Unchecked);
	checkedListBox_emittertype->Items->Add("Point",Forms::CheckState::Unchecked);

	checkedListBox_generatortype->Items->Add("Cone",Forms::CheckState::Unchecked);
	checkedListBox_generatortype->Items->Add("Spiral",Forms::CheckState::Unchecked);
	checkedListBox_generatortype->Items->Add("Static",Forms::CheckState::Unchecked);

	//	get the default values for each type of particle type and load that into the 
	//	tab pages.
	//
	prtParticleGeneratorTemplate PGTemplate;
	PGTemplate.SetType( prtParticleGeneratorTemplate::e_Cone3D );
	prtGeneratorUtil::SetTemplateToDefault( PGTemplate, true );
	Update_ParticleTemplate_From_TabPage_ParticleTypeGeneric( &PGTemplate );

	PGTemplate.SetType( prtParticleGeneratorTemplate::e_Cone );
	prtGeneratorUtil::SetTemplateToDefault( PGTemplate, false );
	Update_ParticleTemplate_From_TabPage_ParticleTypeCone( &PGTemplate );

	PGTemplate.SetType( prtParticleGeneratorTemplate::e_Spiral );
	prtGeneratorUtil::SetTemplateToDefault( PGTemplate, false );
	Update_ParticleTemplate_From_TabPage_ParticleTypeSpiral( &PGTemplate );
}

//----------------------------------------------------------------------------
//	output the data (for debug only)
//----------------------------------------------------------------------------
void ParticleDialog::Output_Data()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::checkedlistbox_keep_one_checked(CheckedListBox^ i_pCheckedListBox, int i_Index)
{
	if (i_pCheckedListBox != nullptr)
	{
		for (int i=0; i < i_pCheckedListBox->Items->Count; ++i)
		{
			if (i_Index != i)
				i_pCheckedListBox->SetItemCheckState(i, CheckState::Unchecked);
			else
				i_pCheckedListBox->SetItemCheckState(i, CheckState::Checked);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ParticleDialog::update_controls_texturepath_changed()
{
	fsLocator filepath;
	tmaManagedStringUtils::ManagedStringToLocator( fileChooser_texture->Fullpath, filepath );
	itString filename = filepath.GetLastName();
	if (filename.HasSubString(itString(".tuv")))
	{
		radioButton_texture_UVA_TUV->Enabled		= true;
		radioButton_texture_UVA_random->Enabled		= true;
		radioButton_texture_UVA_Lifetime->Enabled	= true;
	}
	else
	{
		radioButton_texture_UVA_TUV->Enabled		= false;
		radioButton_texture_UVA_random->Enabled		= false;
		radioButton_texture_UVA_Lifetime->Enabled	= false;
	}
}