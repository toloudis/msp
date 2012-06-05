/********************************************************************************************\
**  mtrlFur.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlFur.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyFolderChooserUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Graphics/Eff/effFurData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Tool/gui/guiMessageBox.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlFur::mtrlFur(mdlMaterialInfo& i_Data, effFurData* i_pShaderData)
:	m_Material(i_Data), 
	m_pShaderData(i_pShaderData)
{
	DBG_ASSERT0(m_pShaderData != NULL, "mtrlFur expected effFurData");

	m_Data.m_bAnisotropic.SetValue(m_pShaderData->m_bAnisotropic);
	m_Data.m_bColorSourcing.SetValue(m_pShaderData->m_bColorSourcing);
	m_Data.m_bFurThinning.SetValue(m_pShaderData->m_bFurThinning);
	m_Data.m_bShowFins.SetValue(m_pShaderData->m_bShowFins);
	m_Data.m_FinFader.SetValue(m_pShaderData->m_FinFader);
	m_Data.m_LengthScale.SetValue(m_pShaderData->m_LengthScale);
	m_Data.m_NumShells.SetValue(m_pShaderData->m_NumShells);
	m_Data.m_ShellFader.SetValue(m_pShaderData->m_ShellFader);
	m_Data.m_SpreadScale.SetValue(m_pShaderData->m_SpreadScale);
	fsLocator texFolder;
	texFolder.Push(m_pShaderData->m_TextureFolder.c_str());
	m_Data.m_TextureFolder.SetValue(texFolder);

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_NumShells);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_LengthScale);
	//this->AddVectorChannel(m_Material.GetMaterialName(), m_Data.m_SpreadScale);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_ShellFader);
	this->AddBooleanChannel(m_Material.GetMaterialName(), m_Data.m_bShowFins);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_FinFader);
	this->AddBooleanChannel(m_Material.GetMaterialName(), m_Data.m_bColorSourcing);
	this->AddBooleanChannel(m_Material.GetMaterialName(), m_Data.m_bFurThinning);
	this->AddBooleanChannel(m_Material.GetMaterialName(), m_Data.m_bAnisotropic);

	RegisterData(); 
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlFur::RegisterData()
{
	//
	prtyPropertyUIInfo* pPUII;
	pPUII  = new prtyFolderChooserUIInfo(&(m_Data.m_TextureFolder), "Fur", "Textures Folder");
	AddProperty( pPUII );

	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_LengthScale), "Fur", "Length Scaling");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(20.0f);
	AddProperty( pRFUII );

	prtyVector3dEditUpDownUIInfo* pV3UII;
	pV3UII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_SpreadScale), "Fur", "Spread scaling");
	pV3UII->SetMaximum(80,80,80);
	pV3UII->SetMinimum(0,0,0);
	AddProperty( pV3UII );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bColorSourcing), "Fur", "Use Color Sourcing") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bFurThinning), "Fur", "Fur Thinning") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bAnisotropic), "Fur", "Anisotropic lighting") );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_NumShells), "Fur", "Num Shells");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(100.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_ShellFader), "Fur", "Shell fade");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bShowFins), "Fur", "Render fins") );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_FinFader), "Fur", "Fin fade");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );

	//
	m_Data.m_TextureFolder.AddCallback(new prtyCallbackWrapper<mtrlFur>(this, &mtrlFur::UpdateTextureFolder));
	m_Data.m_NumShells.AddCallback(new prtyCallbackWrapper<mtrlFur>(this, &mtrlFur::Update));
	m_Data.m_LengthScale.AddCallback(new prtyCallbackWrapper<mtrlFur>(this, &mtrlFur::Update));
	m_Data.m_SpreadScale.AddCallback(new prtyCallbackWrapper<mtrlFur>(this, &mtrlFur::Update));
	m_Data.m_ShellFader.AddCallback(new prtyCallbackWrapper<mtrlFur>(this, &mtrlFur::Update));
	m_Data.m_bShowFins.AddCallback(new prtyCallbackWrapper<mtrlFur>(this, &mtrlFur::Update));
	m_Data.m_FinFader.AddCallback(new prtyCallbackWrapper<mtrlFur>(this, &mtrlFur::Update));
	m_Data.m_bColorSourcing.AddCallback(new prtyCallbackWrapper<mtrlFur>(this, &mtrlFur::Update));
	m_Data.m_bFurThinning.AddCallback(new prtyCallbackWrapper<mtrlFur>(this, &mtrlFur::Update));
	m_Data.m_bAnisotropic.AddCallback(new prtyCallbackWrapper<mtrlFur>(this, &mtrlFur::Update));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlFur::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_bDirty)
	{
		effFurData& fur_data = m_Material.FurParams();
		set_shader_data(fur_data);	// set into material template
	}
	set_shader_data(*m_pShaderData);	// set into material's shader data directly
}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlFur::set_shader_data(effFurData& i_Data)
{
	i_Data.m_bAnisotropic = m_Data.m_bAnisotropic.GetValue();
	i_Data.m_bColorSourcing = m_Data.m_bColorSourcing.GetValue();
	i_Data.m_bFurThinning = m_Data.m_bFurThinning.GetValue();
	i_Data.m_bShowFins = m_Data.m_bShowFins.GetValue();
	i_Data.m_FinFader = m_Data.m_FinFader.GetValue();
	i_Data.m_LengthScale = m_Data.m_LengthScale.GetValue();
	i_Data.m_NumShells = m_Data.m_NumShells.GetValue();
	i_Data.m_ShellFader = m_Data.m_ShellFader.GetValue();
	i_Data.m_SpreadScale = m_Data.m_SpreadScale.GetValue();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlFur::UpdateTextureFolder(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Make a copy of the material in order to do the change material data call below
	mdlMaterialInfo new_material( m_Material );

	std::string val;
	fsFileUtil::LocatorToANSIFilename(m_Data.m_TextureFolder.GetValue(), val);

	// make sure the path exists
	fsLocator locPath;
	locPath.Push(val.c_str());
	bool ok = fsFileUtil::DirectoryExists(locPath);
	// special behavior. this is assumed to be a TEXTURE PATH.
	// strip all but the last part of the path, and make sure it's in a valid place
	size_t extnPos = val.find_last_of('\\');
	std::string subval = val.substr(extnPos + 1);
	if (ok)
	{
		if (i_bDirty)
		{
			effFurData& fur_data = m_Material.FurParams();
			fur_data.m_TextureFolder = subval;
		}
		m_pShaderData->m_TextureFolder = subval;	// set into material's shader data directly
		
		mtrlOperations::UpdateFurTextures();
	}
	else
	{
		guiMessageBox::Show("Please press Browse to pick an existing folder", "Fur Textures", guiMessageBox::e_OKOnly);
	}
}


