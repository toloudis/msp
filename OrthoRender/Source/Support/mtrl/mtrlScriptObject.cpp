/*****************************************************************************
**	mtrlScriptObject.cpp
**
**	This class implements material overriding for script objects
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#include "Support/mtrl/GUI/mtrlShaderUtil.hpp"

#include "Support/mnm/mnmSecurityMgr.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileTxt.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/eff/effReflData.hpp"
#include "Graphics/ent/entEntityTemplate.hpp"
#include "Graphics/g3d/g3dCubicReflectionTargetRenderer.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPlanarReflectionTargetRenderer.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mtr/mtrMaterialSaver.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Graphics/mtr/mtrMaterialUtil.hpp"
#include "Graphics/sc/scObject.hpp"
#include "GraphicsDX9/shdw/shdwCubeMapRendererDX9.hpp"
#include "GraphicsDX9/shdw/shdwPlanarReflectionRendererDX9.hpp"
#include "Tool/api3d/api3dObject.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dObjectGeom.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dTargetRendererMgr.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/tma3d/tma3dViewerMgr.hpp"


#include <list>
#include <map>
#include <sstream>

//
//
namespace
{

	//====================================================================
	//====================================================================
	fsLocator find_texture_path(const fsLocator &i_Locator)
	{
		// just return directory from filename for now
		fsLocator dir = i_Locator;
		dir.Pop();

		fsLocator tex_dir = dir;
		tex_dir.Pop();
		tex_dir.Push("Textures");
		if (fsFileUtil::DirectoryExists(tex_dir))
			return tex_dir;

		return dir;
	}

	fsLocator find_general_texture_path(const fsLocator &i_Locator)
	{
		// just return directory from filename for now
		fsLocator dir = i_Locator;
		dir.Pop();
		fsLocator tex_dir = dir;
		tex_dir.Pop();
		tex_dir.Push("General");
		tex_dir.Push("Textures");
		if (fsFileUtil::DirectoryExists(tex_dir))
			return tex_dir;

		return dir;
	}
	
	//====================================================================
	// construct a display name for the given material data
	//====================================================================
	std::string construct_material_name(mdlMaterialInfo& i_Data, 
										int i_Index)
	{
		if (!i_Data.GetMaterialName().empty())
			return i_Data.GetMaterialName();

		static char local_buffer[256];
		sprintf(local_buffer, "Material #%d", i_Index);

		return local_buffer;
	}

	//====================================================================
	// sort_materials() - Get two material lists to match.
	// This assumes that the template's materials came from the material table,
	// which means they are in alphabetical order.  The i_MatData array is
	// based on the order read from the file, and cannot be changed. So,
	// we resort the template's material list to match the i_MatData list.
	//====================================================================
	void sort_materials(const std::vector< shared_ptr<mdlMaterialInfo> > &i_MatData,
					    entModelTemplate *io_pModelTemplate)
	{
		if (!io_pModelTemplate)
			return;
		if (i_MatData.empty()) 
			return;
		if (i_MatData[0]->GetMaterialName().empty()) 
			return;

		std::map<std::string, int> remap;
		for (int i=0; i<i_MatData.size(); i++)
		{
			//DBG_LOG2("i_MatData %d: %s", i, i_MatData[i].GetMaterialName().c_str());
			remap[i_MatData[i]->GetMaterialName()] = i;

			//int tex_num = i_MatData[i].GetNumTextureLayers();
			//for (int t=0; t<tex_num; t++)
			//{
			//	DBG_LOG1("  Texture: %s", i_MatData[i].GetTextureLayer(t).GetTextureName().c_str());
			//}
			//DBG_LOG1("  Glow Mask: %s", i_MatData[i].GetGlowMask().GetTextureName().c_str());
		}

		std::vector<matMaterial*> mat_list = io_pModelTemplate->GetMaterials();

		std::map<std::string, int>::iterator it = remap.begin();
		for (int index = 0; it != remap.end(); ++it, ++index)
		{
			//DBG_LOG2("Remap Material %d: %d", index, it->second);
			io_pModelTemplate->Materials()[it->second] = mat_list[index];	
		}

	}

	std::list<mtrlScriptObject::RenderTargetInfo*> l_MaterialRenderTargets;

}	// end of namespace


//--------------------------------------------------------------------
// Constructor takes object of which to override materials and
//	also locator of model in order to read/write materials.
//--------------------------------------------------------------------
mtrlScriptObject::mtrlScriptObject(api3dObject* i_pObject,
								   const fsLocator& i_ModelLocator)
: m_pObject(i_pObject),
	m_pParent(NULL),
	m_pModelTemplate(NULL),
	m_ModelLocator(i_ModelLocator),
	m_bTemplateIsSorted(false),
	m_bMaterialsLocked(false),
	m_LastSelectedMaterialIndex(-1)
{
	DBG_ASSERT0(i_pObject, "Null api3dObject.");

	// Setup texture directores
	m_TextureDirectory = find_texture_path(i_ModelLocator);
	m_GeneralTextureDirectory = find_general_texture_path(m_TextureDirectory);

	if (api3dObjectGeom *pGeomObject = dynamic_cast<api3dObjectGeom*>(i_pObject))
	{
		m_pModelTemplate = pGeomObject->GetModelTemplate();
	}
	else if (api3dObjectEntity *pEntityObject = dynamic_cast<api3dObjectEntity*>(i_pObject))
	{
		m_pModelTemplate = pEntityObject->GetModelTemplate();
	}

	InitTargetRenderers();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mtrlScriptObject::~mtrlScriptObject()
{
	// object and template pointers are not owned

	// don't delete channels, they are owned by tmlnScriptObject base class
//	envSTLHelpers::DeleteContainer(m_Controls);
	DestroyTargetRenderers();

	// delete the property object that wrap the materials
	// too late to remove the channels, tmlnScriptObject's
	// destructor has already been called and the channels
	// have been deleted.
	const bool bRemoveChannels = false; 
	clear_properties(bRemoveChannels);
}

//--------------------------------------------------------------------
// Set Parent pointer to use when creating selectable 
// property objects
//--------------------------------------------------------------------
void mtrlScriptObject::SetParent(pick3dPickObject* i_pParent)
{
	m_pParent = i_pParent;
}

//--------------------------------------------------------------------
//	Return locator passed to constructor
//--------------------------------------------------------------------
const fsLocator& mtrlScriptObject::GetModelLocator() const
{
	return m_ModelLocator;
}

//--------------------------------------------------------------------
//	Read the model file and get material data structures out
//	in order to override them.
//--------------------------------------------------------------------
void mtrlScriptObject::GatherMaterials()
{
	// clear out our old property objects before we change the material template vector
	const bool bRemoveChannels = true;
	clear_properties(bRemoveChannels);

	mtrMaterialSaver::Read(m_ModelLocator, m_Materials);
	
	// get two materials lists to match
	if (mtrMaterialSaver::DidReadMaterialTable() && 
		!m_bTemplateIsSorted)
	{
		// If the model file contained a material table, then the
		// materials in the template are now in alphabetical order.
		// We can't change the order of the materials read from 
		// the model locator because we need that order in order
		// to re-write the materials back to the file. So, our
		// only option is to sort the materials in the template.
		sort_materials(this->m_Materials, this->m_pModelTemplate);

		// make sure we only sort the template materials once per object
		m_bTemplateIsSorted = true; 
	}

	// Now we can create property objects for the material templates
	// we have just gathered
	m_PropertyObjects.resize(m_Materials.size());

	// Note that these property objects will keep a reference to the
	// material template info structure, so we can't resize the 
	// material list without breaking those references. Conveniently,
	// this function is the only place where the material list is changed.
	for (int i=0; i<m_Materials.size(); i++)
	{
		// at this point the effShaderParams can be different between the two objects here:
		mdlMaterialInfo &info = *m_Materials[i];
		matMaterial *pMaterial = m_pModelTemplate->Materials()[i];

		// Only use the object's texture directory now, since the whole
		// material library can be used.
		//fsLocator tex_dir = get_texture_dir(info);
		fsLocator tex_dir = m_TextureDirectory;

		m_PropertyObjects[i] = new mtrlPropertyObject( construct_material_name(info, i), 
														info, pMaterial, this->m_pParent );
		m_PropertyObjects[i]->SetReadOnly(m_bMaterialsLocked);

		// CreatePropertyObject will replace the data in pMaterial with the data in info.
		// info may not have textures loaded so let's load them now.
		mtrMaterialUtil::SetMaterialData(*pMaterial, info, *m_pModelTemplate, 
			m_TextureDirectory, m_GeneralTextureDirectory);

		m_PropertyObjects[i]->SetShaders( mtrlShaderUtil::CreatePropertyObject(info, pMaterial, tex_dir, m_pModelTemplate),
										  mtrlShaderUtil::CreateFurObject(info, pMaterial, m_pModelTemplate),
										  mtrlShaderUtil::CreateGlowObject(info, pMaterial, tex_dir, m_pModelTemplate),
										  mtrlShaderUtil::CreateOutlineObject(info, pMaterial, tex_dir, m_pModelTemplate),
										  mtrlShaderUtil::CreateUVTransformObject(info, pMaterial, tex_dir, m_pModelTemplate),
										  mtrlShaderUtil::CreateReflectionObject(info, pMaterial, tex_dir, m_pModelTemplate)
										  );

		tmlnScriptObject *pScriptObject = dynamic_cast<tmlnScriptObject*>(this);
		if (pScriptObject)
		{
			m_PropertyObjects[i]->AddChannels( pScriptObject );
		}
	}
}

//--------------------------------------------------------------------
// OverrideMaterials is used by the user interface. It just calls 
// GatherMaterials; but since it is virtual, it can also set a 
// dirty bit on the chunk and update the object part interface.
//--------------------------------------------------------------------
//virtual 
void mtrlScriptObject::OverrideMaterials()
{
	this->GatherMaterials();
}

//--------------------------------------------------------------------
//	Save materials to given filename. If this is the same
//	filename as the current locator, clears out list of 
//  material overrides.
//--------------------------------------------------------------------
void mtrlScriptObject::SaveMaterials(const fsLocator& i_Locator, 
									const gfFileConstants::gfWriteFormats i_WriteFormat)
{
	//	only perform if the dongle is present
	if (!mnmSecurityMgr::CheckForDongle())
		return;

	mtrMaterialSaver::Write(i_Locator, m_ModelLocator, m_Materials, i_WriteFormat);

	std::string cur_name, save_name;
	fsFileUtil::LocatorToANSIFilename(m_ModelLocator, cur_name);
	DBG_LOG1("Current name: %s", cur_name.c_str());
	fsFileUtil::LocatorToANSIFilename(i_Locator, save_name);
	DBG_LOG1("Saved name  : %s", save_name.c_str());

	//if (i_Locator != m_ModelLocator)
	if (_stricmp(save_name.c_str(), cur_name.c_str()))
	{
		DBG_LOG0("Saved to new name, not clearing out materials");

		//Note: I am no longer changing the locator with this function.
		// As currently defined, this will save to a new filename, but will
		// not change anything about the existing object.

		//m_ModelLocator = i_Locator;

		// Call virtual function so that derived classes can update their
		//	locators.
		//this->NotifyLocatorChanged(i_Locator);
	}
	else if (!this->HasMaterialAnimation()) // Can't clear materials when animated
	{
		// clear the property objects
		const bool bRemoveChannels = true; 
		clear_properties(bRemoveChannels);

		// If saving to current filename, clear out list of overrides
		m_Materials.clear();
	}
}


//--------------------------------------------------------------------
//	ImportMaterials - read materials from file and apply
//	to current geometry by matching names
//--------------------------------------------------------------------
int	mtrlScriptObject::ImportMaterials(const fsLocator &i_Locator)
{
	std::vector< shared_ptr<mdlMaterialInfo> > materials;
	mtrMaterialSaver::Read( i_Locator, materials );

	// Check to see if materials are named
	int num_mats = GetNumMaterials();
	bool bCanUseNames = true;
	int i;
	for (i=0; i<num_mats; i++)
	{
		if (m_Materials[i]->GetMaterialName().empty())
			bCanUseNames = false;
	}

	// If not named and number of materials don't match
	// can't do anything.
	if (!bCanUseNames && (num_mats != materials.size()))
		return 0;

	// load material names from current model into 
	// set of strings
	//
	std::map<std::string, int> name_map;
	for (i=0; i<num_mats; i++)
	{
		std::string name = GetMaterialName(i);
		name_map[name] = i;
	}

	// Find if material names from given list exist in name map
	//
	int count = 0;
	for (i=0; i<materials.size(); i++)
	{
		std::string name = materials[i]->GetMaterialName();
		if (bCanUseNames)
		{
			std::map<std::string, int>::iterator it = name_map.find(name);
			if (it != name_map.end())
			{
				// if found, set material data
				const bool bUpdateProperties = true; 
				ChangeMaterialData(it->second, *materials[i], bUpdateProperties);
				count++;
			}
		}
		else
		{
			// Use material index as match
			const bool bUpdateProperties = true; 
			ChangeMaterialData(i, *materials[i], bUpdateProperties);
			count++;
		}
	}

	return count;
}


//--------------------------------------------------------------------
//	Returns number of materials. May return 0 if no materials are
//	overriden (if GatherMaterials has not been called).
//--------------------------------------------------------------------
int mtrlScriptObject::GetNumMaterials() const
{
	return m_Materials.size();
}

//--------------------------------------------------------------------
//	Return name of Material with given index
//--------------------------------------------------------------------
const char* mtrlScriptObject::GetMaterialName(int i_Index) const
{
	DBG_ASSERT0(i_Index<m_PropertyObjects.size(), "Material index out of range");
	return m_PropertyObjects[i_Index]->GetName().c_str();
}

//--------------------------------------------------------------------
//	Return names of all Materials at in one array
//--------------------------------------------------------------------
void  mtrlScriptObject::GetMaterialNames(std::vector<std::string> &o_Names) const
{
	int num_mats = m_Materials.size();
	o_Names.resize(num_mats);
	for (int i=0; i<num_mats; ++i)
	{
		o_Names[i] = m_PropertyObjects[i]->GetName();
	}
}

//--------------------------------------------------------------------
// Return index for material with given name. Returns -1
//	if nto found.
//--------------------------------------------------------------------
int mtrlScriptObject::GetIndexForName(const std::string &i_Name) const
{
	for (int i=0; i<m_Materials.size(); i++)
	{
		//if (i_Name == m_Materials[i].GetMaterialName())
		if (i_Name == m_PropertyObjects[i]->GetName()) // handles generated display name also
			return i;
	}
	return -1;
}

//--------------------------------------------------------------------
//	Return Material properties structure
//--------------------------------------------------------------------
const mdlMaterialInfo& mtrlScriptObject::GetMaterialData(int i_Index) const
{
	DBG_ASSERT0(i_Index<m_Materials.size(), "Material index out of range");

	return *m_Materials[i_Index];
}

//--------------------------------------------------------------------
// Get vector of materials in order to store info to file
//--------------------------------------------------------------------
void mtrlScriptObject::GetMaterialData(std::vector< shared_ptr<mdlMaterialInfo> > &o_Data) const
{
	o_Data = m_Materials;
}

//--------------------------------------------------------------------
// Set vector of materials from data from file. This uses the name
//	of the materials to match up data in order to handle cases where
//	the model file has changed since the materials were last 
//	overriden.
//--------------------------------------------------------------------
void mtrlScriptObject::SetMaterialData(const std::vector< shared_ptr<mdlMaterialInfo> > &i_Data)
{
	if (i_Data.empty()) return;

	// There could be an efficiency if we could pass these materials to the
	// mayImporter when loading the materials. But, for now, we need to re-read the
	// geometry file in order to know how many materials and of what name are in 
	//the model file in order to match them up correctly.
	if (m_Materials.empty())
	{
		this->GatherMaterials();
	}

	// Now go through each each material passed in and try to find the material by name
	// that matches
	for (int i=0; i<i_Data.size(); ++i)
	{
		std::string mat_name = i_Data[i]->GetMaterialName();
		if (mat_name.empty())
		{
			DBG_WARNING0("Material is not named, cannot set values.");
		}
		else
		{
			bool bFound = false;
			for (int j=0; j<m_Materials.size(); ++j)
			{
				if (m_Materials[j]->GetMaterialName() == mat_name)
				{
					bFound = true;
					const bool bUpdateProperties = true; 
					ChangeMaterialData(j, *i_Data[i], bUpdateProperties);
					break;
				}
			}

			if (!bFound)
			{
				DBG_WARNING1("Could not find material named, %s", mat_name.c_str());
			}
		}
	}
}

//--------------------------------------------------------------------
//	Change the material at a given index, using the material
//	data structure.
//--------------------------------------------------------------------
void mtrlScriptObject::ChangeMaterialData(int i_Index, 
										  const mdlMaterialInfo& i_Data,
										  bool i_bUpdateProperties)
{
	DBG_ASSERT0(i_Index<m_Materials.size(), "Material index out of range");

	if (m_pModelTemplate)
	{
		// set material info in fragments' materials
		//bool bTextureChanged = mtrMaterialUtil::DidTextureChange(m_Materials[i_Index], i_Data);
		DBG_ASSERT0(i_Index<m_pModelTemplate->Materials().size(), "Material index out of template range");

		matMaterial *pMaterial = m_pModelTemplate->Materials()[i_Index];
		mtrMaterialUtil::SetMaterialData(*pMaterial, i_Data, *m_pModelTemplate, 
			m_TextureDirectory, m_GeneralTextureDirectory);

		UpdateTargetRenderer(i_Index);
	}

	// preserve the material name
	std::string mat_name = m_Materials[i_Index]->GetMaterialName();

	// set local data structure
	(*m_Materials[i_Index]) = i_Data;

	// put old name back
	m_Materials[i_Index]->SetMaterialName(mat_name);

	// Create new property objects when needed
	if (i_bUpdateProperties)
	{
		DBG_ASSERT0(!HasMaterialAnimation(i_Index), "Need to remove material animation before deleting channels.");

		mtrlPropertyObject *pPropertyObject = m_PropertyObjects[i_Index];

		tmlnScriptObject *pScriptObject = dynamic_cast<tmlnScriptObject*>(this);
		if (pScriptObject)
		{
			// Disconnect the channels from this script object
			pPropertyObject->RemoveChannels(pScriptObject);
		}

		mdlMaterialInfo &info = (*m_Materials[i_Index]);
		matMaterial *pMaterial = m_pModelTemplate->Materials()[i_Index];

		// Only use the object's texture directory now, since the whole
		// material library can be used.
		//fsLocator tex_dir = get_texture_dir(info);
		fsLocator tex_dir = m_TextureDirectory;

		pPropertyObject->SetShaders(mtrlShaderUtil::CreatePropertyObject(info, pMaterial, tex_dir, m_pModelTemplate),
									mtrlShaderUtil::CreateFurObject(info, pMaterial, m_pModelTemplate),
									mtrlShaderUtil::CreateGlowObject(info, pMaterial, tex_dir, m_pModelTemplate),
									mtrlShaderUtil::CreateOutlineObject(info, pMaterial, tex_dir, m_pModelTemplate),
									mtrlShaderUtil::CreateUVTransformObject(info, pMaterial, tex_dir, m_pModelTemplate),
									mtrlShaderUtil::CreateReflectionObject(info, pMaterial, tex_dir, m_pModelTemplate)
									);
		if (pScriptObject)
		{
			pPropertyObject->AddChannels( pScriptObject );
		}

		// Have to reselect in order to update user interface about our new material shaders
		//if (sel3dMgr::GetSelected() == pPropertyObject)
		//{
		//	sel3dMgr::Renotify();
		//}
	}
}

//--------------------------------------------------------------------
// Set relative path to the library file for the material
// with the given index. This will not re-load the textures,
//use ChangeMaterialData if you want that behavior.
//--------------------------------------------------------------------
void mtrlScriptObject::SetLibraryFilename( int i_Index, 
										   const fsLocator& i_RelativePath )
{
	DBG_ASSERT0(i_Index<m_Materials.size(), "Material index out of range");
	m_Materials[i_Index]->SetLibraryFilename(i_RelativePath);

	//bga - This has changed since the whole material library can be
	// used for textures now.  Just leave the objects' texture directory
	// as the default directory to look in first.
	//// Pass on the new location of the textures to the property objects
	//// so that they can update the user interface
	//fsLocator tex_dir(gfPaths::e_MaterialLibrary);
	//tex_dir.Push( i_RelativePath );
	//tex_dir.Pop();  // take off the .mtl filename

	//mtrlPropertyObject *object = m_PropertyObjects[i_Index];
	//object->UpdateTextureDirectory( tex_dir );
}

//--------------------------------------------------------------------
// Returns true if material with given index is a library material
//--------------------------------------------------------------------
bool mtrlScriptObject::IsLibraryMaterial( int i_Index ) const
{
	DBG_ASSERT0(i_Index<m_Materials.size(), "Material index out of range");
	return m_Materials[i_Index]->UsesMaterialLibrary();
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
void mtrlScriptObject::GetTextureList(int i_Index, std::vector<fsLocator>& o_TextureList)
{
	DBG_ASSERT2(i_Index<m_PropertyObjects.size(), "Material index %d out of property object range %d", i_Index, m_PropertyObjects.size());
	mtrlPropertyObject *object = m_PropertyObjects[i_Index];
	object->GetTextureList(o_TextureList);
}

//--------------------------------------------------------------------
// Let the modeltemplate know that a texture is being changed.
//--------------------------------------------------------------------
void mtrlScriptObject::ReplaceTexture(int i_Index, matTexture* i_pOldTexture, matTexture* i_pNewTexture)
{
	DBG_ASSERT0(i_Index<m_Materials.size(), "Material index out of range");
	if (m_pModelTemplate)
	{
		DBG_ASSERT0(i_Index<m_pModelTemplate->Materials().size(), "Material index out of template range");
		if (i_pOldTexture != NULL)
			envSTLHelpers::RemoveOneValue(m_pModelTemplate->Textures(), i_pOldTexture);
		if (i_pNewTexture != NULL)
			m_pModelTemplate->Textures().push_back(i_pNewTexture);
	}
}

//--------------------------------------------------------------------
//  Get a list of resources used by materials.  
//  The resources will be appended to the passed in list.
//--------------------------------------------------------------------
void mtrlScriptObject::GetMaterialResources( fsResourceTrackerData& io_List )
{
	std::vector<fsLocator> texture_list;
	for (int i=0; i<m_Materials.size(); ++i)
	{
		this->GetTextureList( i, texture_list );
	}
	
	std::sort( texture_list.begin(), texture_list.end() );
	std::unique( texture_list.begin(), texture_list.end() );

	std::vector<fsLocator>::iterator it;
	for (it = texture_list.begin(); it != texture_list.end(); ++it)
		io_List.Add( *it );

}

//--------------------------------------------------------------------
// Access to the three property objects per material
//--------------------------------------------------------------------
mtrlShaderObject* mtrlScriptObject::GetShaderDataObject(int i_Index)
{		
	DBG_ASSERT2(i_Index<m_PropertyObjects.size(), "Material index %d out of property object range %d", i_Index, m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetShaderDataObject();
}
mtrlShaderObject* mtrlScriptObject::GetFurDataObject(int i_Index)
{
	DBG_ASSERT2(i_Index<m_PropertyObjects.size(), "Material index %d out of property object range %d", i_Index, m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetFurDataObject();
}
mtrlShaderObject* mtrlScriptObject::GetGlowDataObject(int i_Index)
{
	DBG_ASSERT2(i_Index<m_PropertyObjects.size(), "Material index %d out of property object range %d", i_Index, m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetGlowDataObject();
}
mtrlShaderObject* mtrlScriptObject::GetOutlineDataObject(int i_Index)
{
	DBG_ASSERT2(i_Index<m_PropertyObjects.size(), "Material index %d out of property object range %d", i_Index, m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetOutlineDataObject();
}
mtrlShaderObject* mtrlScriptObject::GetUVTransformDataObject(int i_Index)
{
	DBG_ASSERT2(i_Index<m_PropertyObjects.size(), "Material index %d out of property object range %d", i_Index, m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetUVTransformObject();
}
mtrlShaderObject* mtrlScriptObject::GetReflectionDataObject(int i_Index)
{
	DBG_ASSERT2(i_Index<m_PropertyObjects.size(), "Material index %d out of property object range %d", i_Index, m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetReflectionDataObject();
}

//--------------------------------------------------------------------
//	Return Material ui properties
//--------------------------------------------------------------------
mtrlPropertyObject* mtrlScriptObject::GetMaterialUI(int i_Index) const
{	
	DBG_ASSERT2(i_Index<m_PropertyObjects.size(), "Material index %d out of property object range %d", i_Index, m_PropertyObjects.size());
	return m_PropertyObjects[i_Index];
}
mtrlPropertyObject* mtrlScriptObject::GetMaterialUI(const std::string& i_MaterialName) const
{
	int index = GetIndexForName(i_MaterialName);
	if (index >= 0)
	{
		return m_PropertyObjects[index];
	}

	return NULL;
}

//--------------------------------------------------------------------
// Returns true if there are some drivers on the channels
// for the material.
//--------------------------------------------------------------------
bool mtrlScriptObject::HasMaterialAnimation() const
{
	std::vector<mtrlPropertyObject*>::const_iterator it;
	for (it=m_PropertyObjects.begin(); it != m_PropertyObjects.end(); ++it)
	{
		if ((*it)->HasMaterialAnimation())
			return true;
	}
	return false;
}
bool mtrlScriptObject::HasMaterialAnimation(int i_Index) const
{
	const mtrlPropertyObject* object = m_PropertyObjects[i_Index];
	if (object->HasMaterialAnimation())
		return true;
	
	return false;
}

//--------------------------------------------------------------------
//	Lock the materials
//--------------------------------------------------------------------
bool mtrlScriptObject::IsLockMaterials() const
{
	return m_bMaterialsLocked;
}

void mtrlScriptObject::LockMaterials(const bool i_bLock)
{
	m_bMaterialsLocked = i_bLock;
	
	// Set the read-only flag on the material property objects so that
	// its properties controls are disabled
	std::vector<mtrlPropertyObject*>::const_iterator it;
	for (it=m_PropertyObjects.begin(); it != m_PropertyObjects.end(); ++it)
	{
		(*it)->SetReadOnly(m_bMaterialsLocked);
	}
}

//--------------------------------------------------------------------
//	Collect all fragments contained by this object for material editing
//--------------------------------------------------------------------
void mtrlScriptObject::GatherFragments(std::vector<g3dFragment*> &o_Fragments) const
{
	if (api3dObjectSingle* pObject = dynamic_cast<api3dObjectSingle*>(m_pObject))
	{
		mtrMaterialUtil::GatherFragments(pObject->Object(), *m_pModelTemplate, o_Fragments);
	}
}

//--------------------------------------------------------------------
// Get material index by looking for the node with the given 
//	pick code.
//--------------------------------------------------------------------
int mtrlScriptObject::GetMaterialIndexFromPickCode(envType::UInt32 i_PickCode) const
{
	const g3dSceneNode *pPickedNode = m_pObject->GetPickedNode(i_PickCode);
	if (pPickedNode)
	{
		const matMaterial *pMaterial = pPickedNode->GetMaterial();
		if (!pMaterial && pPickedNode->GetFragment())
			pMaterial = pPickedNode->GetFragment()->GetMaterial();

		if (pMaterial && m_pModelTemplate)
		{
			for (int i=0; i<m_pModelTemplate->Materials().size(); ++i)
			{
				if (pMaterial == m_pModelTemplate->Materials()[i])
					return i;
			}
		}
	}
	return -1;
}

//--------------------------------------------------------------------
//	Store selected index in order to maintain it when re-selecting
//--------------------------------------------------------------------
void mtrlScriptObject::SetLastSelectedMaterialIndex(int i_Index)
{
	m_LastSelectedMaterialIndex = i_Index;
}
int mtrlScriptObject::GetLastSelectedMaterialIndex() const
{
	return m_LastSelectedMaterialIndex;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrlScriptObject::HighlightMaterial(int i_Index, matMaterial* i_MatHilight, bool i_On)
{
	
	if (api3dObjectSingle* pObject = dynamic_cast<api3dObjectSingle*>(m_pObject))
	{
		matMaterial* pExisting = NULL;
		matMaterial* pReplace = NULL;
		if (i_On)
		{
			// replace existing with highlight
			pExisting = m_pModelTemplate->Materials()[i_Index];
			pReplace = i_MatHilight;
		}
		else
		{
			// replace highlight with original
			pExisting = i_MatHilight;
			pReplace = m_pModelTemplate->Materials()[i_Index];
		}

		std::vector<g3dFragment*> fragments;
		mtrMaterialUtil::GatherFragments(pObject->Object(), *m_pModelTemplate, fragments);

		for (int i = 0; i < fragments.size(); i++)
		{
			if (fragments[i]->GetMaterial() == pExisting)
			{
				fragments[i]->SetMaterial(pReplace);
			}
		}
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mtrlScriptObject::RenderTargetInfo::RenderTargetInfo()
:	m_Renderer(NULL),m_TargetRenderer(NULL),m_pMaterial(NULL),
	m_pParentObject(NULL),m_pRenderTarget(NULL), m_bIsCubeMap(false)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mtrlScriptObject::RenderTargetInfo::~RenderTargetInfo()
{
	// try to remove it from api3d. if it's not there, it must be in tma3d.
	if (!api3dTargetRendererMgr::RemoveTargetRenderer(m_TargetRenderer))
		tma3dViewerMgr::RemoveTargetRendererFromViewers(m_TargetRenderer);

	delete m_TargetRenderer;
	delete m_Renderer;
	matTextureMgr::ReleaseTexture(m_pRenderTarget);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrlScriptObject::InitTargetRenderers()
{
	std::vector<matMaterial*>& materials = m_pModelTemplate->Materials();
	for (int i = 0; i < materials.size(); i++)
	{
		UpdateTargetRenderer(i);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrlScriptObject::DestroyTargetRenderers()
{
	std::map<matMaterial*, RenderTargetInfo*>::iterator iter;
	for (iter=m_MaterialRenderTargets.begin(); iter != m_MaterialRenderTargets.end(); iter++)
	{
		envSTLHelpers::RemoveOneValue(l_MaterialRenderTargets, iter->second);
		delete iter->second;
	}
}

//--------------------------------------------------------------------
//	Reconstruct a particular material target renderer for this object.
//--------------------------------------------------------------------
void mtrlScriptObject::UpdateTargetRenderer(int i_Index)
{
	DBG_ASSERT0(i_Index<m_pModelTemplate->Materials().size(), "Material index out of range");
	matMaterial *pMaterial = m_pModelTemplate->Materials()[i_Index];

	// find existing target renderer (if there is one)
	std::map<matMaterial*, RenderTargetInfo*>::iterator iter;
	iter = m_MaterialRenderTargets.find(pMaterial);

	// pull it out of the list if it exists.
	// then we edit it and re-add it to the list, or delete it if it is invalid.
	RenderTargetInfo* pRenderTargetInfo = NULL;
	bool found = false;
	if (iter != m_MaterialRenderTargets.end())
	{
		pRenderTargetInfo = iter->second;
		found = true;
		m_MaterialRenderTargets.erase(iter);
		envSTLHelpers::RemoveOneValue(l_MaterialRenderTargets, iter->second);
	}

	effReflectionMap* pData = &pMaterial->ReflectionData();
	if (pMaterial->GetHasReflection() && (pData != NULL))
	{
		// this is critical to get the reflection to show up in the shader!
		if (found)
			pData->m_ReflectionMap = pRenderTargetInfo->m_pRenderTarget;

		// do we need a new rendertarget?
		// - did the resolution change?
		// - did the planar/cubic state change?
		// - was the old rendertarget null?
		// - did we just switch HDR-LDR?
		if ((pRenderTargetInfo == NULL) ||
			(pRenderTargetInfo->m_pRenderTarget->GetWidth() != pData->m_ReflMapResolution) ||
			(pRenderTargetInfo->m_bIsCubeMap == pData->m_bIsPlanar)
			)
		{
			// remove old target texture if there is one.
			if (pRenderTargetInfo != NULL)
			{
				matTextureMgr::ReleaseTexture(pRenderTargetInfo->m_pRenderTarget);
			}

			// create new render target texture.

			matTexture* pTargetTexture = NULL;
			g2dPFD* image_format = NULL;
			// TEMPORARILY use default pixel format for render targets
			// FIX TO GET HDR WORKING. We should be able to render reflections to HDR render target.

			//	g2dPFD hdrPFD(g2dPFD::e_RGBA16f, 16*4);
			//	if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererCreate::e_HDR)
			//		image_format = &hdrPFD;
			if (pData->m_bIsPlanar)
			{
				pTargetTexture = matTextureMgr::CreateRenderTargetTexture( pData->m_ReflMapResolution, pData->m_ReflMapResolution, false, image_format, true );
			}
			else
			{
				pTargetTexture = matTextureMgr::CreateCubeRenderTargetTexture( pData->m_ReflMapResolution, pData->m_ReflMapResolution, image_format );
			}
			pData->m_ReflectionMap = pTargetTexture;
			// the map had better be a render target.
			g2dRenderTarget* pTarget = pTargetTexture->GetRenderTargetAPI();
			DBG_ASSERT0(pTarget, "Reflection map is not a valid render target");

			// get the scene object for the renderer
			scObject* pSceneObject = NULL;
			if (api3dObjectGeom *pGeomObject = dynamic_cast<api3dObjectGeom*>(m_pObject))
				pSceneObject = pGeomObject->Object();
			else if (api3dObjectEntity *pEntityObject = dynamic_cast<api3dObjectEntity*>(m_pObject))
				pSceneObject = pEntityObject->Object();

			// create a new renderer
			g3dSceneRenderer* reflMapRenderer = NULL;
			if (pData->m_bIsPlanar)
			{
				// create a planar reflection renderer
				reflMapRenderer = g3dSceneRendererCreate::CreatePlanarReflectionRenderer();
				((shdwPlanarReflectionRendererDX9*)reflMapRenderer)->SetSceneObject(pSceneObject, pMaterial);
			}
			else
			{
				// create a cube map scene renderer
				reflMapRenderer = g3dSceneRendererCreate::CreateCubeMapRenderer();
				((shdwCubeMapRendererDX9*)reflMapRenderer)->SetSceneObject(pSceneObject, pMaterial);
				((shdwCubeMapRendererDX9*)reflMapRenderer)->SetPerFrameRendering(pData->m_bAutoGenEnvMap);
			}

			// remove the old renderers and discard them
			if (found)
			{
				// try to remove it from api3d. if it's not there, it must be in tma3d.
				if (!api3dTargetRendererMgr::RemoveTargetRenderer(pRenderTargetInfo->m_TargetRenderer))
					tma3dViewerMgr::RemoveTargetRendererFromViewers(pRenderTargetInfo->m_TargetRenderer);
				delete pRenderTargetInfo->m_TargetRenderer;
				delete pRenderTargetInfo->m_Renderer;
			}

			// create a target renderer
			g3dTargetRenderer* reflTargetRenderer = NULL;
			if (pData->m_bIsPlanar)
			{
				reflTargetRenderer = new g3dPlanarReflectionTargetRenderer(pTarget, 
					reflMapRenderer, api3dScene::GetScene(), NULL);
			}
			else
			{
				reflTargetRenderer = new g3dCubicReflectionTargetRenderer(pTarget, 
					reflMapRenderer, api3dScene::GetScene(), NULL);
			}
			reflTargetRenderer->SetBackgroundColor(g2dRGBColor(0,0,0));

			if (pRenderTargetInfo == NULL)
				pRenderTargetInfo = new RenderTargetInfo();
			pRenderTargetInfo->m_pParentObject = this;
			pRenderTargetInfo->m_pMaterial = pMaterial;
			pRenderTargetInfo->m_Renderer = reflMapRenderer;
			pRenderTargetInfo->m_TargetRenderer = reflTargetRenderer;
			pRenderTargetInfo->m_pRenderTarget = pTargetTexture;
			pRenderTargetInfo->m_bIsCubeMap = !pData->m_bIsPlanar;

			reflTargetRenderer->SetCamera(&cam3dMgr::GetCamera());
			//reflTargetRenderer->SetCamera(&pRenderTargetInfo->m_Camera);
			if (pData->m_bIsPlanar)
				tma3dViewerMgr::AddTargetRendererToViewers(reflTargetRenderer);
			else
				api3dTargetRendererMgr::AddTargetRenderer(reflTargetRenderer, api3dTargetRendererMgr::e_Reflection);
		}

		m_MaterialRenderTargets[pMaterial] = pRenderTargetInfo;
		l_MaterialRenderTargets.push_back(pRenderTargetInfo);
	}
	else
	{
		if (pData != NULL)
		{
			pData->m_ReflectionMap = NULL;
		}
		if (found)
		{
			delete pRenderTargetInfo;
		}
	}
}

//--------------------------------------------------------------------
//	Pass in a new Viewer that needs to know what reflection targets to render.
//--------------------------------------------------------------------
void mtrlScriptObject::AddTargetsToViewer(g3dViewer* i_pViewer)
{
	std::list<mtrlScriptObject::RenderTargetInfo*>::iterator i;
	for (i=l_MaterialRenderTargets.begin(); i != l_MaterialRenderTargets.end(); i++)
	{
		// add target renderer if planar refl map.
		if (!(*i)->m_bIsCubeMap)
			i_pViewer->AddTargetRenderer((*i)->m_TargetRenderer);
	}
}


//--------------------------------------------------------------------
//	Update reflection targets due to HDR/LDR renderer change 
//	(to allow the underlying render targets to change format)
//--------------------------------------------------------------------
void mtrlScriptObject::RecreateRenderTargets()
{
	std::list<mtrlScriptObject*> objects;

	std::list<mtrlScriptObject*>::iterator j;
	for (j = objects.begin(); j != objects.end(); j++)
	{
		(*j)->DestroyTargetRenderers();
		(*j)->InitTargetRenderers();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrlScriptObject::clear_properties(bool i_bRemoveChannels)
{
	tmlnScriptObject *pScriptObject = dynamic_cast<tmlnScriptObject*>(this);
	if (pScriptObject)
	{
		std::vector<mtrlPropertyObject*>::iterator it;
		for (it=m_PropertyObjects.begin(); it != m_PropertyObjects.end(); ++it)
		{
			if (i_bRemoveChannels)
				(*it)->RemoveChannels(pScriptObject);
		}
	}
	envSTLHelpers::DeleteContainer(	m_PropertyObjects );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
fsLocator mtrlScriptObject::get_texture_dir(const mdlMaterialInfo &i_Info)
{
	fsLocator tex_dir = m_TextureDirectory;
	if (i_Info.UsesMaterialLibrary())
	{
		tex_dir = gfPaths::GetPath(gfPaths::e_MaterialLibrary);
		tex_dir.Push( i_Info.GetLibraryFilename() );
		tex_dir.Pop();
	}
	return tex_dir;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrlScriptObject::ReportMemory(gfFileTxt& i_File)
{
	std::vector<matTexture*> textures;
	unsigned int texMem = 0;
	if (m_pModelTemplate != NULL)
	{
		for (int i = 0; i < m_pModelTemplate->GetMaterials().size(); i++)
		{
			// get the textures of this material
			matMaterial* mat = m_pModelTemplate->Materials()[i];
			effShaderData* effData = mat->GetEffectData();
			std::vector<matTexture*> thisMatTextures;
			effData->GetTextures(thisMatTextures);

			// add one by one to the big list, accounting for shared textures.
			for (int j = 0; j < thisMatTextures.size(); j++)
			{
				if (envSTLHelpers::Contains(textures, thisMatTextures[j]))
					continue;
				textures.push_back(thisMatTextures[j]);
			}
		}
	}

	// now add up all the sizes
	for (int i  = 0; i < textures.size(); i++)
	{
		texMem += textures[i]->GetSize();
	}

	// add in special render targets too
	std::map<matMaterial*, RenderTargetInfo*>::iterator iter;
	for (iter=m_MaterialRenderTargets.begin(); iter != m_MaterialRenderTargets.end(); iter++)
	{
		texMem += (iter->second)->m_pRenderTarget->GetSize();
	}

    std::ostringstream stm;
	stm << "  " << textures.size() << " textures, size: " << texMem/1024 << " KB\r\n";
	i_File.WriteLine(stm.str());
}

//--------------------------------------------------------------------
// Load a new set of fur textures.
//--------------------------------------------------------------------
void mtrlScriptObject::UpdateFurTextures(int i_Index)
{
	DBG_ASSERT0(i_Index<m_pModelTemplate->Materials().size(), "Material index out of range");
	matMaterial *pMaterial = m_pModelTemplate->Materials()[i_Index];

	// remove old fur textures from modeltemplate
	mtrMaterialUtil::RemoveFurTextures(*pMaterial, *m_pModelTemplate);

	// install new ones
	mtrMaterialUtil::LoadFurTextures(*pMaterial, *m_pModelTemplate,
		pMaterial->FurData().m_EffectData.m_TextureFolder,
		m_TextureDirectory, m_GeneralTextureDirectory);
}
