/********************************************************************************************\
**  mtrlSurfaceShader.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlSurfaceShader.hpp"

#include "Support/mtrl/GUI/mtrlOperations.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"

#include "Graphics/mat/matTextureMgr.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlSurfaceShader::mtrlSurfaceShader(mdlMaterialInfo& i_MatData, 
							 matMaterial* i_pMaterial,
							 shared_ptr<effShaderParams> i_pShaderData,
							 const fsLocator& i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_MatData),
	m_pActualMaterial(i_pMaterial),
	m_pShaderData(i_pShaderData)
{

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).
	std::vector<effParamColor*> vColors;
	m_pShaderData->GetAllColorParams(vColors);
	int n = vColors.size();
	for (int i = 0; i < n; i++)
	{
		AddColorChannel(m_Material.GetMaterialName(), vColors[i]->Property());
	}
	std::vector<effParamTexture*> vTextures;
	m_pShaderData->GetAllTextureParams(vTextures);
	n = vTextures.size();
	for (int i = 0; i < n; i++)
	{
		AddTextureChannel(m_Material.GetMaterialName(), vTextures[i]->Property());
	}
	std::vector<effParamFloat*> vFloats;
	m_pShaderData->GetAllFloatParams(vFloats);
	n = vFloats.size();
	for (int i = 0; i < n; i++)
	{
		AddFloatChannel(m_Material.GetMaterialName(), vFloats[i]->Property());
	}

	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	// If the m_pPrtyUI is not built, now is the time to build it!!!!!!!
	// In fact, perhaps it ought to be built at this time, and no sooner.
	// Can make a call to the shader, passing the effShaderParams object.
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	// this initializes the file picker controls with the right folder.
	// texture folder UI controls need to update here:
	n = m_pShaderData->m_TextureParamUIs.size();
	for (int i = 0; i < n; i++)
	{
		effShaderParams::effTextureUI textureUIData = m_pShaderData->m_TextureParamUIs[i];
//		itString name = textureUIData.m_pParam->GetProperty().GetValue();
//		if (name.GetLength() > 0)
//		{
//			fsLocator loc = LocateTexture(name);
//			loc.Pop();
//			// now loc should have the folder I want.
//			textureUIData.m_pUIInfo->SetInitialDirectory( loc );
//		}
//		else
		{
			if (i_MatData.UsesMaterialLibrary())
			{
				fsLocator loc = gfPaths::GetPath(gfPaths::e_MaterialLibrary);
				loc.Push(i_MatData.GetLibraryFilename());
				loc.Pop();
				textureUIData.m_pUIInfo->SetInitialDirectory( loc );
			}
			else
			{
				textureUIData.m_pUIInfo->SetInitialDirectory( i_TextureDir );
			}
		}
	}

	// now add all the controls.
	DBG_ASSERT0(m_pShaderData->m_pPrtyUI != NULL, "Shader UI bindings not yet created!");
	this->GetListContainer().Add(m_pShaderData->m_pPrtyUI->GetList());

	// register any data callbacks here? textures perhaps?
	n = vTextures.size();
	for (int i = 0; i < n; i++)
	{
		vTextures[i]->Property().AddCallback(new prtyCallbackWrapper<mtrlSurfaceShader>(this, &mtrlSurfaceShader::UpdateTexturePrty));
	}
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
//virtual 
void mtrlSurfaceShader::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	mtrlShaderObject::UpdateTextureDirectory(i_TextureDir);

	// texture folder UI controls need to update here:
	int n = m_pShaderData->m_TextureParamUIs.size();
	for (int i = 0; i < n; i++)
	{
		m_pShaderData->m_TextureParamUIs[i].m_pUIInfo->SetInitialDirectory( i_TextureDir );
		m_pShaderData->m_TextureParamUIs[i].m_pUIInfo->UpdateControl();
	}
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
//virtual 
void mtrlSurfaceShader::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
	std::vector<effParamTexture*> vTextures;
	m_pShaderData->GetAllTextureParams(vTextures);
	int n = vTextures.size();
	for (int i = 0; i < n; i++)
	{
		if (vTextures[i]->GetProperty().GetValue().GetLength() > 0)
			o_TextureList.push_back(LocateTexture(vTextures[i]->GetProperty().GetValue()));
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSurfaceShader::UpdateTexturePrty(prtyProperty *i_pProperty, bool i_bDirty)
{
	// find the right shader param for the prty.
	effParamTexture* pParam = NULL;

	std::vector<effParamTexture*> vTextures;
	m_pShaderData->GetAllTextureParams(vTextures);
	int n = vTextures.size();
	for (int i = 0; i < n; i++)
	{
		if (vTextures[i]->GetBaseProperty() == i_pProperty)
		{
			pParam = vTextures[i];
			break;
		}
	}

	UpdateTexture(pParam, i_bDirty);

}
