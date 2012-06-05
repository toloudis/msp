/********************************************************************************************\
**  mtrlPropertyObject.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef MTRL_PROPERTYOBJECT_HPP
#error mtrlPropertyObject.hpp multiply included
#endif
#define MTRL_PROPERTYOBJECT_HPP

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif 
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif 
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif 

class fsLocator;
class matMaterial;
class mtrlShaderObject;
class mdlMaterialInfo;
class prtyCheckBoxUIInfo;
class prtyFileChooserUIInfo;
class tmlnScriptObject;

//============================================================================
//============================================================================
class mtrlPropertyObject : public prtyObject, public pick3dPickObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlPropertyObject(const std::string& i_Name,
						mdlMaterialInfo& i_Data,
						matMaterial* i_pMaterial,
						pick3dPickObject* i_pParent);

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
					mtrlShaderObject* i_pFurData,
					mtrlShaderObject* i_pGlowData,
					mtrlShaderObject* i_pOutlineData,
					mtrlShaderObject* i_pUVTransform,
					mtrlShaderObject* i_pReflectionData
					);

	//--------------------------------------------------------------------
	// Access to the three property objects per material
	//--------------------------------------------------------------------
	mtrlShaderObject* GetShaderDataObject();
	mtrlShaderObject* GetFurDataObject();
	mtrlShaderObject* GetGlowDataObject();
	mtrlShaderObject* GetOutlineDataObject();
	mtrlShaderObject* GetUVTransformObject();
	mtrlShaderObject* GetReflectionDataObject();

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

	//--------------------------------------------------------------------
	// Get list of used textures in order to support copying them
	// to the material library when exporting.
	//--------------------------------------------------------------------
	void GetTextureList(std::vector<fsLocator>& o_TextureList);

//============================================================================
// pick3dPickObject - virtual function overrides
//============================================================================

	//------------------------------------------------------------------------
	// Get parent object of this object in order to define relationships
	//	between icons and their affected objects.
	//------------------------------------------------------------------------
	virtual pick3dPickObject* GetParentObject() const;

	//------------------------------------------------------------------------
	// Get the name of the object for display when selected
	//------------------------------------------------------------------------
	virtual std::string GetPick3dName() const;

private:
	//----------------------------------------------------------------------------
	// remove the properties of this object from our property UI info list
	//----------------------------------------------------------------------------
	void remove_properties(prtyObject* i_pObject);

	//----------------------------------------------------------------------------
	// property callbacks
	//----------------------------------------------------------------------------
	void UpdateShader(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateFur(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateGlow(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateOutline(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateReflection(prtyProperty *i_pProperty, bool i_bDirty);

	std::string m_Name;
	pick3dPickObject* m_pParent;
	mdlMaterialInfo& m_Data;
	matMaterial* m_pMaterial;

	mtrlShaderObject* m_pShaderData;
	mtrlShaderObject* m_pFurData;
	mtrlShaderObject* m_pGlowData;
	mtrlShaderObject* m_pOutlineData;
	mtrlShaderObject* m_pUVTransform;
	mtrlShaderObject* m_pReflectionData;

	//---------------------------------------------------------------------------
	// Local properties and property UI infos
	//---------------------------------------------------------------------------
	prtyFilePath m_ShaderTyp;
	prtyBoolean m_bEnableGlow;
	prtyBoolean m_bEnableFur;
	prtyBoolean m_bEnableOutline;
	prtyFileChooserUIInfo* m_pShaderFileChooser;
	prtyCheckBoxUIInfo* m_pEnableGlow;
	prtyCheckBoxUIInfo* m_pEnableFur;
	prtyCheckBoxUIInfo* m_pEnableOutline;
	prtyBoolean m_bEnableReflection;

};

//------------------------------------------------------------------------
// Quick access to name given in constructor
//------------------------------------------------------------------------
inline const std::string& mtrlPropertyObject::GetName() const
{
	return m_Name;
}
