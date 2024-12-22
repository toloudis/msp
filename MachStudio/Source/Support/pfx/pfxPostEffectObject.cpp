/********************************************************************************************\
**  pfxPostEffectObject.cpp
**
**		See .hpp for details
**
**  StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#include "Support/pfx/pfxPostEffectObject.hpp"

#include "Core/fs/fsResourceFinderDir.hpp"
#include "Core/prty/prtyButtonUIInfo.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Features/RenderLayers/rdrLayersDialogUtil.hpp"
#include "Features/RenderPrefs/rndrPrefsDialogUtil.hpp"
#include "Graphics/eff/effShaderParams.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "Graphics/g3d/g3dPostProcessing.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mtr/mtrTextureFinder.hpp"
#include "Support/brsh/brshPaintBrushMgr.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Support/rmp/rmpDialogMgr.hpp"
#include "Support/rmp/rmpTextureMgr.hpp"
#include "Support/rmp/rmpObject.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gpx/gpxShaderParams.hpp"
#include "Tool/gui/guiMessageBox.hpp"

namespace
{
	prtyFileChooserUIInfo* create_file_chooser(shared_ptr<effShaderParams> i_ShaderParams,
											   prtyFilePath& io_ShaderType)
	{
		fsLocator currentShaderName;
		if (i_ShaderParams.get())
			currentShaderName = i_ShaderParams->GetShaderName();

		prtyFileChooserUIInfo *pShaderFilePickerUI = new prtyFileChooserUIInfo(&io_ShaderType, "Shader", "Location of shader");
		pShaderFilePickerUI->SetConfirmationString("Changing shader may lose all old shader parameters. Continue?");
		pShaderFilePickerUI->SetDirectoryCategory("Shaders");
		pShaderFilePickerUI->SetInitialDirectory( matShaderMgr::GetDefaultShaderPath() );
		return pShaderFilePickerUI;
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//pfxPostEffectObject::pfxPostEffectObject()
//{
//}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
pfxPostEffectObject::pfxPostEffectObject(const std::string& i_ShaderName, 
										 const std::string& i_ShaderPath,
										 pfxData::UIType i_Type):
	 m_pMatchParams(NULL), m_pRampParam(NULL)
{
	Init();
	m_Data.m_Name.SetValue(fsLocator(itString(i_ShaderPath.c_str())));
	m_UIType.SetValue(i_Type);
	//m_pShaderDataProxy.reset(new gpxShaderParams(*m_ShaderParams));

	// must create ui after created gpxShaderParams object
	//RebuildForm();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
pfxPostEffectObject::pfxPostEffectObject(const pfxData& i_Data, pfxData::UIType i_Type):
	m_pMatchParams(NULL), m_pRampParam(NULL)
{
	Init();
	SetData(i_Data);
	m_UIType.SetValue(i_Type);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
pfxPostEffectObject::~pfxPostEffectObject()
{
	// post effect is responsible for releasing textures
	DeleteTextures(m_Data.m_pShaderParams.get());
	m_pRampParam = NULL;

}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shared_ptr<prtyPropertyUIInfoContainer> pfxPostEffectObject::GetBaseContainer()
{
	return m_BaseUI;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
shared_ptr<prtyPropertyUIInfoContainer> pfxPostEffectObject::GetShaderContainer()
{
	return m_ShaderUI;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void pfxPostEffectObject::ApplyPostEffect()
{
	g3dPostProcessing::SetActive(m_Data.m_bActive.GetValue());
	g3dPostProcessing::SetPostEffect(m_Data.m_pShaderParams);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool pfxPostEffectObject::ForceLoadTexture(const fsLocator& i_Path, int i_Index)
{
	if (!m_Data.m_pShaderParams.get())
		return false;


	std::vector<effParamTexture*> vTextures;
	m_Data.m_pShaderParams->GetAllTextureParams(vTextures);
	int n = vTextures.size();

	if (i_Index >= 0 && i_Index < n)
	{
		vTextures[i_Index]->Property().SetValue(prtyTextureFileData(i_Path));
		return true;
	}

	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void pfxPostEffectObject::Init()
{
	m_BaseUI.reset(new prtyPropertyUIInfoContainer);
	m_ShaderUI.reset(new prtyPropertyUIInfoContainer);

	prtyCheckBoxUIInfo* pPUII = NULL;

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bActive), "Post Effect", "Active");
	m_BaseUI->Add( pPUII );

	prtyFileChooserUIInfo* pShaderFilePickerUI = create_file_chooser(m_Data.m_pShaderParams, m_Data.m_Name);
	m_BaseUI->Add( pShaderFilePickerUI );

	m_Data.m_bActive.AddCallback(new prtyCallbackWrapper<pfxPostEffectObject>(this, &pfxPostEffectObject::Update));
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<pfxPostEffectObject>(this, &pfxPostEffectObject::UpdateShader));

	//this->GetListContainer().Add(m_BaseUI->GetPropertyUIInfoList());
	this->GetListContainer().AddSubCategory("Base", m_BaseUI);
	//this->GetListContainer().Add(m_BaseUI->GetList());
	//this->GetListContainer().Add(m_ShaderUI->GetList());
	this->GetListContainer().AddSubCategory("Shader Properties", m_ShaderUI);

	this->GetListContainer().SetShowSubCategory(false);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void pfxPostEffectObject::RemoveProperties(shared_ptr<prtyPropertyUIInfoContainer> i_SubCategory,
						   prtyObject* i_pObject)
{
	i_SubCategory->DeleteAll();
	/*if (i_pObject)
	{
		PropertyUIIList::const_iterator it, end = i_pObject->GetList().end();
		for (it = i_pObject->GetList().begin(); it != end; ++it)
		{
			shared_ptr<prtyPropertyUIInfo> ui_Info(*it);
			i_SubCategory->Remove(ui_Info);
		}
	}*/
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void pfxPostEffectObject::AddPropertiesCallback(shared_ptr<effShaderParams> i_pParam)
{
	std::vector<effParamColor*> vColors;
	i_pParam->GetAllColorParams(vColors);
	int n = vColors.size();
	for (int i = 0; i < n; i++)
	{
		vColors[i]->Property().AddCallback(new prtyCallbackWrapper<pfxPostEffectObject>(this, &pfxPostEffectObject::UpdateProperties));
	}

	std::vector<effParamTexture*> vTextures;
	i_pParam->GetAllTextureParams(vTextures);
	n = vTextures.size();
	for (int i = 0; i < n; i++)
	{
		vTextures[i]->Property().AddCallback(new prtyCallbackWrapper<pfxPostEffectObject>(this, &pfxPostEffectObject::UpdateTexturePrty));
		vTextures[i]->Property().AddCallback(new prtyCallbackWrapper<pfxPostEffectObject>(this, &pfxPostEffectObject::UpdateProperties));
	}
	std::vector<effParamFloat*> vFloats;
	i_pParam->GetAllFloatParams(vFloats);
	n = vFloats.size();
	for (int i = 0; i < n; i++)
	{
		vFloats[i]->Property().AddCallback(new prtyCallbackWrapper<pfxPostEffectObject>(this, &pfxPostEffectObject::UpdateProperties));
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void pfxPostEffectObject::RebuildForm()
{
	//this->GetListContainer().RemoveAll();
	
	// added effect parameters
	/*DBG_ASSERT(m_pShaderDataProxy->GetUIShaderParams().m_pPrtyUI != NULL, "Shader UI bindings not yet created!");
	this->GetListContainer().Add(m_pShaderDataProxy->GetUIShaderParams().m_pPrtyUI->GetList());*/
	DBG_ASSERT(m_Data.m_pShaderParams->m_pPrtyUI != NULL, "Shader UI bindings not yet created!");
	m_ShaderUI->Add(m_Data.m_pShaderParams->m_pPrtyUI->GetList());

	//this->GetListContainer().Add(m_ShaderUI->GetList());
	/*this->GetListContainer().RemoveSubCategory(shared_ptr<prtyPropertyUIInfoContainer>(m_ShaderParams->m_pPrtyUI->GetListContainer()));
	this->GetListContainer().AddSubCategory(std::string("ShaderParams"), m_ShaderParams->m_pPrtyUI->GetListContainer());*/
	
}

//----------------------------------------------------------------------------
// property callbacks
//----------------------------------------------------------------------------
void pfxPostEffectObject::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	//g3dPostProcessing::SetActive(m_Data.m_bActive.GetValue());
	gpxRenderControl::SetNeedsNewRender();
}

void pfxPostEffectObject::UpdateShader(prtyProperty *i_pProperty, bool i_bDirty)
{
	fsLocator old_shader;// = m_ShaderParams->GetShaderName();
	if (m_Data.m_pShaderParams.get())
		old_shader = m_Data.m_pShaderParams->GetShaderName();
	fsLocator new_shader = m_Data.m_Name.GetValue();
	itString new_shader_filename = new_shader.GetLastName();

	if (new_shader != old_shader)
	{
		bool bAbortChange = true;

		// Does loading a user shader have to be done in main thread?
		// Safer to have this confirmation in the code first...
		gpxRenderControl::ConfirmSingleThread();

		matShaderEffect* eff = NULL;
		try
		{
			// If its a machstudio shader, only write out the name
			if ( matShaderMgr::IsMachStudioShader( new_shader ) )
			{
				new_shader = new_shader_filename;
			} 

			eff = matShaderMgr::GetPostEffect(new_shader);
		}
		catch(const g3dShaderLoadX& /*ex*/)
		{
			guiMessageBox::Show( "Shader could not be loaded. See debug.log for compilation error.", "Shader Change", guiMessageBox::e_OK );
			eff = NULL;
		}
		if (eff)
		{
			effShaderParams* params = new effShaderParams();
			params->SetShaderName(new_shader, eff);
			eff->BuildPrtyObject(params);

			if (m_Data.m_pShaderParams.get())
			{
				if (m_pMatchParams)
					params->SetMatchingParams(m_pMatchParams);
				RemoveProperties(m_ShaderUI, m_Data.m_pShaderParams->m_pPrtyUI);
				DeleteTextures(m_Data.m_pShaderParams.get());
			}

			m_Data.m_pShaderParams.reset(params);
			SetupRampParam();
			AddPropertiesCallback(m_Data.m_pShaderParams);
			//if (new_shader_filename == "GradientMap.fx")
			/*{
				ForceLoadTexture(fsLocator(itString("C:\\Users\\rchang\\Desktop\\TestScene\\color.bmp")));
			}*/
			//m_pShaderDataProxy.reset(new gpxShaderParams(*m_ShaderParams));
			RebuildForm();
			//g3dPostProcessing::SetPostEffect(m_ShaderParams);

			// If we were to call the renotify right away, it would delete the
			// control that is causing this callback. Bad things for the stack.
			switch(m_UIType.GetValue())
			{
			case pfxData::e_ViewPort:
				mnmThinkMgr::CallFunctionDelayed(&rndrPrefsDialogUtil::UpdatePfxPage);
				break;
			case pfxData::e_RenderLayer:
				//mnmThinkMgr::CallFunctionDelayed(&rdrLayersDialogUtil::UpdateTabs);
				rdrLayersDialogUtil::UpdatePfxShader();
				break;
			}

			bAbortChange = false;
		}
		else
		{
			// any extra handling of bad shader load here.
		}
		
	}
}

void pfxPostEffectObject::UpdateProperties(prtyProperty *i_pProperty, bool i_bDirty)
{
	gpxRenderControl::SetNeedsNewRender();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
pfxData& pfxPostEffectObject::GetData()
{
	return m_Data;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void pfxPostEffectObject::SetData(const pfxData& i_Data)
{
	// Set shader parameter first. It contains all params that updateShader need
	// This order is important, do not change it
	/*if (i_Data.m_pShaderParams.get())
		m_Data.m_pShaderParams = i_Data.m_pShaderParams;*/
	m_pMatchParams = i_Data.m_pShaderParams.get();
	
	m_Data.m_Name = i_Data.m_Name;
	m_Data.m_bActive = i_Data.m_bActive;

	m_pMatchParams = NULL;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void pfxPostEffectObject::SetupRampParam()
{
	if (!m_Data.m_pShaderParams.get())
		return;

	std::vector<effParamTexture*> vTextures;
	m_Data.m_pShaderParams->GetAllTextureParams(vTextures);
	int n = vTextures.size();
#ifdef SAVE_RMP_TEXTURE
	if (n > 0)
	{
		m_SaveRampToFile = prtyText("Save To", "");
		m_SaveRampToFileTrigger = prtyTrigger("");
		prtyPropertyUIInfo* pPUII;
		pPUII = new prtyTextBoxUIInfo(&(m_SaveRampToFile), "Ramp", "Description of the object");
		AddProperty( pPUII );

		prtyButtonUIInfo* pBUII;
		pBUII = new prtyButtonUIInfo(&(m_SaveRampToFileTrigger), "Ramp", "Delete Resume Point");
		pBUII->SetText(std::string("Save"));
		AddProperty( pBUII );

		m_SaveRampToFileTrigger.AddCallback(new prtyCallbackWrapper<pfxPostEffectObject>(this, &pfxPostEffectObject::SaveToFile));
	}
#endif
	for (int i = 0; i < n; i++)
	{
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
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pfxPostEffectObject::UpdateTexturePrty(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (!m_Data.m_pShaderParams.get())
		return;

	// find the right shader param for the prty.
	effParamTexture* pParam = NULL;

	prtyTextureFileChooserUIInfo* pTextureProperty = dynamic_cast<prtyTextureFileChooserUIInfo*>(i_pProperty);

	std::vector<effParamTexture*> vTextures;
	m_Data.m_pShaderParams->GetAllTextureParams(vTextures);
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
				std::unique_ptr<rmpObject> newRamp(new rmpObject);
				pParam->SetRampProperty((std::shared_ptr<prtyObject>)newRamp.release());
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
void pfxPostEffectObject::NotifyRampUI()
{
	if(m_pRampParam == NULL)
		return;

	rmpObject* pRampObject = dynamic_cast<rmpObject*>(m_pRampParam->GetRampProperty());
	if(pRampObject)
	{
		rmpDialogMgr::SetRampChangedCallback(NULL);
		rmpDialogMgr::SetData(pRampObject->m_Data);
		rmpDialogMgr::SetRampChangedCallback(std::bind(std::mem_fn(&pfxPostEffectObject::RampChangedFromUI), this, std::placeholders::_1));
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void pfxPostEffectObject::RampChangedFromUI(bool i_bDirty)
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
void pfxPostEffectObject::UpdateRampTexture(bool i_bDirty)
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
	}
}

////--------------------------------------------------------------------
////--------------------------------------------------------------------
void pfxPostEffectObject::ConfirmRampTexture(int i_texSize)
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

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pfxPostEffectObject::UpdateTexture(effParamTexture* i_pParam, bool i_bDirty)
{
	// Test to see if we are just restoring from a failed texture change.
	if (i_pParam->IsNameCurrent() && !i_bDirty)
		return;

	// stop any render threads for texture manager changes
	gpxRenderControl::ConfirmSingleThread();

	bool changed = LoadTexture(i_pParam, i_bDirty);

	if (!changed)
	{
		prtyTextureFileData val;
		val.m_TextureLocator = i_pParam->GetRestoreTextureName();
		// This line isn't changing the shader value back in the
		// user interface because it thinks the callback is from its local change.
		i_pParam->Property().SetValue(val);
		// So, ask for a new refresh from the ObjectDialog
		//mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
	}
}

//------------------------------------------------------------------------
// Load a texture.  Do not change out params if load fails.
//------------------------------------------------------------------------
bool pfxPostEffectObject::LoadTexture(effParamTexture* i_pParam, bool i_bDirty)
{
	matTexture* originalTexture = i_pParam->GetTexture();

	bool success = false;
	try
	{
		// Construct a fsResourceFinder to use to locate the textures in the object directory
		const bool bStrict = true; // has to return false if not found
		fsResourceFinderDir texture_finder(fsLocator(), bStrict);

		// Use resource finder that also searches through material library
		mtrTextureFinder lib_texture_finder(texture_finder);

		success = i_pParam->Load(lib_texture_finder);
	}
	catch ( const envExceptionX& i_Ex )
	{
		std::string msg = "Problem loading texture, " + i_Ex.GetErrorMessage();
		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Texture Load Error", guiMessageBox::e_OKOnly);
	}	

	if (success)
	{
		// need to remove texture from scriptobject's modeltemplate, and add new one to same.
		//mtrlOperations::ReplaceTexture(originalTexture, io_TextureSlot);
		//this->ReplaceTextureInTemplate(originalTexture, i_pParam->GetTexture()); // handle this here now, not in mtrlOperations
		// i waited until now to release the old texture.
		matTextureMgr::ReleaseTexture(originalTexture);
	}

	return success;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pfxPostEffectObject::DeleteTextures(effShaderParams* i_pParam)
{
	std::vector<effParamTexture*> vTextures;
	if (i_pParam)
	{
		i_pParam->GetAllTextureParams(vTextures);
		int n = vTextures.size();
		for (int i = 0; i < n; i++)
		{
			matTextureMgr::ReleaseTexture(vTextures[i]->GetTexture());
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#ifdef SAVE_RMP_TEXTURE
void pfxPostEffectObject::SaveToFile(prtyProperty *i_pProperty, bool i_bDirty)
{
	std::vector<effParamTexture*> vTextures;
	m_Data.m_pShaderParams->GetAllTextureParams(vTextures);
	int n = vTextures.size();
	
	for (int i = 0; i < n; i++)
	{
		//initialize any ramp data 
		if( vTextures[i]->GetRampProperty() != NULL )
		{
			if (m_pRampParam->GetTexture())
			{
				matTextureMgr::SaveTextureToFile(m_pRampParam->GetTexture(), fsLocator(itString(m_SaveRampToFile.GetValue().c_str())));
			}
		}
	}
}
#endif