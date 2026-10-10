/****************************************************************************\
**  mtrlPropertyObject.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"

#include "Support/brsh/brshBrushStroke.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Support/rmp/rmpDialogMgr.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Core/Fs/fsFileUtil.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFilePathComboBoxUIInfo.hpp"
#include "Graphics/eff/effShaderParams.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiMessageBox.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	// Dropdown of the built-in material shaders, labelled by effect name
	//	("Phong") and storing the shader name ("Phong.fx"). A shader that is
	//	not in the list (an old or custom one) is added by the control.
	//------------------------------------------------------------------------
	prtyFilePathComboBoxUIInfo* create_shader_picker(prtyFilePath& io_ShaderType)
	{
		prtyFilePathComboBoxUIInfo *pShaderPickerUI = new prtyFilePathComboBoxUIInfo(&io_ShaderType, "Shader", "Material shader");
		pShaderPickerUI->SetConfirmationString("Changing shader may lose all old shader parameters. Continue?");

		const std::vector<matShaderInfo>& shaders = matShaderMgr::GetMaterialShaders();
		for (size_t i = 0; i < shaders.size(); i++)
		{
			std::string label = itStringUtil::GetStdString(shaders[i].m_Name);
			const size_t dot = label.rfind('.');
			if (dot != std::string::npos)
				label.erase(dot);
			pShaderPickerUI->AddChoice(label, fsLocator(shaders[i].m_Name));
		}
		return pShaderPickerUI;
	}

}	// end of namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mtrlShaderTypeProperty::mtrlShaderTypeProperty(const std::string& i_Name)
: prtyFilePath(i_Name)
{
}

//--------------------------------------------------------------------
// Create an undo operation of the correct type for this
// property. A reference to this property should be passed in.
// Ownership passes to the caller.
//--------------------------------------------------------------------
undoUndoOperation* mtrlShaderTypeProperty::CreateUndoOperation(shared_ptr<prtyPropertyReference> i_pPropertyRef)
{
	// when shader tyope changes, it changes all properties of the material object,
	// so we have to create a different type of undo operation that backs up the
	// full material data structure instead of a single property undo operation.
	return NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlPropertyObject::mtrlPropertyObject(const std::string& i_Name,
									   mdlMaterialInfo& i_Data,
									   matMaterial* i_pMaterial)
:	m_Name(i_Name), 
	m_Data(i_Data),
	m_pMaterial(i_pMaterial),
	//m_pParent(i_pParent),
	m_pShaderData(NULL),
	m_pGlowData(NULL),
	m_pOutlineData(NULL),
	m_pUVTransform(NULL),
	m_pReflectionData(NULL),
	m_pDisplacementData(NULL),
	m_pRendermanOverrideData(NULL),
	m_pNormalsData(NULL),
	m_pTextureFilterData(NULL),
	m_ShaderType("Shader",(i_pMaterial->GetShaderParams()) ? i_pMaterial->GetShaderParams()->GetShaderName() : fsLocator() ),
	m_bEnableGlow("Enable Glow", i_Data.GetHasGlow()),
	m_bEnableOutline("Enable Outline", i_Data.GetHasOutline()),
	m_bEnableReflection("Enable Reflection", i_Data.GetHasReflection()),
	m_bEnableDisplacement("Enable Displacement", i_Data.GetHasDisplacement() ),
	m_bEnableRendermanOverride("Enable RenderMan Override", i_Data.GetHasRendermanOverride() ),
	m_pBrushStroke(NULL)
{
	// Our properties are organized into two subcategories, 
	// 1) the base material layer data, and 
	// 2) the global material dataavailable to all layers
	m_BaseLayerUI.reset(new prtyPropertyUIInfoContainer);
	this->GetListContainer().AddSubCategory("Base Layer", m_BaseLayerUI);
	// There are two alternatives here, use the same UI category for base and global
	// properties, or separate them into two different containers and categories.
	m_GlobalUI = m_BaseLayerUI;
	//m_GlobalUI.reset(new prtyPropertyUIInfoContainer);
	//this->GetListContainer().AddSubCategory("Global", m_GlobalUI);

	m_pShaderFilePicker.reset(create_shader_picker(m_ShaderType));
	m_BaseLayerUI->Add( m_pShaderFilePicker );

	// These property controls must be added later to ensure correct ordering of UI elements.
	m_pEnableGlow.reset(new prtyCheckBoxUIInfo(&m_bEnableGlow, "Glow", "Enable Glow"));
	//AddProperty( m_pEnableGlow );
	m_pEnableOutline.reset(new prtyCheckBoxUIInfo(&m_bEnableOutline, "Outline", "Enable Outline"));
//	AddProperty( m_pEnableOutline );

	m_pEnableDisplacement.reset(new prtyCheckBoxUIInfo(&m_bEnableDisplacement, "Displacement", "Enable Displacement"));
	m_pEnableDisplacement->SetDescription( "Enable displacement mapping with hardware tessellation.");

#if(SGPU_APP == MS_CORE)
	m_pEnableDisplacement->SetReadOnly( true );
#endif

	m_GlobalUI->Add( m_pEnableDisplacement );

	m_pEnableRendermanOverride.reset(new prtyCheckBoxUIInfo(&m_bEnableRendermanOverride, "RenderMan Override", "Enable Renderman Override"));
	m_GlobalUI->Add( m_pEnableRendermanOverride );

	m_ShaderType.AddCallback(new prtyCallbackWrapper<mtrlPropertyObject>(this, &mtrlPropertyObject::UpdateShader));
	m_bEnableGlow.AddCallback(new prtyCallbackWrapper<mtrlPropertyObject>(this, &mtrlPropertyObject::UpdateGlow));
	m_bEnableOutline.AddCallback(new prtyCallbackWrapper<mtrlPropertyObject>(this, &mtrlPropertyObject::UpdateOutline));
	m_bEnableReflection.AddCallback(new prtyCallbackWrapper<mtrlPropertyObject>(this, &mtrlPropertyObject::UpdateReflection));
	m_bEnableDisplacement.AddCallback(new prtyCallbackWrapper<mtrlPropertyObject>(this, &mtrlPropertyObject::UpdateDisplacement));
	m_bEnableRendermanOverride.AddCallback(new prtyCallbackWrapper<mtrlPropertyObject>(this, &mtrlPropertyObject::UpdateRendermanOverride));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlPropertyObject::~mtrlPropertyObject()
{
	cleanup_shader_objects();
	delete m_pBrushStroke;
	m_pBrushStroke = NULL;
}

//--------------------------------------------------------------------
// Set the shader property objects to use for this material
//--------------------------------------------------------------------
void mtrlPropertyObject::SetShaders(mtrlShaderObject* i_pShaderData,
									mtrlShaderObject* i_pGlowData,
									mtrlShaderObject* i_pOutlineData,
									mtrlShaderObject* i_pUVTransform,
									mtrlShaderObject* i_pReflectionData,
									mtrlShaderObject* i_pDisplacementData,
									mtrlShaderObject* i_pNormalsData,
									mtrlShaderObject* i_pRendermanOverrideData,
									mtrlShaderObject* i_pTextureFilterData)
{
	// Remove old properties and shader objects
	cleanup_shader_objects();

	// Set the new ones
	m_pShaderData = i_pShaderData;
	m_pGlowData = i_pGlowData;
	m_pOutlineData = i_pOutlineData;
	m_pUVTransform = i_pUVTransform;
	m_pReflectionData = i_pReflectionData;
	m_pDisplacementData = i_pDisplacementData;
	m_pNormalsData = i_pNormalsData;
	m_pRendermanOverrideData = i_pRendermanOverrideData;
	m_pTextureFilterData = i_pTextureFilterData;

	// Update our local properties to reflect the possible changes
	fsLocator currentShaderName;
	if (m_Data.GetShaderParams())
		currentShaderName = (m_Data.GetShaderParams()->GetShaderName());
	else
		currentShaderName = (m_Data.GetShader());

	// Gather the UI Properties from the shader objects all together
	if (m_pShaderData) {
		m_BaseLayerUI->Add(m_pShaderData->GetList());
	}
	m_BaseLayerUI->Add(m_pUVTransform->GetList());
	m_BaseLayerUI->Add(m_pNormalsData->GetList());
	m_BaseLayerUI->Add(m_pTextureFilterData->GetList());
	m_GlobalUI->Add( m_pEnableGlow );
	if (m_bEnableGlow.GetValue())
		m_GlobalUI->Add(m_pGlowData->GetList());

	fsLocator fsLoc = matShaderMgr::ResolveShaderPath(m_ShaderType.GetValue());

	matShaderEffect* effect = matShaderMgr::GetEffect(fsLoc);
	//if (matShaderMgr::QueryShaderAnnotationBool(fsLoc, "SupportsOutline"))
	if (effect)
	{
		if (effect->GetSupportOutline())
		{
			m_Data.SetOutlineVisible(true);
			m_GlobalUI->Add( m_pEnableOutline );
			if (m_bEnableOutline.GetValue())
				m_GlobalUI->Add(m_pOutlineData->GetList());
		}
		else
		{
			m_bEnableOutline.SetValue(false);	//disable outline if not visible
		}
	}

	if (m_bEnableReflection.GetValue())
		m_GlobalUI->Add(m_pReflectionData->GetList());
	m_GlobalUI->Add( m_pEnableDisplacement );
	if (m_bEnableDisplacement.GetValue())
		m_GlobalUI->Add(m_pDisplacementData->GetList());

	m_GlobalUI->Add( m_pEnableRendermanOverride );
	if (m_bEnableRendermanOverride.GetValue())
		m_GlobalUI->Add(m_pRendermanOverrideData->GetList());

	m_bEnableGlow.SetValue(m_Data.GetHasGlow());
	m_bEnableOutline.SetValue(m_Data.GetHasOutline());
	m_bEnableReflection.SetValue(m_Data.GetHasReflection());
	m_bEnableDisplacement.SetValue(m_Data.GetHasDisplacement());
	m_bEnableRendermanOverride.SetValue(m_Data.GetHasRendermanOverride());
}

//--------------------------------------------------------------------
// Set the shader property objects for a new material layer.
// The material layer should already exists in the m_Data structure.
//--------------------------------------------------------------------
void mtrlPropertyObject::SetMaterialLayerShaders(int i_LayerIndex,
										  mtrlShaderObject* i_pShaderData,
										  mtrlShaderObject* i_pUVTransform)
{
	// get a counter that starts at "2" because base layer is stored elsewhere,
	// so our first layer through this method is layer #2.
	//int layer_index = m_MaterialLayerProperties.size() + 2;
	//bga - but not using the index in the name right now anyway

	// Create the subcategory for the extra material layer beyond the base data
	sMaterialLayerProperties layer;
	layer.m_LayerUI.reset(new prtyPropertyUIInfoContainer());
	this->GetListContainer().AddSubCategory("Material Layer", layer.m_LayerUI);

	// Create a new shader picker for this layer
	layer.m_pLayerShaderType = new prtyFilePath();
	layer.m_LayerUI->Add( create_shader_picker(*layer.m_pLayerShaderType) );
	layer.m_pLayerShaderType->AddCallback(new prtyCallbackWrapper<mtrlPropertyObject>(this, &mtrlPropertyObject::UpdateShader));

	//layer.m_pShaderData = NULL;
	//layer.m_pUVTransform = NULL;	
	layer.m_pShaderData = i_pShaderData;
	layer.m_pUVTransform = i_pUVTransform;

	// Gather the UI Properties from the shader objects all together
	layer.m_LayerUI->Add(layer.m_pShaderData->GetList());
	layer.m_LayerUI->Add(layer.m_pUVTransform->GetList());

	m_MaterialLayerProperties.push_back( layer );
}

//--------------------------------------------------------------------
// Set the shader property objects for a new material layer.
// The material layer should already exists in the m_Data structure.
//--------------------------------------------------------------------
void mtrlPropertyObject::ChangeMaterialLayerShaders(int i_LayerIndex,
										  mtrlShaderObject* i_pShaderData,
										  mtrlShaderObject* i_pUVTransform)
{
	const int num_layers = m_MaterialLayerProperties.size();
	DBG_ASSERT(i_LayerIndex > 0, "ChangeMaterialLayerShaders incorrectly called on default layer.");
	DBG_ASSERT(i_LayerIndex <= num_layers, "ChangeMaterialLayerShaders changing a layer that doesn't exist");

	// offset for array of non-default layers.
	int layerArrayIndex = i_LayerIndex - 1;

	// edit the layer in-place.
	sMaterialLayerProperties& layer = m_MaterialLayerProperties[layerArrayIndex];

	remove_properties(layer.m_LayerUI, layer.m_pShaderData);
	remove_properties(layer.m_LayerUI, layer.m_pUVTransform);
	delete layer.m_pShaderData;
	delete layer.m_pUVTransform;

	layer.m_pShaderData = i_pShaderData;
	layer.m_pUVTransform = i_pUVTransform;
	// Gather the UI Properties from the shader objects all together
	layer.m_LayerUI->Add(layer.m_pShaderData->GetList());
	layer.m_LayerUI->Add(layer.m_pUVTransform->GetList());
}

//--------------------------------------------------------------------
// Set shader type for given material layer index
//--------------------------------------------------------------------
void mtrlPropertyObject::SetLayerShaderName(int i_LayerIndex, const fsLocator& i_ShaderType)
{
	// offset for array of non-default layers.
	int layerArrayIndex = i_LayerIndex - 1;

	if ((layerArrayIndex >= 0) && (layerArrayIndex < m_MaterialLayerProperties.size()))
	{
		// edit the layer in-place.
		sMaterialLayerProperties& layer = m_MaterialLayerProperties[layerArrayIndex];

		layer.m_pLayerShaderType->SetValue( i_ShaderType );
	}
}

//--------------------------------------------------------------------
// Access to the three property objects per material
//--------------------------------------------------------------------
mtrlShaderObject* mtrlPropertyObject::GetShaderDataObject()
{		
	return m_pShaderData;
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
mtrlShaderObject* mtrlPropertyObject::GetDisplacementDataObject()
{
	return m_pDisplacementData;
}
mtrlShaderObject* mtrlPropertyObject::GetNormalsDataObject()
{
	return m_pNormalsData;
}
mtrlShaderObject* mtrlPropertyObject::GetRendermanOverrideDataObject()
{
	return m_pRendermanOverrideData;
}
mtrlShaderObject* mtrlPropertyObject::GetTextureFilterDataObject()
{
	return m_pTextureFilterData;
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
		if (m_pGlowData)
			m_pGlowData->AddChannels( i_pScriptObject );
		if (m_pOutlineData)
			m_pOutlineData->AddChannels( i_pScriptObject );
		if (m_pUVTransform)
			m_pUVTransform->AddChannels( i_pScriptObject );
		if (m_pReflectionData)
			m_pReflectionData->AddChannels( i_pScriptObject );
		if (m_pDisplacementData)
			m_pDisplacementData->AddChannels( i_pScriptObject );
		if (m_pNormalsData)
			m_pNormalsData->AddChannels( i_pScriptObject );
		if (m_pRendermanOverrideData)
			m_pRendermanOverrideData->AddChannels( i_pScriptObject );
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
		if (m_pGlowData)
			m_pGlowData->RemoveChannels( i_pScriptObject );
		if (m_pOutlineData)
			m_pOutlineData->RemoveChannels( i_pScriptObject );
		if (m_pUVTransform)
			m_pUVTransform->RemoveChannels( i_pScriptObject );
		if (m_pReflectionData)
			m_pReflectionData->RemoveChannels( i_pScriptObject );
		if (m_pDisplacementData)
			m_pDisplacementData->RemoveChannels( i_pScriptObject );
		if (m_pNormalsData)
			m_pNormalsData->RemoveChannels( i_pScriptObject );
		if (m_pRendermanOverrideData)
			m_pRendermanOverrideData->RemoveChannels( i_pScriptObject );
	}
}

//------------------------------------------------------------------------
// Returns true if one of its shaders has a material animation
//------------------------------------------------------------------------
bool mtrlPropertyObject::HasMaterialAnimation() const
{
	if (m_pShaderData && m_pShaderData->HasMaterialAnimation())
		return true;
	if (m_pGlowData && m_pGlowData->HasMaterialAnimation())
		return true;
	if (m_pOutlineData && m_pOutlineData->HasMaterialAnimation())
		return true;
	if (m_pUVTransform && m_pUVTransform->HasMaterialAnimation())
		return true;
	if (m_pReflectionData && m_pReflectionData->HasMaterialAnimation())
		return true;
	if (m_pDisplacementData && m_pDisplacementData->HasMaterialAnimation())
		return true;
	if (m_pNormalsData && m_pNormalsData->HasMaterialAnimation())
		return true;
	if (m_pRendermanOverrideData && m_pRendermanOverrideData->HasMaterialAnimation())
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
	if (m_pGlowData)
		m_pGlowData->UpdateTextureDirectory( i_TextureDir );
	if (m_pOutlineData)
		m_pOutlineData->UpdateTextureDirectory( i_TextureDir );
	if (m_pUVTransform)
		m_pUVTransform->UpdateTextureDirectory( i_TextureDir );
	if (m_pReflectionData)
		m_pReflectionData->UpdateTextureDirectory( i_TextureDir );
	if (m_pDisplacementData)
		m_pDisplacementData->UpdateTextureDirectory( i_TextureDir );
	if (m_pNormalsData)
		m_pNormalsData->UpdateTextureDirectory( i_TextureDir );
	if (m_pRendermanOverrideData)
		m_pRendermanOverrideData->UpdateTextureDirectory( i_TextureDir );
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
void mtrlPropertyObject::GetTextureList(std::vector<fsLocator>& o_TextureList)
{
	if (m_pShaderData)
		m_pShaderData->GetTextureList(o_TextureList);
	if (m_pGlowData)
		m_pGlowData->GetTextureList(o_TextureList);
	if (m_pOutlineData)
		m_pOutlineData->GetTextureList(o_TextureList);
	if (m_pUVTransform)
		m_pUVTransform->GetTextureList(o_TextureList);
	if (m_pReflectionData)
		m_pReflectionData->GetTextureList(o_TextureList);
	if (m_pDisplacementData)
		m_pDisplacementData->GetTextureList(o_TextureList);
	if (m_pNormalsData)
		m_pNormalsData->GetTextureList(o_TextureList);
	if (m_pRendermanOverrideData)
		m_pRendermanOverrideData->GetTextureList(o_TextureList);
}

//------------------------------------------------------------------------
// Set the brush stroke object associated with this material
//------------------------------------------------------------------------
void mtrlPropertyObject::SetBrushStroke(brshBrushStroke* i_pBrushStroke)
{
	m_pBrushStroke = i_pBrushStroke;
}

//------------------------------------------------------------------------
// Get the brush stroke object associated with this material
//------------------------------------------------------------------------
brshBrushStroke* mtrlPropertyObject::GetBrushStroke() const
{
	return m_pBrushStroke;
}

//------------------------------------------------------------------------
// Get the Base UI for this object
//------------------------------------------------------------------------
prtyPropertyUIInfoContainer& mtrlPropertyObject::GetBaseUI() const
{
	return *m_BaseLayerUI;
}

//--------------------------------------------------------------------
// Set the shader name
//--------------------------------------------------------------------
void mtrlPropertyObject::SetShaderName(fsLocator i_ShaderType)
{
	m_ShaderType.SetValue( i_ShaderType );
}

//============================================================================
// sel3dObject - virtual function overrides
//============================================================================

//------------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//------------------------------------------------------------------------
//virtual 
//sel3dObject* mtrlPropertyObject::GetParentObject() const
//{
//	return m_pParent;
//}

//------------------------------------------------------------------------
// Get the name of the object.
//------------------------------------------------------------------------
//virtual 
std::string mtrlPropertyObject::GetDisplayName() const
{
//	return m_Name;	
	
	std::string material_name = m_Name;
	if (m_Data.UsesMaterialLibrary())
	{
		// Add relative path to library file to end of material name
		material_name += " : ";
		std::string rel_path;
		// just filename for now, no relative paths from a standardized 
		// material library anymore
		//fsFileUtil::LocatorToANSIFilename(m_Data.GetLibraryFilename(), rel_path);
		rel_path = itStringUtil::GetStdString(m_Data.GetLibraryFilename().GetLastName());
		material_name += rel_path;
	}
	return material_name;
}

//----------------------------------------------------------------------------
// remove the properties of this object from our property UI info list
//----------------------------------------------------------------------------
void mtrlPropertyObject::remove_properties(shared_ptr<prtyPropertyUIInfoContainer>& i_SubCategory,
										   prtyObject* i_pObject)
{
	if (i_pObject)
	{
		PropertyUIIList::const_iterator it, end = i_pObject->GetList().end();
		for (it = i_pObject->GetList().begin(); it != end; ++it)
		{
			shared_ptr<prtyPropertyUIInfo> ui_Info(*it);
			i_SubCategory->Remove(ui_Info);
		}
	}
}

//----------------------------------------------------------------------------
// part of the cleanup of the shader objects is the step that removes the
// subcategories and properties associated with the extra material layers.
//----------------------------------------------------------------------------
void mtrlPropertyObject::remove_material_layers()
{
	const int num_layers = m_MaterialLayerProperties.size();
	for (int i=0; i<num_layers; ++i)
	{
		sMaterialLayerProperties& layer = m_MaterialLayerProperties[i];
		this->GetListContainer().RemoveSubCategory(layer.m_LayerUI);

		remove_properties(layer.m_LayerUI, layer.m_pShaderData);
		remove_properties(layer.m_LayerUI, layer.m_pUVTransform);
		delete layer.m_pShaderData;
		delete layer.m_pUVTransform;

		layer.m_LayerUI.reset();
		delete layer.m_pLayerShaderType;
	}
	m_MaterialLayerProperties.clear();
}

//----------------------------------------------------------------------------
// removes up all properties from all shader objects from our list,
// deletes all shader objects
//----------------------------------------------------------------------------
void mtrlPropertyObject::cleanup_shader_objects()
{
	// clean up the extra material layers above the base layer
	remove_material_layers();

	// quick fix of wrong binding callback
	rmpDialogMgr::SetRampChangedCallback(NULL);

	// Remove property UI infos that we don't own
	remove_properties(m_BaseLayerUI, m_pShaderData);
	remove_properties(m_BaseLayerUI, m_pUVTransform);
	m_GlobalUI->Remove(m_pEnableGlow);
	remove_properties(m_GlobalUI, m_pGlowData);
	m_GlobalUI->Remove(m_pEnableOutline);
	m_GlobalUI->Remove(m_pEnableDisplacement);
	m_GlobalUI->Remove(m_pEnableRendermanOverride);
	remove_properties(m_GlobalUI, m_pOutlineData);
	remove_properties(m_GlobalUI, m_pReflectionData);
	remove_properties(m_GlobalUI, m_pDisplacementData);
	remove_properties(m_GlobalUI, m_pNormalsData);
	remove_properties(m_GlobalUI, m_pTextureFilterData);
	remove_properties(m_GlobalUI, m_pRendermanOverrideData);
	
	
	// Delete the shader objects (which do own the property ui infos)
	delete m_pShaderData;
	m_pShaderData = NULL;
	delete m_pGlowData;
	m_pGlowData = NULL;
	delete m_pOutlineData;
	m_pOutlineData = NULL;
	delete m_pUVTransform;
	m_pUVTransform = NULL;
	delete m_pReflectionData;
	m_pReflectionData = NULL;
	delete m_pDisplacementData;
	m_pDisplacementData = NULL;
	delete m_pNormalsData;
	m_pNormalsData = NULL;
	delete m_pTextureFilterData;
	m_pTextureFilterData = NULL;
	delete m_pRendermanOverrideData;
	m_pRendermanOverrideData = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlPropertyObject::UpdateShader(prtyProperty *i_pProperty, bool i_bDirty)
{
	// Find the layer index for this shader type property
	int layer_index = 0;
	prtyFilePath* pFilePathPrty = &m_ShaderType;
	for (int i=0; i<m_MaterialLayerProperties.size(); i++)
	{
		if (m_MaterialLayerProperties[i].m_pLayerShaderType == i_pProperty)
		{
			layer_index = i+1;
			pFilePathPrty = m_MaterialLayerProperties[i].m_pLayerShaderType;
			break;
		}
	}

	fsLocator old_shader = m_Data.GetShader(layer_index);
	fsLocator new_shader = pFilePathPrty->GetValue();
	itString new_shader_filename = new_shader.GetLastName();

	if (new_shader != old_shader)
	{
		bool bAbortChange = true;
		if (this->HasMaterialAnimation())
		{
			guiMessageBox::Show( "This material is animated and cannot be changed in this way.", "Material AnimationExists", guiMessageBox::e_OK );
		}
		else
		{
			// Does loading a user shader have to be done in main thread?
			// Safer to have this confirmation in the code first...
			gpxRenderControl::ConfirmSingleThread();

			//bga - This confirmation dilaog is moved to the UIInfo and is now displayed and confirmed before the property is actually changed.
			//int retval = guiMessageBox::Show( "Changing shader may lose all old shader parameters. Continue?", "Shader Change", guiMessageBox::e_YesNo );
			//if ( retval == guiMessageBox::e_Yes )
			//{
				matShaderEffect* eff = NULL;
				try
				{
					// If its a machstudio shader, only write out the name
					if ( matShaderMgr::IsMachStudioShader( new_shader ) )
					{
						new_shader = new_shader_filename;
					} 
					eff = matShaderMgr::GetEffect(new_shader);
				}
				catch(const g3dShaderLoadX& /*ex*/)
				{
					guiMessageBox::Show( "Shader could not be loaded. See debug.log for compilation error.", "Shader Change", guiMessageBox::e_OK );
					eff = NULL;
				}
				if (eff)
				{
					if( i_bDirty )
						mtrlOperations::SetChunkDataChanged();

					shared_ptr<effShaderParams> params(new effShaderParams());

					params->SetShaderName(new_shader, eff);
					eff->BuildPrtyObject(params.get());

					// find and set matching param settings by name.
					params->SetMatchingParams(m_pMaterial->GetShaderParams(layer_index).get());

					// selection time is where we determine if the shader uses dynamic reflection!
					// this will trigger a hidden property that will get saved with the material.
					m_Data.SetHasReflection(eff->HasReflectionMap());

					mdlMaterialInfo new_material(m_Data);
					new_material.SetShaderParams(params, layer_index);
					//new_material.SetShader(shaderTable[new_shader_index].m_Name, shaderTable[new_shader_index].m_DataTemplate);

					const bool bUpdateProperties = true; 
					bool bDoUndoOp = i_bDirty;
					if (layer_index == 0)
					{
						mtrlOperations::ChangeMaterialData(this, new_material, bUpdateProperties, bDoUndoOp);
					}
					else
					{
						mtrlOperations::ChangeMaterialLayerData(this, layer_index, new_material, bUpdateProperties, bDoUndoOp);
					}

					// If we were to call the renotify right away, it would delete the
					// control that is causing this callback. Bad things for the stack.
					mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);

					bAbortChange = false;
				}
				else
				{
					// any extra handling of bad shader load here.
				}
			//}
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlPropertyObject::UpdateGlow(prtyProperty *i_pProperty, bool i_bDirty)
{
	// no proxy for this change, so stop the render thread
	gpxRenderControl::ConfirmSingleThread();

	bool bEnabled = m_bEnableGlow.GetValue();
	if( i_bDirty )
		mtrlOperations::SetChunkDataChanged();
	// Set value in material and data
	m_Data.SetHasGlow(bEnabled);
	m_pMaterial->SetHasGlow(bEnabled);

	// Adjust the properties that are displayed
	if (bEnabled)
		m_GlobalUI->Add(m_pGlowData->GetList());
	else
		this->remove_properties(m_GlobalUI, m_pGlowData);

	// If we were to call the renotify right away, it would delete the
	// control that is causing this callback. Bad things for the stack.
	mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlPropertyObject::UpdateOutline(prtyProperty *i_pProperty, bool i_bDirty)
{
	// no proxy for this change, so stop the render thread
	gpxRenderControl::ConfirmSingleThread();

	bool bEnabled = m_bEnableOutline.GetValue();
	if( i_bDirty )
		mtrlOperations::SetChunkDataChanged();
	// Set value in material and data
	m_Data.SetHasOutline(bEnabled);
	m_pMaterial->SetHasOutline(bEnabled);

	// Adjust the properties that are displayed
	if( m_Data.IsOutlineVisible() )
	{
		if (bEnabled)	m_GlobalUI->Add(m_pOutlineData->GetList());
		else			remove_properties(m_GlobalUI, m_pOutlineData);
	}

	// If we were to call the renotify right away, it would delete the
	// control that is causing this callback. Bad things for the stack.
	mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlPropertyObject::UpdateReflection(prtyProperty *i_pProperty, bool i_bDirty)
{
	// no proxy for this change, so stop the render thread
	gpxRenderControl::ConfirmSingleThread();

	bool bEnabled = m_bEnableReflection.GetValue();
	if( i_bDirty )
		mtrlOperations::SetChunkDataChanged();
	// Set value in material and data
	m_Data.SetHasReflection(bEnabled);
	m_pMaterial->SetHasReflection(bEnabled);

	// Adjust the properties that are displayed
	if (bEnabled)
		m_GlobalUI->Add(m_pReflectionData->GetList());
	else
		this->remove_properties(m_GlobalUI, m_pReflectionData);

	// If we were to call the renotify right away, it would delete the
	// control that is causing this callback. Bad things for the stack.
	mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlPropertyObject::UpdateDisplacement(prtyProperty *i_pProperty, bool i_bDirty)
{
	// no proxy for this change, so stop the render thread
	gpxRenderControl::ConfirmSingleThread();

	bool bEnabled = m_bEnableDisplacement.GetValue();
	if( i_bDirty )
		mtrlOperations::SetChunkDataChanged();
	// Set value in material and data
	m_Data.SetHasDisplacement(bEnabled);
	m_pMaterial->SetHasDisplacement(bEnabled);

	// Adjust the properties that are displayed
	if (bEnabled)	m_GlobalUI->Add(m_pDisplacementData->GetList());
	else			remove_properties(m_GlobalUI, m_pDisplacementData);

	// If we were to call the renotify right away, it would delete the
	// control that is causing this callback. Bad things for the stack.
	mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void mtrlPropertyObject::UpdateRendermanOverride(prtyProperty *i_pProperty, bool i_bDirty)
{
	// no proxy for this change, so stop the render thread
	gpxRenderControl::ConfirmSingleThread();

	bool bEnabled = m_bEnableRendermanOverride.GetValue();
	if( i_bDirty )
		mtrlOperations::SetChunkDataChanged();
	// Set value in material and data
	m_Data.SetHasRendermanOverride(bEnabled);
	m_pMaterial->SetHasRendermanOverride(bEnabled);

	// Adjust the properties that are displayed
	if (bEnabled)	m_GlobalUI->Add(m_pRendermanOverrideData->GetList());
	else			remove_properties(m_GlobalUI, m_pRendermanOverrideData);

	// If we were to call the renotify right away, it would delete the
	// control that is causing this callback. Bad things for the stack.
	mnmThinkMgr::CallFunctionDelayed(&cmmObjectDialogUtil::UpdateDialog);
}

