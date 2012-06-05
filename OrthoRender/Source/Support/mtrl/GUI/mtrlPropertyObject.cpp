/****************************************************************************\
**  mtrlPropertyObject.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"

#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Core/Fs/fsFileUtil.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Graphics/eff/effShaderParams.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Tool/gui/guiMessageBox.hpp"

namespace
{
}	// end of namespace

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlPropertyObject::mtrlPropertyObject(const std::string& i_Name,
									   mdlMaterialInfo& i_Data,
									   matMaterial* i_pMaterial,
									   pick3dPickObject* i_pParent)
:	m_Name(i_Name), 
	m_Data(i_Data),
	m_pMaterial(i_pMaterial),
	m_pParent(i_pParent),
	m_pShaderData(NULL),
	m_pFurData(NULL),
	m_pGlowData(NULL),
	m_pOutlineData(NULL),
	m_pUVTransform(NULL),
	m_pReflectionData(NULL),
	m_ShaderTyp("Shader"),
	m_bEnableGlow("Enable Glow", i_Data.GetHasGlow()),
	m_bEnableFur("Enable Fur", i_Data.GetHasFur()),
	m_bEnableOutline("Enable Outline", i_Data.GetHasOutline()),
	m_bEnableReflection("Enable Reflection", i_Data.GetHasReflection())
{
	if (i_Data.GetShaderParams())
		m_ShaderTyp.SetValue(i_Data.GetShaderParams()->GetShaderName());
	else
		m_ShaderTyp.SetValue(i_Data.GetShader());

	// Register our local properties
	m_pShaderFileChooser = new prtyFileChooserUIInfo(&(m_ShaderTyp), " Shader", "Location of shader");
	m_pShaderFileChooser->SetShowFileNameOnly(true);
	m_pShaderFileChooser->SetInitialDirectory(matShaderMgr::GetShadersFolder());
	AddProperty( m_pShaderFileChooser );
	m_pEnableGlow = new prtyCheckBoxUIInfo(&m_bEnableGlow, "Glow", "Enable Glow");
	AddProperty( m_pEnableGlow );
	m_pEnableFur = new prtyCheckBoxUIInfo(&m_bEnableFur, "Fur", "Enable Fur");
	AddProperty( m_pEnableFur );
	m_pEnableOutline = new prtyCheckBoxUIInfo(&m_bEnableOutline, "Outline", "Enable Outline");
	AddProperty( m_pEnableOutline );

	m_ShaderTyp.AddCallback(new prtyCallbackWrapper<mtrlPropertyObject>(this, &mtrlPropertyObject::UpdateShader));
	m_bEnableGlow.AddCallback(new prtyCallbackWrapper<mtrlPropertyObject>(this, &mtrlPropertyObject::UpdateGlow));
	m_bEnableFur.AddCallback(new prtyCallbackWrapper<mtrlPropertyObject>(this, &mtrlPropertyObject::UpdateFur));
	m_bEnableOutline.AddCallback(new prtyCallbackWrapper<mtrlPropertyObject>(this, &mtrlPropertyObject::UpdateOutline));
	m_bEnableReflection.AddCallback(new prtyCallbackWrapper<mtrlPropertyObject>(this, &mtrlPropertyObject::UpdateReflection));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlPropertyObject::~mtrlPropertyObject()
{
	// Remove property UI infos that we don't own
	remove_properties(m_pShaderData);
	remove_properties(m_pFurData);
	remove_properties(m_pGlowData);
	remove_properties(m_pOutlineData);
	remove_properties(m_pUVTransform);
	remove_properties(m_pReflectionData);
	
	// Delete the shader objects (which do own the property ui infos)
	delete m_pShaderData;
	m_pShaderData = NULL;
	delete m_pFurData;
	m_pFurData = NULL;
	delete m_pGlowData;
	m_pGlowData = NULL;
	delete m_pOutlineData;
	m_pOutlineData = NULL;
	delete m_pUVTransform;
	m_pUVTransform = NULL;
	delete m_pReflectionData;
	m_pReflectionData = NULL;
}

//--------------------------------------------------------------------
// Set the shader property objects to use for this material
//--------------------------------------------------------------------
void mtrlPropertyObject::SetShaders(mtrlShaderObject* i_pShaderData,
									mtrlShaderObject* i_pFurData,
									mtrlShaderObject* i_pGlowData,
									mtrlShaderObject* i_pOutlineData,
									mtrlShaderObject* i_pUVTransform,
									mtrlShaderObject* i_pReflectionData)
{
	// Remove old properties
	remove_properties(m_pShaderData);
	remove_properties(m_pFurData);
	remove_properties(m_pGlowData);
	remove_properties(m_pOutlineData);
	remove_properties(m_pUVTransform);
	remove_properties(m_pReflectionData);
	
	// Clear out old data
	delete m_pShaderData;
	delete m_pFurData;
	delete m_pGlowData;
	delete m_pOutlineData;
	delete m_pUVTransform;
	delete m_pReflectionData;

	// Set the new ones
	m_pShaderData = i_pShaderData;
	m_pFurData = i_pFurData;
	m_pGlowData = i_pGlowData;
	m_pOutlineData = i_pOutlineData;
	m_pUVTransform = i_pUVTransform;
	m_pReflectionData = i_pReflectionData;

	// Gather the UI Properties from the shader objects all together
	this->GetListContainer().Add(m_pShaderData->GetList());
	this->GetListContainer().Add(m_pUVTransform->GetList());
	if (m_bEnableFur.GetValue())
		this->GetListContainer().Add(m_pFurData->GetList());
	if (m_bEnableGlow.GetValue())
		this->GetListContainer().Add(m_pGlowData->GetList());
	if (m_bEnableOutline.GetValue())
		GetListContainer().Add(m_pOutlineData->GetList());
	if (m_bEnableReflection.GetValue())
		this->GetListContainer().Add(m_pReflectionData->GetList());

	// Update our local properties to reflect the possible changes
	if (m_Data.GetShaderParams())
		m_ShaderTyp.SetValue(m_Data.GetShaderParams()->GetShaderName());
	else
		m_ShaderTyp.SetValue(m_Data.GetShader());
	m_bEnableGlow.SetValue(m_Data.GetHasGlow());
	m_bEnableFur.SetValue(m_Data.GetHasFur());
	m_bEnableOutline.SetValue(m_Data.GetHasOutline());
	m_bEnableReflection.SetValue(m_Data.GetHasReflection());
}

//--------------------------------------------------------------------
// Access to the three property objects per material
//--------------------------------------------------------------------
mtrlShaderObject* mtrlPropertyObject::GetShaderDataObject()
{		
	return m_pShaderData;
}
mtrlShaderObject* mtrlPropertyObject::GetFurDataObject()
{
	return m_pFurData;
}
mtrlShaderObject* mtrlPropertyObject::GetGlowDataObject()
{
	return m_pGlowData;
}
mtrlShaderObject* mtrlPropertyObject::GetOutlineDataObject()
{
	return m_pOutlineData;
}
mtrlShaderObject* mtrlPropertyObject::GetUVTransformObject()
{
	return m_pUVTransform;
}
mtrlShaderObject* mtrlPropertyObject::GetReflectionDataObject()
{
	return m_pReflectionData;
}

//--------------------------------------------------------------------
// Add channels from shader objects into this script object
//--------------------------------------------------------------------
void mtrlPropertyObject::AddChannels(tmlnScriptObject* i_pScriptObject)
{
	if (i_pScriptObject)
	{
		if (m_pShaderData)
			m_pShaderData->AddChannels( i_pScriptObject );
		if (m_pFurData)
			m_pFurData->AddChannels( i_pScriptObject );
		if (m_pGlowData)
			m_pGlowData->AddChannels( i_pScriptObject );
		if (m_pOutlineData)
			m_pOutlineData->AddChannels( i_pScriptObject );
		if (m_pUVTransform)
			m_pUVTransform->AddChannels( i_pScriptObject );
		if (m_pReflectionData)
			m_pReflectionData->AddChannels( i_pScriptObject );
	}
}
//--------------------------------------------------------------------
// Remove channels from this property object
//--------------------------------------------------------------------
void mtrlPropertyObject::RemoveChannels(tmlnScriptObject* i_pScriptObject)
{
	if (i_pScriptObject)
	{
		if (m_pShaderData)
			m_pShaderData->RemoveChannels( i_pScriptObject );
		if (m_pFurData)
			m_pFurData->RemoveChannels( i_pScriptObject );
		if (m_pGlowData)
			m_pGlowData->RemoveChannels( i_pScriptObject );
		if (m_pOutlineData)
			m_pOutlineData->RemoveChannels( i_pScriptObject );
		if (m_pUVTransform)
			m_pUVTransform->RemoveChannels( i_pScriptObject );
		if (m_pReflectionData)
			m_pReflectionData->RemoveChannels( i_pScriptObject );
	}
}

//------------------------------------------------------------------------
// Returns true if one of its shaders has a material animation
//------------------------------------------------------------------------
bool mtrlPropertyObject::HasMaterialAnimation() const
{
	if (m_pShaderData && m_pShaderData->HasMaterialAnimation())
		return true;
	if (m_pFurData && m_pFurData->HasMaterialAnimation())
		return true;
	if (m_pGlowData && m_pGlowData->HasMaterialAnimation())
		return true;
	if (m_pOutlineData && m_pOutlineData->HasMaterialAnimation())
		return true;
	if (m_pUVTransform && m_pUVTransform->HasMaterialAnimation())
		return true;
	if (m_pReflectionData && m_pReflectionData->HasMaterialAnimation())
		return true;

	return false;
}

//------------------------------------------------------------------------
// Set a new directory for the material in order to find the textures.
// Called when the material has been exported to the material library.
//------------------------------------------------------------------------
void mtrlPropertyObject::UpdateTextureDirectory(const fsLocator &i_TextureDir)
{
	if (m_pShaderData)
		m_pShaderData->UpdateTextureDirectory( i_TextureDir );
	if (m_pFurData)
		m_pFurData->UpdateTextureDirectory( i_TextureDir );
	if (m_pGlowData)
		m_pGlowData->UpdateTextureDirectory( i_TextureDir );
	if (m_pOutlineData)
		m_pOutlineData->UpdateTextureDirectory( i_TextureDir );
	if (m_pUVTransform)
		m_pUVTransform->UpdateTextureDirectory( i_TextureDir );
	if (m_pReflectionData)
		m_pReflectionData->UpdateTextureDirectory( i_TextureDir );
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
void mtrlPropertyObject::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
	if (m_pShaderData)
		m_pShaderData->GetTextureList(o_TextureList);
	if (m_pFurData)
		m_pFurData->GetTextureList(o_TextureList);
	if (m_pGlowData)
		m_pGlowData->GetTextureList(o_TextureList);
	if (m_pOutlineData)
		m_pOutlineData->GetTextureList(o_TextureList);
	if (m_pUVTransform)
		m_pUVTransform->GetTextureList(o_TextureList);
	if (m_pReflectionData)
		m_pReflectionData->GetTextureList(o_TextureList);
}

//============================================================================
// pick3dPickObject - virtual function overrides
//============================================================================

//------------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//------------------------------------------------------------------------
//virtual 
pick3dPickObject* mtrlPropertyObject::GetParentObject() const
{
	return m_pParent;
}

//------------------------------------------------------------------------
// Get the name of the object.
//------------------------------------------------------------------------
//virtual 
std::string mtrlPropertyObject::GetPick3dName() const
{
//	return m_Name;	
	
	std::string material_name = m_Name;
	if (m_Data.UsesMaterialLibrary())
	{
		// Add relative path to library file to end of material name
		material_name += " : ";
		std::string rel_path;
		fsFileUtil::LocatorToANSIFilename(m_Data.GetLibraryFilename(), rel_path);
		material_name += rel_path;
	}
	return material_name;
}

//----------------------------------------------------------------------------
// remove the properties of this object from our property UI info list
//----------------------------------------------------------------------------
void mtrlPropertyObject::remove_properties(prtyObject* i_pObject)
{
	if (i_pObject)
	{
		PropertyUIIList::const_iterator it, end = i_pObject->GetList().end();
		for (it = i_pObject->GetList().begin(); it != end; ++it)
		{
			shared_ptr<prtyPropertyUIInfo> ui_Info(*it);
			this->GetListContainer().Remove(ui_Info);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlPropertyObject::UpdateShader(prtyProperty *i_pProperty, bool i_bDirty)
{
	itString old_shader = m_Data.GetShader();
	// compare with last name of the file path.
	// that means, for now, only search the main shaders directory!!
	itString new_shader = m_ShaderTyp.GetValue().GetLastName();

	if (new_shader != old_shader)
	{
		bool bAbortChange = true;
		if (this->HasMaterialAnimation())
		{
			guiMessageBox::Show( "This material is animated and cannot be changed in this way.", "Material AnimationExists", guiMessageBox::e_OK );
		}
		else
		{
			int retval = guiMessageBox::Show( "Changing shader will lose all old shader parameters. Continue?", "Shader Change", guiMessageBox::e_YesNo );
			if ( retval == guiMessageBox::e_Yes )
			{
				matShaderEffect* eff = NULL;
				try
				{
					eff = matShaderMgr::GetEffect(new_shader);
				}
				catch(const g3dShaderLoadX& /*ex*/)
				{
					guiMessageBox::Show( "Shader could not be loaded. See debug.log for compilation error.", "Shader Change", guiMessageBox::e_OK );
					eff = NULL;
				}
				if (eff)
				{
					shared_ptr<effShaderParams> params(new effShaderParams());
					params->SetShaderName(new_shader, eff);
					eff->BuildPrtyObject(params.get());

					m_Data.SetHasReflection(eff->HasReflectionMap());
					mdlMaterialInfo new_material(m_Data);
					new_material.SetShaderParams(params);
					//new_material.SetShader(shaderTable[new_shader_index].m_Name, shaderTable[new_shader_index].m_DataTemplate);

					const bool bUpdateProperties = true; 
					mtrlOperations::ChangeMaterialData(new_material, bUpdateProperties);

					// If we were to call the renotify right away, it would delete the
					// control that is causing this callback. Bad things for the stack.
					mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);

					bAbortChange = false;
				}
				else
				{
					// any extra handling of bad shader load here.
				}
			}
		}

		// If we didn't execute the shader change, put the old shader index back into the
		// property. Although this will also trigger a callback, it will not pass into the
		// "if" case because the new and old shader indices will be the same.
		if (bAbortChange)
		{
			// This line isn't changing the shader value back in the
			// user interface because it thinks the callback is from its local change.
			m_ShaderTyp.SetValue(old_shader);
			// So, ask for a new refresh from the ObjectDialog
			mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
		}

	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlPropertyObject::UpdateFur(prtyProperty *i_pProperty, bool i_bDirty)
{
	bool bEnabled = m_bEnableFur.GetValue();

	// Set value in material and data
	m_Data.SetHasFur(bEnabled);
	m_pMaterial->SetHasFur(bEnabled);

	// Adjust the properties that are displayed
	if (bEnabled)
		this->GetListContainer().Add(m_pFurData->GetList());
	else
		this->remove_properties(m_pFurData);

	// If we were to call the renotify right away, it would delete the
	// control that is causing this callback. Bad things for the stack.
	mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlPropertyObject::UpdateGlow(prtyProperty *i_pProperty, bool i_bDirty)
{
	bool bEnabled = m_bEnableGlow.GetValue();

	// Set value in material and data
	m_Data.SetHasGlow(bEnabled);
	m_pMaterial->SetHasGlow(bEnabled);

	// Adjust the properties that are displayed
	if (bEnabled)
		this->GetListContainer().Add(m_pGlowData->GetList());
	else
		this->remove_properties(m_pGlowData);

	// If we were to call the renotify right away, it would delete the
	// control that is causing this callback. Bad things for the stack.
	mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlPropertyObject::UpdateOutline(prtyProperty *i_pProperty, bool i_bDirty)
{
	bool bEnabled = m_bEnableOutline.GetValue();

	// Set value in material and data
	m_Data.SetHasOutline(bEnabled);
	m_pMaterial->SetHasOutline(bEnabled);

	// Adjust the properties that are displayed
	if (bEnabled)	GetListContainer().Add(m_pOutlineData->GetList());
	else			remove_properties(m_pOutlineData);

	// If we were to call the renotify right away, it would delete the
	// control that is causing this callback. Bad things for the stack.
	mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlPropertyObject::UpdateReflection(prtyProperty *i_pProperty, bool i_bDirty)
{
	bool bEnabled = m_bEnableReflection.GetValue();

	// Set value in material and data
	m_Data.SetHasReflection(bEnabled);
	m_pMaterial->SetHasReflection(bEnabled);

	// Adjust the properties that are displayed
	if (bEnabled)
		this->GetListContainer().Add(m_pReflectionData->GetList());
	else
		this->remove_properties(m_pReflectionData);

	// If we were to call the renotify right away, it would delete the
	// control that is causing this callback. Bad things for the stack.
	mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
}
