/********************************************************************************************\
**  mtrlSurfaceShader.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlSurfaceShader.hpp"

#include "Support/brsh/brshPaintBrushMgr.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/rmp/rmpDialogMgr.hpp"
#include "Support/rmp/rmpTextureMgr.hpp"
#include "Support/rmp/rmpObject.hpp"

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
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/gpx/gpxShaderParams.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#include <string>
#include <boost/bind.hpp>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlSurfaceShader::mtrlSurfaceShader(mdlMaterialInfo& i_MatData, 
							 matMaterial* i_pMaterial,
							 shared_ptr<effShaderParams> i_pShaderData,
							 const fsLocator& i_TextureDir)
:	mtrlShaderObject(i_TextureDir),
	m_Material(i_MatData),
	m_pActualMaterial(i_pMaterial),
	m_pShaderData(i_pShaderData),
	m_pRampParam(NULL)
{
	// Create proxy object, which will create a new set of shader params to
	// use in the UI th read.
	m_pShaderDataProxy.reset(new gpxShaderParams(*m_pShaderData));

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).
	std::vector<effParamColor*> vColors;
	m_pShaderDataProxy->GetUIShaderParams().GetAllColorParams(vColors);
	int n = vColors.size();
	for (int i = 0; i < n; i++)
	{
		AddColorChannel(m_Material.GetMaterialName(), vColors[i]->Property());
	}
	std::vector<effParamTexture*> vTextures;
	m_pShaderDataProxy->GetUIShaderParams().GetAllTextureParams(vTextures);
	n = vTextures.size();
	for (int i = 0; i < n; i++)
	{
		AddTextureChannel(m_Material.GetMaterialName(), vTextures[i]->Property());
		//initialize any ramp data 
		if( vTextures[i]->GetRampProperty() != NULL )
		{
			m_pRampParam = vTextures[i];
			matTexture* pOldTexture = m_pRampParam->GetTexture();
			UpdateRampTexture(false);

			//Remove the old texture if the param's texture has been changed
			if(pOldTexture != m_pRampParam->GetTexture())
				matTextureMgr::ReleaseTexture(pOldTexture);
			m_pRampParam = NULL;
			m_pShaderDataProxy->Update();  //update the proxy for this material
		}
	}
	std::vector<effParamFloat*> vFloats;
	m_pShaderDataProxy->GetUIShaderParams().GetAllFloatParams(vFloats);
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
	n = m_pShaderDataProxy->GetUIShaderParams().m_TextureParamUIs.size();
	for (int i = 0; i < n; i++)
	{
		effShaderParams::effTextureUI textureUIData = m_pShaderDataProxy->GetUIShaderParams().m_TextureParamUIs[i];

//		itString name = textureUIData.m_pParam->GetProperty().GetValue();
//		if (name.GetLength() > 0)
//		{
//			fsLocator loc = LocateTexture(name);
//			loc.Pop();
//			// now loc should have the folder I want.
//			textureUIData.m_pUIInfo->SetInitialDirectory( loc );
//		}
//		else
		//bga - no more attempt to locate textures per material, just using
		// a single "textures" directory category, which has been already set up.
		//{
		//	if (i_MatData.UsesMaterialLibrary())
		//	{
		//		fsLocator loc = gfPaths::GetPath(gfPaths::e_MaterialLibrary);
		//		loc.Push(i_MatData.GetLibraryFilename());
		//		loc.Pop();
		//		textureUIData.m_pUIInfo->SetInitialDirectory( loc );
		//	}
		//	else
		//	{
		//		textureUIData.m_pUIInfo->SetInitialDirectory( i_TextureDir );
		//	}
		//}
	}

	// now add all the controls.
	DBG_ASSERT(m_pShaderDataProxy->GetUIShaderParams().m_pPrtyUI != NULL, "Shader UI bindings not yet created!");
	this->GetListContainer().Add(m_pShaderDataProxy->GetUIShaderParams().m_pPrtyUI->GetList());

	// register any data callbacks here? textures perhaps?
	n = vTextures.size();
	for (int i = 0; i < n; i++)
	{
		vTextures[i]->Property().AddCallback(new prtyCallbackWrapper<mtrlSurfaceShader>(this, &mtrlSurfaceShader::UpdateTexturePrty));
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//virtual 
mtrlSurfaceShader::~mtrlSurfaceShader()
{
	m_pRampParam = NULL;
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
	int n = m_pShaderDataProxy->GetUIShaderParams().m_TextureParamUIs.size();
	for (int i = 0; i < n; i++)
	{
		m_pShaderDataProxy->GetUIShaderParams().m_TextureParamUIs[i].m_pUIInfo->SetInitialDirectory( i_TextureDir );
		m_pShaderDataProxy->GetUIShaderParams().m_TextureParamUIs[i].m_pUIInfo->UpdateControl();
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
	m_pShaderDataProxy->GetUIShaderParams().GetAllTextureParams(vTextures);
	int n = vTextures.size();
	for (int i = 0; i < n; i++)
	{
		if (vTextures[i]->GetProperty().GetValue().GetNumNames() > 1)
			o_TextureList.push_back(vTextures[i]->GetProperty().GetValue());
		else if (vTextures[i]->GetProperty().GetValue().GetNumNames() == 1)
			o_TextureList.push_back(LocateTexture(vTextures[i]->GetProperty().GetValue().GetLastName()));
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSurfaceShader::UpdateTexturePrty(prtyProperty *i_pProperty, bool i_bDirty)
{
	// find the right shader param for the prty.
	effParamTexture* pParam = NULL;

	prtyTextureFileChooserUIInfo* pTextureProperty = dynamic_cast<prtyTextureFileChooserUIInfo*>(i_pProperty);

	std::vector<effParamTexture*> vTextures;
	m_pShaderDataProxy->GetUIShaderParams().GetAllTextureParams(vTextures);
	int n = vTextures.size();
	for (int i = 0; i < n; i++)
	{
		if (vTextures[i]->GetBaseProperty() == i_pProperty)
		{
			pParam = vTextures[i];
			break;
		}
	}
	bool clear_mgr_data = !pParam->IsNameCurrent() || i_bDirty;
	std::string callback( pParam->Property().GetFullValue().m_CurrentCallback );

	//if the button on one of the texture options was clicked we need to pass
	//the necessary info to the manager of that operation
	if( pParam->GetProperty().GetFullValue().m_bButtonPressed )
	{
		//reset our texture locator
		prtyTextureFileData val;
		val.m_TextureLocator = pParam->GetProperty().GetValue();
		//decide which callback was executed
		if( callback == pTextureProperty->GetTextureType(pTextureProperty->e_Paint) )
		{
			//do paint manager operations
			brshPaintBrushMgr::SetMatTexture(pParam);
			pParam->ClearRampTexture();
		}
		else if( callback == pTextureProperty->GetTextureType(pTextureProperty->e_Ramp) )
		{
			//set up the data for the ramp managers
			if(pParam->GetRampProperty() == NULL && pParam->GetProperty().GetFullValue().m_RampObject.get() == NULL)
			{
				std::auto_ptr<rmpObject> newRamp(new rmpObject);
				pParam->SetRampProperty((std::auto_ptr<prtyObject>)newRamp);
			}
			else if(pParam->GetProperty().GetFullValue().m_RampObject.get() != NULL)
			{
				pParam->SetRampProperty(pParam->GetProperty().GetFullValue().m_RampObject);
			}

			if(m_pRampParam)
				m_pRampParam = NULL;

			m_pRampParam = pParam;
			matTexture* pOldTexture = m_pRampParam->GetTexture();
			NotifyRampUI();
			UpdateRampTexture(i_bDirty);

			//Remove the old texture if the param's texture has been changed
			if(pOldTexture != m_pRampParam->GetTexture())
				matTextureMgr::ReleaseTexture(pOldTexture);
		}
		else if( callback == pTextureProperty->GetTextureType(pTextureProperty->e_Reset) )
		{
			
			callback = pTextureProperty->GetTextureType(pTextureProperty->e_Texture);
			//we need to clean up any ramp data if the reset button is pressed
			if( pParam->GetRampProperty() != NULL )
			{
				pParam->ClearTexture();
				pParam->ClearRampProperty();
			}
			//clean up any other data
			if(clear_mgr_data)
			{
				brshPaintBrushMgr::ClearPaintData(true);
				m_pRampParam = NULL;
			}
			UpdateTexture(pParam, i_bDirty);
			pParam->ClearRampTexture();
		}

		val.m_CurrentCallback = callback;
		pParam->Property().SetValueWithoutNotify(val);
	}
	else
	{
		UpdateTexture(pParam, i_bDirty);
		pParam->ClearRampTexture();

		if(clear_mgr_data)
		{
			brshPaintBrushMgr::ClearPaintData(true);
			m_pRampParam = NULL;
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrlSurfaceShader::NotifyRampUI()
{
	if(m_pRampParam == NULL)
		return;

	rmpObject* pRampObject = dynamic_cast<rmpObject*>(m_pRampParam->GetRampProperty());
	if(pRampObject)
	{
		rmpDialogMgr::SetRampChangedCallback(NULL);
		rmpDialogMgr::SetData(pRampObject->m_Data);
		rmpDialogMgr::SetRampChangedCallback(std::bind1st(std::mem_fun(&mtrlSurfaceShader::RampChangedFromUI), this));
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrlSurfaceShader::RampChangedFromUI(bool i_bDirty)
{
	if(m_pRampParam == NULL)
		return;

	rmpObject* pRampObject = dynamic_cast<rmpObject*>(m_pRampParam->GetRampProperty());
	if(pRampObject)
	{
		pRampObject->m_Data = rmpDialogMgr::Data();
		UpdateRampTexture(i_bDirty);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlSurfaceShader::UpdateRampTexture(bool i_bDirty)
{
	if(m_pRampParam == NULL)
		return;

	matTexture* pOldTexture = m_pRampParam->GetTexture();

	//first set the ramp data
	rmpObject* pRampObject = dynamic_cast<rmpObject*>(m_pRampParam->GetRampProperty());
	if(pRampObject)
	{
		//now verify the ramp texture
		ConfirmRampTexture(pRampObject->m_Data.m_TexSize.GetValue());
		matTexture* pRampTexture = m_pRampParam->GetRampTexture();

		//perform the ramp operations
		rmpTextureMgr::EnableRampTexture(pRampObject->m_Data, pRampTexture);
		bool success = false;
		if (pRampTexture)
		{
			//Set the updated ramp texture
			m_pRampParam->SetTexture(pRampTexture);
			success = true;
		}
		if(success && i_bDirty)
		{
			//Replace the previous texture in our model instance with the new one
			ReplaceTextureInTemplate(pOldTexture,  m_pRampParam->GetTexture());
		}
	}
}

////--------------------------------------------------------------------
////--------------------------------------------------------------------
void mtrlSurfaceShader::ConfirmRampTexture(int i_texSize)
{
	if(m_pRampParam == NULL)
		return;

	gpxRenderControl::ConfirmSingleThread();

	matTexture* pRampTexture = m_pRampParam->GetRampTexture();
	// check if texture size change
	if (!pRampTexture||  pRampTexture->GetWidth() != i_texSize)
	{
		if (m_pRampParam->GetTexture() == pRampTexture)
			m_pRampParam->SetTexture(NULL);

		rmpTextureMgr::ReleaseRampTexture(pRampTexture);
		m_pRampParam->SetRampTexture(NULL);
		pRampTexture = NULL;

		try
		{
			pRampTexture = rmpTextureMgr::CreateRampTexture(i_texSize);
			m_pRampParam->SetRampTexture(pRampTexture);
			pRampTexture = NULL;
		}
		catch (const g2dOutOfVideoMemoryX& )
		{
			guiMessageBox::Show("Not enough video mem to create ramp texture. Disable ramp texture.", "Error", guiMessageBox::e_OKOnly);
			pRampTexture = NULL;
		}
	}
}

shared_ptr<gpxShaderParams> mtrlSurfaceShader::GetShaderData()
{
	
	return m_pShaderDataProxy;

}

