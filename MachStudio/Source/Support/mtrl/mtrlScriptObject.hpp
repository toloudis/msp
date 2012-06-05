/*****************************************************************************
**	mtrlScriptObject.hpp
**
**	This class implements material overriding for script objects
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MTRL_SCRIPTOBJECT_HPP
#error mtrlScriptObject.hpp multiply included
#endif
#define MTRL_SCRIPTOBJECT_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef GF_FILECONSTANTS_HPP
#include "Core/gf/gfFileConstants.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif 

#include <vector>
#include <map>


//============================================================================
//============================================================================
class api3dObject;
class entModelInstance;
class entModelTemplate;
class fsResourceTrackerData;
class g3dFragment;
class g3dSceneNode;
class g3dSceneRenderer;
class g3dTargetRenderer;
class g3dViewer;
class gfFileTxt;
class matMaterial;
class matTexture;
class mdlMaterialInfo;
class mtrlShaderObject;
class mtrlPropertyObject;
class relObject;
class relRelationship;


//============================================================================
//============================================================================
class mtrlScriptObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes object of which to override materials and
	//	also locator of model in order to read/write materials.
	//--------------------------------------------------------------------
	mtrlScriptObject(api3dObject* i_pObject,
					 const fsLocator& i_ModelLocator);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~mtrlScriptObject();

	//--------------------------------------------------------------------
	// Set Parent pointer to use when creating selectable 
	// property objects
	//--------------------------------------------------------------------
	void SetParent(relObject &i_Parent);

	//--------------------------------------------------------------------
	//	Return locator passed to constructor
	//--------------------------------------------------------------------
	const fsLocator& GetModelLocator() const;

	//--------------------------------------------------------------------
	//	Read the model file and get material data structures out
	//	in order to override them.
	//--------------------------------------------------------------------
	void GatherMaterials(bool i_bLoadShaders = false, bool i_bLoadTextures = true);

	//--------------------------------------------------------------------
	// OverrideMaterials is used by the user interface. It just calls 
	// GatherMaterials; but since it is virtual, it can also set a 
	// dirty bit on the chunk and update the object part interface.
	//--------------------------------------------------------------------
	virtual void OverrideMaterials();

	//--------------------------------------------------------------------
	//	Save materials to given filename. If this is the same
	//	filename as the current locator, clears out list of 
	//  material overrides.
	//--------------------------------------------------------------------
	void SaveMaterials(const fsLocator& i_Locator, 
						const gfFileConstants::gfWriteFormats i_WriteFormat = gfFileConstants::eFileBinary);

	//--------------------------------------------------------------------
	//	ImportMaterials - read materials from file and apply
	//	to current geometry by matching names
	//--------------------------------------------------------------------
	int	ImportMaterials(const fsLocator &i_Locator);

	//--------------------------------------------------------------------
	//	Returns number of materials. May return 0 if no materials are
	//	overriden (if GatherMaterials has not been called).
	//--------------------------------------------------------------------
	int GetNumMaterials() const;

	//--------------------------------------------------------------------
	//	Return name of Material with given index
	//--------------------------------------------------------------------
	const char* GetMaterialName(int i_Index) const;

	//--------------------------------------------------------------------
	//	Return pointer of a material with given index
	//--------------------------------------------------------------------
	matMaterial* GetMaterial(int i_Index) const;

	//--------------------------------------------------------------------
	//	Return names of all Materials in one array
	//--------------------------------------------------------------------
	void  GetMaterialNames(std::vector<std::string> &o_Names) const;

	//--------------------------------------------------------------------
	// Return index for material with given name. Returns -1
	//	if not found.
	//--------------------------------------------------------------------
	int GetIndexForName(const std::string &i_Name) const;

	//--------------------------------------------------------------------
	// Return index for material with given sceneNode. Returns -1
	//	if not found.
	//--------------------------------------------------------------------
	int GetIndexForSceneNode(const g3dSceneNode* i_Node) const;

	//--------------------------------------------------------------------
	// Return index for material with given material ptr. Returns -1
	//	if not found.
	//--------------------------------------------------------------------
	int GetIndexForMaterial(const matMaterial* i_Material) const;

	//--------------------------------------------------------------------
	//	Return Material properties structure
	//--------------------------------------------------------------------
	const mdlMaterialInfo& GetMaterialData(int i_Index) const;

	//--------------------------------------------------------------------
	//	Fill array of shaders as they load in
	//--------------------------------------------------------------------
	void fillShaderArray(const std::vector< shared_ptr<mdlMaterialInfo> > &i_Data);

	//--------------------------------------------------------------------
	// Get vector of materials in order to store info to file
	//--------------------------------------------------------------------
	void GetMaterialData(std::vector< shared_ptr<mdlMaterialInfo> > &o_Data) const;

	//--------------------------------------------------------------------
	// Set vector of materials from data from file. This uses the name
	//	of the materials to match up data in order to handle cases where
	//	the model file has changed since the materials were last 
	//	overriden.
	//--------------------------------------------------------------------
	void SetMaterialData(const std::vector< shared_ptr<mdlMaterialInfo> > &i_Data);

	//--------------------------------------------------------------------
	//	Change the material at a given index, using the material
	//	data structure. If this change is coming from a property
	//	callback, then set i_bUpdateProperties to be false.
	//--------------------------------------------------------------------
	void ChangeMaterialData(int i_Index, 
							const mdlMaterialInfo& i_Data,
							bool i_bUpdateProperties);
	//--------------------------------------------------------------------
	//	Change the material at a given index, using the material
	//	data structure.
	//--------------------------------------------------------------------
	void ChangeMaterialLayerData(int i_MaterialIndex, 
		int i_LayerIndex,
		const mdlMaterialInfo& i_Data,
		bool i_bUpdateProperties);

	//--------------------------------------------------------------------
	// Set relative path to the library file for the material
	// with the given index. This will not re-load the textures,
	//use ChangeMaterialData if you want that behavior.
	//--------------------------------------------------------------------
	void SetLibraryFilename( int i_Index, 
							const fsLocator& i_RelativePath );

	//--------------------------------------------------------------------
	// Returns true if material with given index is a library material
	//--------------------------------------------------------------------
	bool IsLibraryMaterial( int i_Index ) const;

	//--------------------------------------------------------------------
	// Get list of used textures in order to support copying them
	// to the material library when exporting.
	//--------------------------------------------------------------------
	void GetTextureList(int i_Index, std::vector<fsLocator>& o_TextureList);

	//--------------------------------------------------------------------
	// Let the modeltemplate know that a texture is being changed.
	//--------------------------------------------------------------------
	void ReplaceTexture(int i_Index, matTexture* i_pOldTexture, matTexture* i_pNewTexture);

	//--------------------------------------------------------------------
	//  Get a list of resources used by materials.  
	//  The resources will be appended to the passed in list.
	//--------------------------------------------------------------------
	void GetMaterialResources( fsResourceTrackerData& io_List );

	//--------------------------------------------------------------------
	// Access to the three property objects per material
	//--------------------------------------------------------------------
	mtrlShaderObject* GetShaderDataObject(int i_Index);
	mtrlShaderObject* GetGlowDataObject(int i_Index);
	mtrlShaderObject* GetOutlineDataObject(int i_Index);
	mtrlShaderObject* GetUVTransformDataObject(int i_Index);
	mtrlShaderObject* GetReflectionDataObject(int i_Index);
	mtrlShaderObject* GetDisplacementDataObject(int i_Index);
	mtrlShaderObject* GetNormalsDataObject(int i_Index);
	mtrlShaderObject* GetTextureFilterDataObject(int i_Index);

	//--------------------------------------------------------------------
	//	Return Material ui properties
	//--------------------------------------------------------------------
	mtrlPropertyObject* GetMaterialUI(int i_Index) const;
	mtrlPropertyObject* GetMaterialUI(const std::string& i_MaterialName) const;

	//--------------------------------------------------------------------
	// Returns true if there are some drivers on the channels
	// for the material.
	//--------------------------------------------------------------------
	bool HasMaterialAnimation() const;
	bool HasMaterialAnimation(int i_Index) const;

	//--------------------------------------------------------------------
	//	Lock the materials
	//--------------------------------------------------------------------
	bool IsLockMaterials() const;
	virtual void LockMaterials(const bool i_bLock);

	//--------------------------------------------------------------------
	//	Return bounding box of all surfaces that are currently
	//	using this material.
	//--------------------------------------------------------------------
	void GetBoundingBoxForMaterial(int i_Index, maAxisBox &o_BBox);

	//--------------------------------------------------------------------
	//	Store selected index in order to maintain it when re-selecting
	//--------------------------------------------------------------------
	//void SetLastSelectedMaterialIndex(int i_Index);
	//int GetLastSelectedMaterialIndex() const;

	//--------------------------------------------------------------------
	// Get material index by looking for the node with the given 
	//	pick code.
	//--------------------------------------------------------------------
	int GetMaterialIndexFromPickCode(envType::UInt32 i_PickCode) const;

	//--------------------------------------------------------------------
	//	Toggle material hilighting 
	//--------------------------------------------------------------------
	void HighlightMaterial(int i_Index, matMaterial* i_MatHilight, bool i_On);

	//--------------------------------------------------------------------
	//	Reconstruct a particular material target renderer for this object.
	//--------------------------------------------------------------------
	void UpdateTargetRenderer(int i_Index);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ReportMemory(gfFileTxt& i_File);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	entModelInstance * GetModelInstance();

	//--------------------------------------------------------------------
	//	Update reflection targets due to HDR/LDR renderer change 
	//	(to allow the underlying render targets to change format)
	//--------------------------------------------------------------------
	static void RecreateRenderTargets();

	//--------------------------------------------------------------------
	//	Pass in a new Viewer that needs to know what reflection targets to render.
	//--------------------------------------------------------------------
	static void AddTargetsToViewer(g3dViewer* i_pViewer);

	//--------------------------------------------------------------------
	//	Increase/Decrease material count for given node
	//--------------------------------------------------------------------
	void IncreaseMaterialRefCount(const matMaterial* i_Material);
	void DecreaseMaterialRefCount(const matMaterial* i_Material);

	class RenderTargetInfo
	{
	public:
		RenderTargetInfo();
		~RenderTargetInfo();

		mtrlScriptObject* m_pParentObject;
		matMaterial* m_pMaterial;
		matTexture* m_pRenderTarget;
		g3dSceneRenderer* m_Renderer;
		g3dTargetRenderer* m_TargetRenderer;
		bool m_bIsCubeMap;
		bool m_bIsDynamic;
	};

protected:
	//--------------------------------------------------------------------
	// This virtual function is called when the materials are saved 
	//	to a new filename. The locator representing this geometry
	//	should now point to the new filename.
	//--------------------------------------------------------------------
	//virtual void NotifyLocatorChanged(const fsLocator& i_NewLocator) = 0;

private:
	api3dObject* m_pObject;
	shared_ptr<relRelationship> m_ParentRelationship;
	entModelInstance *m_pModelInstance;
	entModelTemplate *m_pModelTemplate;

	std::map< std::string, fsLocator > m_ShaderList;

	bool m_bTemplateIsSorted;
	bool m_bMaterialsLocked;
	fsLocator	m_ModelLocator;
	std::vector< shared_ptr<mdlMaterialInfo> > m_Materials;
	std::vector< std::vector<g3dSceneNode*> > m_FragmentNodes;
	//std::vector<int> m_RefCount;
	//int m_LastSelectedMaterialIndex;

	std::vector<mtrlPropertyObject*> m_PropertyObjects;

	fsLocator	m_TextureDirectory;
	fsLocator	m_GeneralTextureDirectory;

	std::map<matMaterial*, RenderTargetInfo*> m_MaterialRenderTargets;

	std::vector<maVector3d> m_GeneratedColors;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void InitTargetRenderers();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DestroyTargetRenderers();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void clear_properties(bool i_bRemoveChannels);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	fsLocator get_texture_dir(const mdlMaterialInfo &i_Info);
};
