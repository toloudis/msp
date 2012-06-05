/*****************************************************************************
**	mtrlScriptObject.cpp
**
**	This class implements material overriding for script objects
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/mtrlScriptObject.hpp"

#include "Support/mnm/mnmSecurityMgr.hpp"
#include "Support/mtrl/GUI/mtrlDialogUtil.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"
#include "Support/mtrl/GUI/mtrlSetOperation.hpp"
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#include "Support/mtrl/GUI/mtrlShaderUtil.hpp"
#include "Support/mtrl/mtrlHighlight.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileTxt.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/rel/relRelationshipMultiple.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Graphics/eff/effReflData.hpp"
#include "Graphics/ent/entModelInstance.hpp"
#include "Graphics/ent/entModelTemplate.hpp"
#include "Graphics/g3d/g3dCubicReflectionTargetRenderer.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPlanarReflectionTargetRenderer.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlMatInfo.hpp"
#include "Graphics/mtr/mtrMaterialSaver.hpp"
#include "Graphics/mtr/mtrMaterialUtil.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Tool/api3d/api3dObject.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dObjectGeom.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dTargetRendererMgr.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/tma3d/tma3dViewerMgr.hpp"

#include <list>
#include <map>
#include <sstream>


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
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
	
	//--------------------------------------------------------------------
	// construct a display name for the given material data
	//--------------------------------------------------------------------
	std::string construct_material_name(mdlMaterialInfo& i_Data, 
										int i_Index)
	{
		if (!i_Data.GetMaterialName().empty())
			return i_Data.GetMaterialName();

		//static char local_buffer[256];
		//sprintf(local_buffer, "Material #%d", i_Index);
		
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss.setf(std::ios::fixed, std::ios::floatfield);
		oss<<"Material #"<<i_Index;
		static std::string local_buffer(oss.str());
				


		return local_buffer;
	}

	/*void set_material_active(bool i_bActive, 
							mtrlPropertyObject* i_PropertyObject,
							shared_ptr<mdlMaterialInfo> i_MaterialInfoPtr)*/
	void set_material_active(bool i_bActive, 
							matMaterial* i_Material,
							entModelInstance *i_pModelInstance,
							shared_ptr<mdlMaterialInfo> i_MaterialInfoPtr,
							fsLocator& i_TextureDirectory,
							fsLocator& i_GeneralTextureDirectory)
	{
		if (i_MaterialInfoPtr->GetActive() != i_bActive)
		{
			i_MaterialInfoPtr->SetActive(i_bActive);
			
			if (i_bActive)
			{
				//reload material texture here
				//mtrMaterialUtil::SetMaterialData(*i_Material, *(i_MaterialInfoPtr.get()), *i_pModelInstance, 
				//		i_TextureDirectory, i_GeneralTextureDirectory, true);
			}
			else
			{
				//release material texture here
				//mtrMaterialUtil::RemoveAllTextures(*i_Material, *i_pModelInstance);
			}
		}
	}

	//--------------------------------------------------------------------
	//	In/DereaseRefCount helps releasing texture when no one is
	//	referencing them
	//--------------------------------------------------------------------
	/*void increase_ref_count(int i_Index, 
							std::vector< int > &io_RefCount,
							mtrlPropertyObject* i_PropertyObject,
							shared_ptr<mdlMaterialInfo> i_MaterialInfoPtr)*/
	void increase_ref_count(int i_Index,
							matMaterial* i_Material,
							entModelInstance *i_pModelInstance,
							shared_ptr<mdlMaterialInfo> i_MaterialInfoPtr,
							fsLocator& i_TextureDirectory,
							fsLocator& i_GeneralTextureDirectory)
	{
		i_MaterialInfoPtr->IncreaseRefCount();

		if (i_MaterialInfoPtr->GetRefCount() > 0)
		{
			//set_material_active(true, i_PropertyObject, i_MaterialInfoPtr);
			set_material_active(true, i_Material, i_pModelInstance, i_MaterialInfoPtr,
				i_TextureDirectory, i_GeneralTextureDirectory);
		}
	}

	//void decrease_ref_count(int i_Index, 
	//						std::vector< int > &io_RefCount,
	//						mtrlPropertyObject* i_PropertyObject,
	//						shared_ptr<mdlMaterialInfo> i_MaterialInfoPtr)
	void decrease_ref_count(int i_Index,
							matMaterial* i_Material,
							entModelInstance *i_pModelInstance,
							shared_ptr<mdlMaterialInfo> i_MaterialInfoPtr,
							fsLocator& i_TextureDirectory,
							fsLocator& i_GeneralTextureDirectory)
	{
		i_MaterialInfoPtr->DecreaseRefCount();

		if (i_MaterialInfoPtr->GetRefCount() == 0)
		{
			//set_material_active(false, i_PropertyObject, i_MaterialInfoPtr);
			set_material_active(false, i_Material, i_pModelInstance, i_MaterialInfoPtr,
				i_TextureDirectory, i_GeneralTextureDirectory);
		}
	}

	//--------------------------------------------------------------------
	//	Collect all fragments contained by this object for material editing
	//--------------------------------------------------------------------
	void gather_fragments(api3dObject* i_pObject,
						  entModelInstance &i_ModelInstance,
						  std::vector< std::vector<g3dSceneNode*> > &o_FragmentNodes,
						  std::vector<mtrlPropertyObject*>& i_PropertyObjects,
						  std::vector< shared_ptr<mdlMaterialInfo> > &i_MaterialsInfo,
						  fsLocator& i_TextureDirectory,
						  fsLocator& i_GeneralTextureDirectory)
	{
		o_FragmentNodes.clear();
		int num_materials = i_ModelInstance.GetMaterials().size();
		o_FragmentNodes.resize(num_materials);
		//io_RefCount.resize(num_materials);

		if (api3dObjectSingle* pObject = dynamic_cast<api3dObjectSingle*>(i_pObject))
		{
			std::vector<g3dSceneNode*> fragments;
			mtrMaterialUtil::GatherFragmentNodes(pObject->Object(), fragments);

			// Now sort the fragments based on material indices
			for (int mi=0; mi<num_materials; mi++)
			{
				matMaterial* pMaterial = i_ModelInstance.GetMaterials()[mi];
				for (int fi=0; fi<fragments.size(); fi++)
				{
					DBG_ASSERT(fragments[fi]->GetFragment(), "mtrMaterialUtil::GatherFragmentNodes returned a node without a fragment");
					if (fragments[fi]->GetFragment()->GetMaterial() == pMaterial)
					{
						o_FragmentNodes[mi].push_back(fragments[fi]);
						/*increase_ref_count(mi, io_RefCount, 
							i_PropertyObjects.size() > mi ? i_PropertyObjects[mi] : NULL,
							i_MaterialsInfo[mi]);*/
						increase_ref_count(mi, pMaterial, 
							&i_ModelInstance, i_MaterialsInfo[mi],
							i_TextureDirectory,
							i_GeneralTextureDirectory);
					}
				}
			}
		}
	}

	//--------------------------------------------------------------------
	// sort_materials() - Get two material lists to match.
	// This assumes that the template's materials came from the material table,
	// which means they are in alphabetical order.  The i_MatData array is
	// based on the order read from the file, and cannot be changed. So,
	// we resort the template's material list to match the i_MatData list.
	//--------------------------------------------------------------------
	void sort_materials(const std::vector< shared_ptr<mdlMaterialInfo> > &i_MatData,
					    entModelInstance *io_pModelInstance)
	{
		if (!io_pModelInstance)
			return;
		if (i_MatData.empty()) 
			return;
		if (i_MatData[0]->GetMaterialName().empty()) 
			return;
		if (i_MatData.size() != io_pModelInstance->Materials().size())
		{
			DBG_WARNING("Number of materials does not match, gathered " << i_MatData.size()  << " vs. loaded " << io_pModelInstance->Materials().size());
			return;
		}

		//std::map<std::string, int> remap;
		//for (int i=0; i<i_MatData.size(); i++)
		//{
		//	//DBG_LOG2("i_MatData %d: %s", i, i_MatData[i].GetMaterialName().c_str());
		//	remap[i_MatData[i]->GetMaterialName()] = i;
		//}		
		std::map<std::string, int> remap;
		for (int i=0; i<io_pModelInstance->GetMaterials().size(); i++)
		{
			// Find the index of materials by name in instances list
			//DBG_LOG2("ModelInstance %d: %s", i, io_pModelInstance->GetMaterials()[i]->GetName().c_str());
			remap[io_pModelInstance->GetMaterials()[i]->GetName()] = i;
		}

		// Makes copy of list in order to resort the instance's copy
		std::vector<matMaterial*> mat_list = io_pModelInstance->GetMaterials();

		//std::map<std::string, int>::iterator it = remap.begin();
		//for (int index = 0; it != remap.end(); ++it, ++index)
		for (int i=0; i<i_MatData.size(); i++)
		{
			//DBG_LOG2("Remap Material %d: %d", index, it->second);
			//io_pModelInstance->Materials()[it->second] = mat_list[index];

			int index = remap[i_MatData[i]->GetMaterialName()];
			//DBG_LOG2("Remap Material %d: %d", i, index);
			io_pModelInstance->Materials()[i] = mat_list[index];	
		}

	}

	std::list<mtrlScriptObject::RenderTargetInfo*> l_MaterialRenderTargets;

	//--------------------------------------------------------------------
	// Ask effect if it supports reflections.
	// This code is to correct a bug where most materials
	// thought they supported reflections when they really don't.
	//--------------------------------------------------------------------
	bool ShaderSupportsReflection(matMaterial* i_pMaterial)
	{
		bool effSupportsRefl = true;
		matShaderEffect* eff = NULL;
		if (i_pMaterial->GetShaderParams())
			eff = i_pMaterial->GetShaderParams()->GetShader();
		if (eff)
			effSupportsRefl = eff->HasReflectionMap();

		return effSupportsRefl;
	}

}	// end of namespace


//--------------------------------------------------------------------
// Constructor takes object of which to override materials and
//	also locator of model in order to read/write materials.
//--------------------------------------------------------------------
mtrlScriptObject::mtrlScriptObject(api3dObject* i_pObject,
								   const fsLocator& i_ModelLocator)
: m_pObject(i_pObject),
	m_pModelInstance(NULL),
	m_pModelTemplate(NULL),
	m_ModelLocator(i_ModelLocator),
	m_bTemplateIsSorted(false),
	m_bMaterialsLocked(false)
	//m_LastSelectedMaterialIndex(-1)
{
	DBG_ASSERT(i_pObject, "Null api3dObject.");

	// Setup texture directores
	m_TextureDirectory = find_texture_path(i_ModelLocator);
	m_GeneralTextureDirectory = find_general_texture_path(m_TextureDirectory);

	if (api3dObjectGeom *pGeomObject = dynamic_cast<api3dObjectGeom*>(i_pObject))
	{
		m_pModelInstance = pGeomObject->GetModelInstance();
		m_pModelTemplate = pGeomObject->GetModelTemplate();
	}
	else if (api3dObjectEntity *pEntityObject = dynamic_cast<api3dObjectEntity*>(i_pObject))
	{
		m_pModelInstance = pEntityObject->GetModelInstance();
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
void mtrlScriptObject::SetParent(relObject &i_Parent)
{
	// Create new multiple relationship connecting the property
	// objects to the new parent object
	//
	m_ParentRelationship.reset(
		new relRelationshipMultiple<mtrlPropertyObject>("Materials", i_Parent, m_PropertyObjects));
	i_Parent.AddRelationship(m_ParentRelationship);
	
	// update fragments if already "gathered".
	for (int i = 0; i < m_PropertyObjects.size(); i++)
	{
		m_PropertyObjects[i]->SetParentRelationship(m_ParentRelationship);
	}
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
void mtrlScriptObject::GatherMaterials( bool i_bLoadShaders, bool i_bLoadTextures)
{
	// clear out our old property objects before we change the material template vector
	const bool bRemoveChannels = true;
	clear_properties(bRemoveChannels);

	// Old technique: re-read the geometry file in order to get material data
	//mtrMaterialSaver::Read(m_ModelLocator, m_Materials);
	// New technique: use the material info that was read into the template
	if (m_pModelTemplate)
	{
		mdlMatInfoTable &mat_table = m_pModelTemplate->MaterialTable();
		mdlMatInfoTable::iterator it;
		for (it = mat_table.begin(); it != mat_table.end(); ++it)
		{
			// Extract out just the material information, not the mapping to material pointer
			shared_ptr<mdlMaterialInfo> material_info(new mdlMaterialInfo(it->second->m_Info));
			this->m_Materials.push_back(material_info);
		}
	}
	
	// get two materials lists to match
	//if (mtrMaterialSaver::DidReadMaterialTable() && 
	//	!m_bTemplateIsSorted)

	if ( i_bLoadShaders )
	{
		// Now go through each each material and try to find the material by name
		// that matches
		for (int i = 0; i < m_Materials.size(); ++i)
		{
			std::string mat_name = m_Materials[i]->GetMaterialName();
			if (mat_name.empty())
			{
				//DBG_WARNING("Material is not named, cannot set shadername values.");
				continue;
			}
			else
			{
				bool bFound = false;
				std::map<std::string, fsLocator>::iterator j = m_ShaderList.find(mat_name);
				if (j != m_ShaderList.end())
				{
					m_Materials[i]->GetShaderParams()->SetShaderName( j->second );
				}
				else
				{
					//DBG_WARNING("Could not find material named, " << mat_name.c_str() << " to set shadername");
				}
			}
		}
	}

	if (!m_bTemplateIsSorted)
	{
		// If the model file contained a material table, then the
		// materials in the template are now in alphabetical order.
		// We can't change the order of the materials read from 
		// the model locator because we need that order in order
		// to re-write the materials back to the file. So, our
		// only option is to sort the materials in the template.
		sort_materials(this->m_Materials, this->m_pModelInstance);

		// make sure we only sort the template materials once per object
		m_bTemplateIsSorted = true; 
	}

	// Now gather up the fragments that use each material
	gather_fragments(m_pObject, *m_pModelInstance, m_FragmentNodes, 
		m_PropertyObjects, m_Materials,
		m_TextureDirectory, m_GeneralTextureDirectory);

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
		matMaterial *pMaterial = m_pModelInstance->Materials()[i];

		// Only use the object's texture directory now, since the whole
		// material library can be used.
		//fsLocator tex_dir = get_texture_dir(info);
		fsLocator tex_dir = m_TextureDirectory;

		m_PropertyObjects[i] = new mtrlPropertyObject( construct_material_name(info, i), 
														info, pMaterial );
		m_PropertyObjects[i]->SetParentRelationship(m_ParentRelationship);
		m_PropertyObjects[i]->SetReadOnly(m_bMaterialsLocked);

		if (i_bLoadTextures)
		{
			// CreatePropertyObject will replace the data in pMaterial with the data in info.
			// info may not have textures loaded so let's load them now.
			mtrMaterialUtil::SetMaterialData(*pMaterial, info, *m_pModelInstance, 
				m_TextureDirectory, m_GeneralTextureDirectory);
			UpdateTargetRenderer(i);
		}

		const int base_layer_index = 0;
		m_PropertyObjects[i]->SetShaders( mtrlShaderUtil::CreatePropertyObject(info, base_layer_index, pMaterial, tex_dir, m_pModelInstance),
										  mtrlShaderUtil::CreateGlowObject(info, pMaterial, tex_dir, m_pModelInstance),
										  mtrlShaderUtil::CreateOutlineObject(info, pMaterial, tex_dir, m_pModelInstance),
										  mtrlShaderUtil::CreateUVTransformObject(info, base_layer_index, pMaterial, tex_dir, m_pModelInstance),
										  mtrlShaderUtil::CreateReflectionObject(info, pMaterial, tex_dir, m_pModelInstance),
										  mtrlShaderUtil::CreateDisplacementObject(info, pMaterial, tex_dir, m_pModelInstance),
										  mtrlShaderUtil::CreateNormalsObject(info, pMaterial, tex_dir, m_pModelInstance),
										  mtrlShaderUtil::CreateRendermanOverrideObject(info, pMaterial, tex_dir, m_pModelInstance),
										  mtrlShaderUtil::CreateTextureFilterObject(info, pMaterial, tex_dir, m_pModelInstance)
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
	//	only perform if the security is present
	if (!mnmSecurityMgr::CheckSecurity2())
		return;

	// Overrides material in an existing geometry file:
	//mtrMaterialSaver::Write(i_Locator, m_ModelLocator, m_Materials, i_WriteFormat);
	
	// Writes materials into a separate file format (.gmb) for saved sets of materials
	// Have to confirm the new extension as .gmb to make sure people don't overwrite 
	// the geometry files anymore (that was the old way of doing it).
	fsLocator gmb_locator = i_Locator;
	gmb_locator.Pop();
	itString filename = i_Locator.GetLastName();
	filename.StripExtension();
	filename += itString(".gmb");
	gmb_locator.Push(filename);
	mtrMaterialSaver::WriteMaterialSet(gmb_locator, m_Materials);

	//std::string cur_name, save_name;
	//fsFileUtil::LocatorToANSIFilename(m_ModelLocator, cur_name);
	//DBG_LOG("Current name: " << cur_name.c_str());
	//fsFileUtil::LocatorToANSIFilename(i_Locator, save_name);
	//DBG_LOG("Saved name  : " << save_name.c_str());

	//bga -  No clearing of materials when saving to material files anymore...
	//if (i_Locator != m_ModelLocator)
	//if (_stricmp(save_name.c_str(), cur_name.c_str()))
	//{
	//	DBG_LOG("Saved to new name, not clearing out materials");

	//	//Note: I am no longer changing the locator with this function.
	//	// As currently defined, this will save to a new filename, but will
	//	// not change anything about the existing object.

	//	//m_ModelLocator = i_Locator;

	//	// Call virtual function so that derived classes can update their
	//	//	locators.
	//	//this->NotifyLocatorChanged(i_Locator);
	//}
	//else if (!this->HasMaterialAnimation()) // Can't clear materials when animated
	//{
	//	// clear the property objects
	//	const bool bRemoveChannels = true; 
	//	clear_properties(bRemoveChannels);

	//	// If saving to current filename, clear out list of overrides
	//	m_Materials.clear();
	//}
}


//--------------------------------------------------------------------
//	ImportMaterials - read materials from file and apply
//	to current geometry by matching names
//--------------------------------------------------------------------
int	mtrlScriptObject::ImportMaterials(const fsLocator &i_Locator)
{
	// This Read function can handle any chunk file format with
	// materials in it - so it works for GXb or GMB files.
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

	// wrap all material sets into one undo operation
	undoUndoMgr::BeginMultipleOperationBlock("Import Materials");

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
				undoUndoMgr::AddOperation(new mtrlSetOperation("Import Materials", this, it->second, 
							*m_Materials[it->second], bUpdateProperties) );

				ChangeMaterialData(it->second, *materials[i], bUpdateProperties);
				count++;
			}
		}
		else
		{
			// Use material index as match
			const bool bUpdateProperties = true; 
			undoUndoMgr::AddOperation(new mtrlSetOperation("Import Materials", this, i, 
						*m_Materials[i], bUpdateProperties) );
			ChangeMaterialData(i, *materials[i], bUpdateProperties);
			count++;
		}
	}

	undoUndoMgr::EndMultipleOperationBlock();

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
	DBG_ASSERT(i_Index<m_PropertyObjects.size(), "Material index out of range");
	return m_PropertyObjects[i_Index]->GetName().c_str();
}

//--------------------------------------------------------------------
//	Return pointer of a material with given index
//--------------------------------------------------------------------
matMaterial* mtrlScriptObject::GetMaterial(int i_Index) const
{
	DBG_ASSERT(i_Index < m_pModelInstance->GetMaterials().size(), "Material index out of range");
	return m_pModelInstance->GetMaterials()[i_Index];
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
// Return index for material with given sceneNode. Returns -1
//	if not found.
//--------------------------------------------------------------------
int mtrlScriptObject::GetIndexForSceneNode(const g3dSceneNode* i_Node) const
{
	for ( int i = 0; i < m_FragmentNodes.size(); i++)
	{
		for ( int j = 0; j < m_FragmentNodes[i].size(); j++)
		{
			if (m_FragmentNodes[i][j] == i_Node)
				return i;
		}
	}

	return -1;
}

//--------------------------------------------------------------------
// Return index for material with given material ptr. Returns -1
//	if not found.
//--------------------------------------------------------------------
int mtrlScriptObject::GetIndexForMaterial(const matMaterial* i_Material) const
{
	for (int i=0; i<m_Materials.size(); i++)
	{
		matMaterial* mat = m_pModelInstance->GetMaterials()[i];
		if (mat == i_Material) // handles generated display name also
			return i;
	}
	return -1;
}

//--------------------------------------------------------------------
//	Return Material properties structure
//--------------------------------------------------------------------
const mdlMaterialInfo& mtrlScriptObject::GetMaterialData(int i_Index) const
{
	DBG_ASSERT(i_Index<m_Materials.size(), "Material index out of range");

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

	for (int i=0; i<i_Data.size(); ++i)
	{
		m_ShaderList[i_Data[i]->GetMaterialName()] = i_Data[i]->GetShaderParams()->GetShaderName();
	}

	if (m_Materials.empty())
	{
		const bool bLoadTextures = false;
		this->GatherMaterials(true, bLoadTextures);
	}

	// Now go through each each material passed in and try to find the material by name
	// that matches
	for (int i=0; i<i_Data.size(); ++i)
	{
		std::string mat_name = i_Data[i]->GetMaterialName();
		if (mat_name.empty())
		{
			DBG_WARNING("Material is not named, cannot set values.");
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
				DBG_WARNING("Could not find material named, " << mat_name.c_str());
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
	DBG_ASSERT(i_Index<m_Materials.size(), "Material index out of range");

	if (m_pModelInstance)
	{
		// set material info in fragments' materials
		//bool bTextureChanged = mtrMaterialUtil::DidTextureChange(m_Materials[i_Index], i_Data);
		DBG_ASSERT(i_Index<m_pModelInstance->Materials().size(), "Material index out of template range");

		matMaterial *pMaterial = m_pModelInstance->Materials()[i_Index];
		mtrMaterialUtil::SetMaterialData(*pMaterial, i_Data, *m_pModelInstance, 
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
		DBG_ASSERT(!HasMaterialAnimation(i_Index), "Need to remove material animation before deleting channels. (" << mat_name << ")");

		mtrlPropertyObject *pPropertyObject = m_PropertyObjects[i_Index];

		tmlnScriptObject *pScriptObject = dynamic_cast<tmlnScriptObject*>(this);
		if (pScriptObject)
		{
			// Disconnect the channels from this script object
			pPropertyObject->RemoveChannels(pScriptObject);
		}

		mdlMaterialInfo &info = (*m_Materials[i_Index]);
		matMaterial *pMaterial = m_pModelInstance->Materials()[i_Index];

		// Only use the object's texture directory now, since the whole
		// material library can be used.
		//fsLocator tex_dir = get_texture_dir(info);
		fsLocator tex_dir = m_TextureDirectory;

		const int base_layer_index = 0;
		pPropertyObject->SetShaders(mtrlShaderUtil::CreatePropertyObject(info, base_layer_index, pMaterial, tex_dir, m_pModelInstance),
									mtrlShaderUtil::CreateGlowObject(info, pMaterial, tex_dir, m_pModelInstance),
									mtrlShaderUtil::CreateOutlineObject(info, pMaterial, tex_dir, m_pModelInstance),
									mtrlShaderUtil::CreateUVTransformObject(info, base_layer_index, pMaterial, tex_dir, m_pModelInstance),
									mtrlShaderUtil::CreateReflectionObject(info, pMaterial, tex_dir, m_pModelInstance),
									mtrlShaderUtil::CreateDisplacementObject(info, pMaterial, tex_dir, m_pModelInstance),
									mtrlShaderUtil::CreateNormalsObject(info, pMaterial, tex_dir, m_pModelInstance),
									mtrlShaderUtil::CreateRendermanOverrideObject(info, pMaterial, tex_dir, m_pModelInstance),
									mtrlShaderUtil::CreateTextureFilterObject(info, pMaterial, tex_dir, m_pModelInstance)
									);

		pPropertyObject->SetShaderName( info.GetShaderParams()->GetShaderName() );

		if (pScriptObject)
		{
			pPropertyObject->AddChannels( pScriptObject );
		}

		// Set shaders for the extra layers above the base material layer
		const int num_layers = info.GetNumMaterialLayers();
		for (int li=1; li<num_layers; ++li)
		{
			pPropertyObject->SetMaterialLayerShaders(li,
				mtrlShaderUtil::CreatePropertyObject(info, li, pMaterial, tex_dir, m_pModelInstance),
				mtrlShaderUtil::CreateUVTransformObject(info, li, pMaterial, tex_dir, m_pModelInstance) );

			pPropertyObject->SetLayerShaderName( li, info.GetShaderParams(li)->GetShaderName() );
		}

		// Have to reselect in order to update user interface about our new material shaders
		//if (sel3dMgr::GetSelected() == pPropertyObject)
		//{
		//	sel3dMgr::Renotify();
		//}
	}
}

//--------------------------------------------------------------------
//	Change the material at a given index, using the material
//	data structure.
//--------------------------------------------------------------------
void mtrlScriptObject::ChangeMaterialLayerData(int i_MaterialIndex, 
										   int i_LayerIndex,
										  const mdlMaterialInfo& i_Data,
										  bool i_bUpdateProperties)
{
	DBG_ASSERT(i_MaterialIndex<m_Materials.size(), "Material index out of range");

	if (m_pModelInstance)
	{
		// set material info in fragments' materials
		//bool bTextureChanged = mtrMaterialUtil::DidTextureChange(m_Materials[i_Index], i_Data);
		DBG_ASSERT(i_MaterialIndex<m_pModelInstance->Materials().size(), "Material index out of template range");

		matMaterial *pMaterial = m_pModelInstance->Materials()[i_MaterialIndex];
		mtrMaterialUtil::SetMaterialLayerData(*pMaterial, i_LayerIndex, i_Data, *m_pModelInstance, 
			m_TextureDirectory, m_GeneralTextureDirectory);

		UpdateTargetRenderer(i_MaterialIndex);
	}

	// set local data structure
	m_Materials[i_MaterialIndex]->SetShaderParams(i_Data.GetShaderParams(i_LayerIndex), i_LayerIndex);
	m_Materials[i_MaterialIndex]->UVTransform(i_LayerIndex) = (i_Data.GetUVTransform(i_LayerIndex));

	// Create new property objects when needed
	if (i_bUpdateProperties)
	{
		DBG_ASSERT(!HasMaterialAnimation(i_MaterialIndex), "Need to remove material animation before deleting channels.");

		mtrlPropertyObject *pPropertyObject = m_PropertyObjects[i_MaterialIndex];

		tmlnScriptObject *pScriptObject = dynamic_cast<tmlnScriptObject*>(this);
		if (pScriptObject)
		{
			// Disconnect the channels from this script object
			pPropertyObject->RemoveChannels(pScriptObject);
		}

		mdlMaterialInfo &info = (*m_Materials[i_MaterialIndex]);
		matMaterial *pMaterial = m_pModelInstance->Materials()[i_MaterialIndex];

		// Only use the object's texture directory now, since the whole
		// material library can be used.
		//fsLocator tex_dir = get_texture_dir(info);
		fsLocator tex_dir = m_TextureDirectory;

		const int base_layer_index = 0;

		if (i_LayerIndex == base_layer_index)
		{
			pPropertyObject->SetShaders(mtrlShaderUtil::CreatePropertyObject(info, base_layer_index, pMaterial, tex_dir, m_pModelInstance),
										mtrlShaderUtil::CreateGlowObject(info, pMaterial, tex_dir, m_pModelInstance),
										mtrlShaderUtil::CreateOutlineObject(info, pMaterial, tex_dir, m_pModelInstance),
										mtrlShaderUtil::CreateUVTransformObject(info, base_layer_index, pMaterial, tex_dir, m_pModelInstance),
										mtrlShaderUtil::CreateReflectionObject(info, pMaterial, tex_dir, m_pModelInstance),
										mtrlShaderUtil::CreateDisplacementObject(info, pMaterial, tex_dir, m_pModelInstance),
										mtrlShaderUtil::CreateNormalsObject(info, pMaterial, tex_dir, m_pModelInstance),
										mtrlShaderUtil::CreateRendermanOverrideObject(info, pMaterial, tex_dir, m_pModelInstance),
										mtrlShaderUtil::CreateTextureFilterObject(info, pMaterial, tex_dir, m_pModelInstance)
										);
		}
		else
		{
			pPropertyObject->ChangeMaterialLayerShaders(i_LayerIndex,
				mtrlShaderUtil::CreatePropertyObject(info, i_LayerIndex, pMaterial, tex_dir, m_pModelInstance),
				mtrlShaderUtil::CreateUVTransformObject(info, i_LayerIndex, pMaterial, tex_dir, m_pModelInstance) );
		}

		if (pScriptObject)
		{
			pPropertyObject->AddChannels( pScriptObject );
		}
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
	DBG_ASSERT(i_Index<m_Materials.size(), "Material index out of range");
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
	DBG_ASSERT(i_Index<m_Materials.size(), "Material index out of range");
	return m_Materials[i_Index]->UsesMaterialLibrary();
}

//--------------------------------------------------------------------
// Get list of used textures in order to support copying them
// to the material library when exporting.
//--------------------------------------------------------------------
void mtrlScriptObject::GetTextureList(int i_Index, std::vector<fsLocator>& o_TextureList)
{
	DBG_ASSERT(i_Index<m_PropertyObjects.size(), "Material index out of property object range " << i_Index << " < " << m_PropertyObjects.size());
	mtrlPropertyObject *object = m_PropertyObjects[i_Index];
	object->GetTextureList(o_TextureList);
}

//--------------------------------------------------------------------
// Let the modeltemplate know that a texture is being changed.
//--------------------------------------------------------------------
void mtrlScriptObject::ReplaceTexture(int i_Index, matTexture* i_pOldTexture, matTexture* i_pNewTexture)
{
	DBG_ASSERT(i_Index<m_Materials.size(), "Material index out of range");
	if (m_pModelInstance)
	{
		DBG_ASSERT(i_Index<m_pModelInstance->Materials().size(), "Material index out of template range");
		if (i_pOldTexture != NULL)
			envSTLHelpers::RemoveOneValue(m_pModelInstance->Textures(), i_pOldTexture);
		if (i_pNewTexture != NULL)
			m_pModelInstance->Textures().push_back(i_pNewTexture);
	}
}

//--------------------------------------------------------------------
//  Get a list of resources used by materials.  
//  The resources will be appended to the passed in list.
//--------------------------------------------------------------------
void mtrlScriptObject::GetMaterialResources( fsResourceTrackerData& io_List )
{
	std::vector<fsLocator> texture_list;
	std::vector<fsLocator> shader_list;

	for (int i=0; i<m_Materials.size(); ++i)
	{
		this->GetTextureList( i, texture_list );

		itString itName;
		fsLocator shaderLoc;	
		fsFileUtil::LocatorToUnicodeString( m_Materials[i]->GetShaderParams()->GetShaderName(), itName );	
		fsFileUtil::UnicodeStringToLocator( itName, shaderLoc );

		// Test to see whether its a user shader or not 
		if ( shaderLoc.GetNumNames() != 1 )
		{
			shader_list.push_back( shaderLoc );
		}
	}

	std::vector<fsLocator>::iterator it;

	std::sort( texture_list.begin(), texture_list.end() );
	std::unique( texture_list.begin(), texture_list.end() );
	
	for (it = texture_list.begin(); it != texture_list.end(); ++it)
		io_List.Add( *it );

	std::sort( shader_list.begin(), shader_list.end() );
	std::unique( shader_list.begin(), shader_list.end() );

	for (it = shader_list.begin(); it != shader_list.end(); ++it)
		io_List.Add( *it );

}

//--------------------------------------------------------------------
// Access to the three property objects per material
//--------------------------------------------------------------------
mtrlShaderObject* mtrlScriptObject::GetShaderDataObject(int i_Index)
{
	DBG_ASSERT(i_Index < m_PropertyObjects.size(), "Material index out of property object range " << i_Index << " < " << m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetShaderDataObject();
}
mtrlShaderObject* mtrlScriptObject::GetGlowDataObject(int i_Index)
{
	DBG_ASSERT(i_Index < m_PropertyObjects.size(), "Material index out of property object range " << i_Index << " < " << m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetGlowDataObject();
}
mtrlShaderObject* mtrlScriptObject::GetOutlineDataObject(int i_Index)
{
	DBG_ASSERT(i_Index < m_PropertyObjects.size(), "Material index out of property object range " << i_Index << " < " << m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetOutlineDataObject();
}
mtrlShaderObject* mtrlScriptObject::GetUVTransformDataObject(int i_Index)
{
	DBG_ASSERT(i_Index < m_PropertyObjects.size(), "Material index out of property object range " << i_Index << " < " << m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetUVTransformObject();
}
mtrlShaderObject* mtrlScriptObject::GetReflectionDataObject(int i_Index)
{
	DBG_ASSERT(i_Index < m_PropertyObjects.size(), "Material index out of property object range " << i_Index << " < " << m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetReflectionDataObject();
}
mtrlShaderObject* mtrlScriptObject::GetDisplacementDataObject(int i_Index)
{
	DBG_ASSERT(i_Index < m_PropertyObjects.size(), "Material index out of property object range " << i_Index << " < " << m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetDisplacementDataObject();
}
mtrlShaderObject* mtrlScriptObject::GetNormalsDataObject(int i_Index)
{
	DBG_ASSERT(i_Index < m_PropertyObjects.size(), "Material index out of property object range " << i_Index << " < " << m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetNormalsDataObject();
}
mtrlShaderObject* mtrlScriptObject::GetTextureFilterDataObject(int i_Index)
{
	DBG_ASSERT(i_Index < m_PropertyObjects.size(), "Material index out of property object range " << i_Index << " < " << m_PropertyObjects.size());
	return m_PropertyObjects[i_Index]->GetTextureFilterDataObject();
}

//--------------------------------------------------------------------
//	Return Material ui properties
//--------------------------------------------------------------------
mtrlPropertyObject* mtrlScriptObject::GetMaterialUI(int i_Index) const
{
	DBG_ASSERT(i_Index < m_PropertyObjects.size(), "Material index out of property object range " << i_Index << " < " << m_PropertyObjects.size());
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
//	Return bounding box of all surfaces that are currently
//	using this material.
//--------------------------------------------------------------------
void mtrlScriptObject::GetBoundingBoxForMaterial(int i_Index, maAxisBox &o_BBox)
{
	if (i_Index >=0 && i_Index<m_FragmentNodes.size())
	{
		const int num_nodes = m_FragmentNodes[i_Index].size();
		for (int i=0; i<num_nodes; i++)
			o_BBox.Union( m_FragmentNodes[i_Index][i]->GetWorldBox() );
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

		//check to see if the material is highlighted
		if( pMaterial == mtrlHighlight::GetHighlightMaterial() )
			pMaterial = pPickedNode->GetFragment()->GetOriginalMaterial();

		if (pMaterial && m_pModelInstance)
		{
			for (int i=0; i<m_pModelInstance->Materials().size(); ++i)
			{
				if (pMaterial == m_pModelInstance->Materials()[i])
					return i;
			}
		}
	}
	return -1;
}

//--------------------------------------------------------------------
//	Store selected index in order to maintain it when re-selecting
//--------------------------------------------------------------------
//void mtrlScriptObject::SetLastSelectedMaterialIndex(int i_Index)
//{
//	m_LastSelectedMaterialIndex = i_Index;
//}
//int mtrlScriptObject::GetLastSelectedMaterialIndex() const
//{
//	return m_LastSelectedMaterialIndex;
//}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrlScriptObject::HighlightMaterial(int i_Index, matMaterial* i_MatHilight, bool i_On)
{
	//matMaterial* pExisting = NULL;
	matMaterial* pReplace = NULL;
	if (i_On)
	{
		// replace existing with highlight
		//pExisting = m_pModelInstance->Materials()[i_Index];
		pReplace = i_MatHilight;
	}
	else
	{
		// replace highlight with original
		//pExisting = i_MatHilight;
		pReplace = m_pModelInstance->Materials()[i_Index];
	}
	
	std::vector<g3dSceneNode*> &fragments = m_FragmentNodes[i_Index];
	for (int i = 0; i < fragments.size(); i++)
	{

		if (!fragments[i]->GetFragment()->GetUseBakedTexture())
		{
			if( i_On )
			{
				fragments[i]->GetFragment()->SetOriginalMaterial(m_pModelInstance->Materials()[i_Index]);
			}
			//DBG_ASSERT(pExisting == fragments[i]->GetFragment()->GetMaterial(), "HighlightMaterial: original fragment material does not match");
			fragments[i]->GetFragment()->SetMaterial(pReplace);
		}
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mtrlScriptObject::RenderTargetInfo::RenderTargetInfo()
:	m_Renderer(NULL),m_TargetRenderer(NULL),m_pMaterial(NULL),
	m_pParentObject(NULL),m_pRenderTarget(NULL), m_bIsCubeMap(false),
	m_bIsDynamic(false)
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
	std::vector<matMaterial*>& materials = m_pModelInstance->Materials();
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
	// stop any render threads for texture manager changes
	gpxRenderControl::ConfirmSingleThread();

	DBG_ASSERT(i_Index<m_pModelInstance->Materials().size(), "Material index out of range");
	matMaterial *pMaterial = m_pModelInstance->Materials()[i_Index];
	DBG_ASSERT(pMaterial != NULL, "null material for reflections!");

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
		envSTLHelpers::RemoveOneValue(l_MaterialRenderTargets, pRenderTargetInfo);
	}
	DBG_ASSERT( found == (pRenderTargetInfo!=NULL) , "inconsistent reflection render target state");

	effReflectionMap* pData = &pMaterial->ReflectionData();
	if (pMaterial->GetHasReflection() && (pData != NULL) && ShaderSupportsReflection(pMaterial))
	{
		// this is critical to get the reflection to show up in the shader!
		if (pRenderTargetInfo != NULL)
		{
			pData->m_ReflectionMap = pRenderTargetInfo->m_pRenderTarget;
		}

		bool bChangePlanarCubic = (pRenderTargetInfo == NULL) ? true : (pRenderTargetInfo->m_bIsCubeMap == pData->m_bIsPlanar);
		bool bChangeResolution = (pRenderTargetInfo == NULL) ? true : (pRenderTargetInfo->m_pRenderTarget->GetWidth() != pData->m_ReflMapResolution);
		bool bChangeDynamic = (pRenderTargetInfo == NULL) ? true : (pRenderTargetInfo->m_bIsDynamic != pData->m_bAutoGenEnvMap);
		// do we need a new rendertarget?
		// - did the resolution change?
		// - did the planar/cubic state change?
		// - was the old rendertarget null?
		bool needNewReflMap = (pRenderTargetInfo == NULL) || bChangeResolution || bChangePlanarCubic;

		// do we need a new renderer?
		// - did the planar/cubic state change?
		// - was the old rendertarget null?
		// - did the dynamic flag change?
		bool needNewRenderer = (pRenderTargetInfo == NULL) || bChangeDynamic || bChangePlanarCubic;

		if (needNewReflMap || needNewRenderer)
		{
			matTexture* pTargetTexture = pData->m_ReflectionMap;
			if (needNewReflMap)
			{
				// remove old target texture if there is one.
				if (pRenderTargetInfo != NULL)
				{
					matTextureMgr::ReleaseTexture(pRenderTargetInfo->m_pRenderTarget);
					pRenderTargetInfo->m_pRenderTarget = NULL;
					if (pRenderTargetInfo->m_TargetRenderer)
						pRenderTargetInfo->m_TargetRenderer->SetTarget(NULL);
				}

				// create new render target texture.

				g2dPFD* image_format = NULL;
				// TEMPORARILY use default pixel format for render targets
				// FIX TO GET HDR WORKING. We should be able to render reflections to HDR render target.

				//	g2dPFD hdrPFD(g2dPFD::e_RGBA16f, 16*4);
				//	if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_HDR)
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
			}

			// the map had better be a render target.
			g2dRenderTarget* pTarget = pTargetTexture->GetRenderTargetAPI();
			DBG_ASSERT(pTarget, "Reflection map is not a valid render target");
			// at this point we have recreated the reflection map.
			// now create the target renderer that will be used to fill the map.

			g3dSceneRenderer* reflMapRenderer = pRenderTargetInfo ? pRenderTargetInfo->m_Renderer : NULL;
			g3dTargetRenderer* reflTargetRenderer = pRenderTargetInfo ? pRenderTargetInfo->m_TargetRenderer : NULL;

			if (needNewRenderer)
			{
				// get the scene object for the renderer
				scObject* pSceneObject = NULL;
				if (api3dObjectGeom *pGeomObject = dynamic_cast<api3dObjectGeom*>(m_pObject))
					pSceneObject = pGeomObject->Object();
				else if (api3dObjectEntity *pEntityObject = dynamic_cast<api3dObjectEntity*>(m_pObject))
					pSceneObject = pEntityObject->Object();

				// remove and delete old
				if (pRenderTargetInfo)
				{
					if (!api3dTargetRendererMgr::RemoveTargetRenderer(pRenderTargetInfo->m_TargetRenderer))
						tma3dViewerMgr::RemoveTargetRendererFromViewers(pRenderTargetInfo->m_TargetRenderer);

					delete pRenderTargetInfo->m_TargetRenderer;
					delete pRenderTargetInfo->m_Renderer;
				}
				if (pData->m_bIsPlanar)
				{
					// create a planar reflection renderer
					reflMapRenderer = g3dSceneRendererCreate::CreatePlanarReflectionRenderer(pSceneObject, pMaterial);
					reflTargetRenderer = new g3dPlanarReflectionTargetRenderer(pTarget, 
						reflMapRenderer, api3dScene::GetScene(), NULL);
					tma3dViewerMgr::AddTargetRendererToViewers(reflTargetRenderer);
				}
				else
				{
					// create a cube map reflection renderer
					reflMapRenderer = g3dSceneRendererCreate::CreateCubeMapRenderer(pSceneObject, pMaterial, pData->m_bAutoGenEnvMap);
					reflTargetRenderer = new g3dCubicReflectionTargetRenderer(pTarget, 
						reflMapRenderer, api3dScene::GetScene(), NULL);
					api3dTargetRendererMgr::AddTargetRenderer(reflTargetRenderer, api3dTargetRendererMgr::e_Reflection);
				}
				DBG_ASSERT(reflTargetRenderer, "failed to create reflection target renderer");
				reflTargetRenderer->SetBackgroundColor(g2dRGBColor(0,0,0));
			}

			// completely refill rendertargetinfo struct regardless of what changed.
			if (pRenderTargetInfo == NULL)
				pRenderTargetInfo = new RenderTargetInfo();
			pRenderTargetInfo->m_pParentObject = this;
			pRenderTargetInfo->m_pMaterial = pMaterial;
			pRenderTargetInfo->m_Renderer = reflMapRenderer;
			pRenderTargetInfo->m_TargetRenderer = reflTargetRenderer;
			if (pRenderTargetInfo->m_TargetRenderer)
				pRenderTargetInfo->m_TargetRenderer->SetTarget(pTarget);
			pRenderTargetInfo->m_pRenderTarget = pTargetTexture;
			pRenderTargetInfo->m_bIsCubeMap = !pData->m_bIsPlanar;
			pRenderTargetInfo->m_bIsDynamic = pData->m_bAutoGenEnvMap;

			reflTargetRenderer->SetCamera(&cam3dMgr::GetCamera());
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
		if (pRenderTargetInfo)
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
//	Increase/Decrease material count for given node
//--------------------------------------------------------------------
void mtrlScriptObject::IncreaseMaterialRefCount(const matMaterial* i_Material)
{
	int index = GetIndexForMaterial(i_Material);
	if (index >= 0)
	{
	//	increase_ref_count(index, m_RefCount, m_PropertyObjects[index], m_Materials[index]);
		matMaterial* pMaterial = const_cast<matMaterial*>(i_Material);
		increase_ref_count(index, pMaterial, m_pModelInstance, m_Materials[index],
			m_TextureDirectory, m_GeneralTextureDirectory);
	}
}

void mtrlScriptObject::DecreaseMaterialRefCount(const matMaterial* i_Material)
{
	int index = GetIndexForMaterial(i_Material);
	if (index >= 0)
	{
		//decrease_ref_count(index, m_RefCount, m_PropertyObjects[index], m_Materials[index]);
		matMaterial* pMaterial = const_cast<matMaterial*>(i_Material);
		decrease_ref_count(index, pMaterial, m_pModelInstance, m_Materials[index],
			m_TextureDirectory, m_GeneralTextureDirectory);
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
	float texMem = 0;
	if (m_pModelInstance != NULL)
	{
		for (int i = 0; i < m_pModelInstance->GetMaterials().size(); i++)
		{
			// get the textures of this material
			matMaterial* mat = m_pModelInstance->Materials()[i];
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
	stm << "  " << textures.size() << " textures, size: " << texMem << " KB\r\n";
	i_File.WriteLine(stm.str());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
entModelInstance * mtrlScriptObject::GetModelInstance()
{
	return m_pModelInstance;
}