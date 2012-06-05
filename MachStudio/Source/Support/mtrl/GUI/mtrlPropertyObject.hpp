/********************************************************************************************\
**  mtrlPropertyObject.hpp
**
**
**  StudioGPU
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef MTRL_PROPERTYOBJECT_HPP
#error mtrlPropertyObject.hpp multiply included
#endif
#define MTRL_PROPERTYOBJECT_HPP

#ifndef CMM_SELECTABLEPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmSelectablePropertyObject.hpp"
#endif 
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif 
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif 
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif 

class brshBrushStroke;
class fsLocator;
class matMaterial;
class mtrlShaderObject;
class mdlMaterialInfo;
class prtyCheckBoxUIInfo;
class prtyComboBoxUIInfo;
class prtyFileChooserUIInfo;
class tmlnScriptObject;

//============================================================================
// Special property type for shader type because of the way it alters
// other properties in this property object.
//============================================================================
class mtrlShaderTypeProperty : public prtyFilePath
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	mtrlShaderTypeProperty(const std::string& i_Name);

	//--------------------------------------------------------------------
	// Create an undo operation of the correct type for this
	// property. A reference to this property should be passed in.
	// Ownership passes to the caller.
	//--------------------------------------------------------------------
	virtual undoUndoOperation* CreateUndoOperation(shared_ptr<prtyPropertyReference> i_pPropertyRef);
};

//============================================================================
//============================================================================
class mtrlPropertyObject : public cmmSelectablePropertyObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlPropertyObject(const std::string& i_Name,
						mdlMaterialInfo& i_Data,
						matMaterial* i_pMaterial);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	~mtrlPropertyObject();

	//------------------------------------------------------------------------
	// Quick access to name given in constructor
	//------------------------------------------------------------------------
	inline const std::string&  GetName() const;

	//--------------------------------------------------------------------
	// Set the shader property objects to use for this material
	//--------------------------------------------------------------------
	void SetShaders(mtrlShaderObject* i_pShaderData,
					mtrlShaderObject* i_pGlowData,
					mtrlShaderObject* i_pOutlineData,
					mtrlShaderObject* i_pUVTransform,
					mtrlShaderObject* i_pReflectionData,
					mtrlShaderObject* i_pDisplacementData,
					mtrlShaderObject* i_pNormalsData,					
					mtrlShaderObject* i_pRendermanOverrideData,
					mtrlShaderObject* i_pTextureFilterData
					);

	//--------------------------------------------------------------------
	// Set the shader property objects for a new material layer
	//--------------------------------------------------------------------
	void SetMaterialLayerShaders(int i_LayerIndex,
								 mtrlShaderObject* i_pShaderData,
								 mtrlShaderObject* i_pUVTransform);
	//--------------------------------------------------------------------
	// Set the shader property objects (in-place) for a new material layer.
	//--------------------------------------------------------------------
	void ChangeMaterialLayerShaders(int i_LayerIndex,
		mtrlShaderObject* i_pShaderData,
		mtrlShaderObject* i_pUVTransform);

	//--------------------------------------------------------------------
	// Set shader type for given material layer index
	//--------------------------------------------------------------------
	void SetLayerShaderName(int i_LayerIndex, const fsLocator& i_ShaderType);

	//--------------------------------------------------------------------
	// Access to the three property objects per material
	//--------------------------------------------------------------------
	mtrlShaderObject* GetShaderDataObject();
	mtrlShaderObject* GetGlowDataObject();
	mtrlShaderObject* GetOutlineDataObject();
	mtrlShaderObject* GetUVTransformObject();
	mtrlShaderObject* GetReflectionDataObject();
	mtrlShaderObject* GetDisplacementDataObject();
	mtrlShaderObject* GetNormalsDataObject();
	mtrlShaderObject* GetRendermanOverrideDataObject();
	mtrlShaderObject* GetTextureFilterDataObject();

	//--------------------------------------------------------------------
	// Add channels from shader objects into this script object
	//--------------------------------------------------------------------
	void AddChannels(tmlnScriptObject* i_pScriptObject);

	//--------------------------------------------------------------------
	// Remove channels from this property object
	//--------------------------------------------------------------------
	void RemoveChannels(tmlnScriptObject* i_pScriptObject);

	//------------------------------------------------------------------------
	// Returns true if one of its shaders has a material animation
	//------------------------------------------------------------------------
	bool HasMaterialAnimation() const;

	//------------------------------------------------------------------------
	// Set a new directory for the material in order to find the textures.
	// Called when the material has been exported to the material library.
	//------------------------------------------------------------------------
	void UpdateTextureDirectory(const fsLocator &i_TextureDir);

	//------------------------------------------------------------------------
	// Get list of used textures in order to support copying them
	// to the material library when exporting.
	//------------------------------------------------------------------------
	void GetTextureList(std::vector<fsLocator>& o_TextureList);

	//------------------------------------------------------------------------
	// Set the brush stroke object associated with this material
	//------------------------------------------------------------------------
	void SetBrushStroke(brshBrushStroke* i_pBrushStroke);

	//------------------------------------------------------------------------
	// Get the brush stroke object associated with this material
	//------------------------------------------------------------------------
	brshBrushStroke* GetBrushStroke() const;

	//------------------------------------------------------------------------
	// Get the Base UI for this object
	//------------------------------------------------------------------------
	prtyPropertyUIInfoContainer& GetBaseUI() const;

	//--------------------------------------------------------------------
	// Set the shader name
	//--------------------------------------------------------------------
	void SetShaderName(fsLocator i_ShaderType);

//============================================================================
// sel3dObject - virtual function overrides
//============================================================================

	//------------------------------------------------------------------------
	// Get parent object of this object in order to define relationships
	//	between icons and their affected objects.
	//------------------------------------------------------------------------
	//virtual sel3dObject* GetParentObject() const;

	//------------------------------------------------------------------------
	// Get the name of the object for display when selected
	//------------------------------------------------------------------------
	virtual std::string GetDisplayName() const;

private:
	//----------------------------------------------------------------------------
	// remove the properties of this object from our property UI info list
	//----------------------------------------------------------------------------
	void remove_properties(shared_ptr<prtyPropertyUIInfoContainer>& i_SubCategory,
						   prtyObject* i_pObject);
	
	//----------------------------------------------------------------------------
	// part of the cleanup of the shader objects is the step that removes the
	// subcategories and properties associated with the extra material layers.
	//----------------------------------------------------------------------------
	void remove_material_layers();

	//----------------------------------------------------------------------------
	// removes up all properties from all shader objects from our list,
	// deletes all shader objects
	//----------------------------------------------------------------------------
	void cleanup_shader_objects();

	//----------------------------------------------------------------------------
	// property callbacks
	//----------------------------------------------------------------------------
	void UpdateShader(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateGlow(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateOutline(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateReflection(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateDisplacement(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateRendermanOverride(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureFilter(prtyProperty *i_pProperty, bool i_bDirty);

	std::string m_Name;

	brshBrushStroke* m_pBrushStroke;
	//sel3dObject* m_pParent;
	mdlMaterialInfo& m_Data;
	matMaterial* m_pMaterial;

	mtrlShaderObject* m_pShaderData;
	mtrlShaderObject* m_pUVTransform;
	mtrlShaderObject* m_pGlowData;
	mtrlShaderObject* m_pNormalsData;
	mtrlShaderObject* m_pOutlineData;
	mtrlShaderObject* m_pReflectionData;
	mtrlShaderObject* m_pDisplacementData;
	mtrlShaderObject* m_pRendermanOverrideData;
	mtrlShaderObject* m_pTextureFilterData;

	shared_ptr<prtyPropertyUIInfoContainer> m_BaseLayerUI;
	shared_ptr<prtyPropertyUIInfoContainer> m_GlobalUI;

	//---------------------------------------------------------------------------
	// Local properties and property UI infos
	//---------------------------------------------------------------------------
	//mtrlShaderTypeProperty m_ShaderType;
	prtyFilePath m_ShaderType;

	prtyBoolean m_bEnableGlow;
	prtyBoolean m_bEnableOutline;
	prtyBoolean m_bEnableDisplacement;
	prtyBoolean m_bEnableRendermanOverride;

	shared_ptr<prtyPropertyUIInfo> m_pShaderFilePicker;
	shared_ptr<prtyPropertyUIInfo> m_pEnableGlow;
	shared_ptr<prtyPropertyUIInfo> m_pEnableOutline;
	shared_ptr<prtyPropertyUIInfo> m_pEnableDisplacement;
	shared_ptr<prtyPropertyUIInfo> m_pEnableRendermanOverride;
	prtyBoolean m_bEnableReflection;

	//---------------------------------------------------------------------------
	// Material layers
	//---------------------------------------------------------------------------
	struct sMaterialLayerProperties
	{
		prtyFilePath* m_pLayerShaderType;
		shared_ptr<prtyPropertyUIInfoContainer> m_LayerUI;
		mtrlShaderObject* m_pShaderData;
		mtrlShaderObject* m_pUVTransform;
	};
	std::vector<sMaterialLayerProperties> m_MaterialLayerProperties;

};

//------------------------------------------------------------------------
// Quick access to name given in constructor
//------------------------------------------------------------------------
inline const std::string& mtrlPropertyObject::GetName() const
{
	return m_Name;
}
