/*****************************************************************************
**	fgmtScriptObject.hpp
**
**	This class handles altering the fragment flags within a script object
**	after loading
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef FGMT_SCRIPTOBJECT_HPP
#error fgmtScriptObject.hpp multiply included
#endif
#define FGMT_SCRIPTOBJECT_HPP

#ifndef FGMT_FRAGMENTSAODATA_HPP
#include "Support/fgmt/fgmtFragmentsAOData.hpp"
#endif

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <list>
#include <string>
#include <vector>

//============================================================================
//============================================================================
class api3dObject;
class fgmtFragmentData;
class fgmtPropertyObject;
class fsResourceTrackerData;
class g3dFragment;
class gfFileTxt;
class matMaterial;
class matTexture;
class pick3dPickObject;
class prtyObject;

//============================================================================
//============================================================================
class fgmtScriptObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes object of which to override fragments and
	//	also locator of model in order to read/write fragments.
	//--------------------------------------------------------------------
	fgmtScriptObject(api3dObject* i_pObject,
		const fsLocator& i_ModelLocator);


	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~fgmtScriptObject();

	//--------------------------------------------------------------------
	// Set Parent pointer to use when creating selectable 
	// property objects
	//--------------------------------------------------------------------
	void SetParent(pick3dPickObject* i_pParent);

	//--------------------------------------------------------------------
	// Set pointer of UI container for prty info.
	//--------------------------------------------------------------------
	void SetPrtyObject(prtyObject* i_pParent);

	//--------------------------------------------------------------------
	//	Read the model file and get fragment data structures out
	//	in order to override them.
	//--------------------------------------------------------------------
	void GatherFragments();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CleanUpFragments();

	//--------------------------------------------------------------------
	//	Get new fragment list and refresh state of flags. Used
	//	when chaning resolution of model.
	//--------------------------------------------------------------------
	void RefreshFragments();

	//--------------------------------------------------------------------
	//	Returns number of fragments. May return 0 if no fragments are
	//	overriden (if GatherFragments has not been called).
	//--------------------------------------------------------------------
	int GetNumFragments() const;

	//--------------------------------------------------------------------
	//	Return name of Fragment with given index
	//--------------------------------------------------------------------
	std::string  GetFragmentName(int i_Index) const;

	//--------------------------------------------------------------------
	//	Return g3dFragment with given index
	//--------------------------------------------------------------------
	g3dFragment* GetFragment(int i_Index) const;

	//--------------------------------------------------------------------
	//	Return names of all Fragments at in one array
	//--------------------------------------------------------------------
	void  GetFragmentNames(std::vector<std::string> &o_Names) const;

	//--------------------------------------------------------------------
	// Return index for material with given name. Returns -1
	//	if not found.
	//--------------------------------------------------------------------
	int GetIndexForName(const std::string &i_Name) const;

	//--------------------------------------------------------------------
	//	Return Fragment properties structure
	//--------------------------------------------------------------------
	const fgmtFragmentData& GetFragmentData(int i_Index) const;

	//--------------------------------------------------------------------
	//	Return Fragment ui properties
	//--------------------------------------------------------------------
	fgmtPropertyObject* GetFragmentUI(int i_Index) const;
	fgmtPropertyObject* GetFragmentUI(const std::string& i_SurfaceName) const;

	//--------------------------------------------------------------------
	// Get vector of fragments in order to store info to file
	//--------------------------------------------------------------------
	void GetFragmentData(std::vector<fgmtFragmentData> &o_Data,
		fgmtFragmentsAOData& o_AOData) const;

	//--------------------------------------------------------------------
	// Set vector of fragments from data from file. This uses the name
	//	of the fragments to match up data in order to handle cases where
	//	the model file has changed since the fragments were last 
	//	overriden.
	//--------------------------------------------------------------------
	void SetFragmentData(const std::vector<fgmtFragmentData> &i_Data,
		const fgmtFragmentsAOData& i_AOData,
		const std::string& i_ModelName);

	//--------------------------------------------------------------------
	//	Change the fragment at a given index, using the fragment
	//	data structure.
	//--------------------------------------------------------------------
	void ChangeFragmentData(int i_Index, 
							const fgmtFragmentData& i_Data);

	//--------------------------------------------------------------------
	//	Change all fragments' surface flags to match the fragment at a given index.
	//--------------------------------------------------------------------
	void ChangeFragmentDataFlags(int i_Index);
	
	//--------------------------------------------------------------------
	//	Change all fragments' AO data to match the fragment at a given index.
	//--------------------------------------------------------------------
	void ChangeFragmentDataAO(int i_Index);

	//--------------------------------------------------------------------
	//	Store selected index in order to maintain it when re-selecting
	//--------------------------------------------------------------------
	void SetLastSelectedFragmentIndex(int i_Index);
	int GetLastSelectedFragmentIndex() const;

	//--------------------------------------------------------------------
	// Get fragment index by looking for the node with the given 
	//	pick code.
	//--------------------------------------------------------------------
	int GetFragmentIndexFromPickCode(envType::UInt32 i_PickCode) const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void HighlightFragment(int i_Index, matMaterial* i_MatHilight, bool i_On);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetAOTextureLocator(fsLocator& i_Locator);

	//--------------------------------------------------------------------
	// auto-save all AO textures to file, generating filenames when needed.
	//--------------------------------------------------------------------
	void SaveAOTextures();

	//--------------------------------------------------------------------
	// auto-import all AO textures from file, using generated filenames 
	//--------------------------------------------------------------------
	void AutoImportAOTextures();

	//--------------------------------------------------------------------
	// set all texture filenames to empty string
	//--------------------------------------------------------------------
	void ClearAOTextures();

	//--------------------------------------------------------------------
	// auto-save a single fragment's AO texture.
	//--------------------------------------------------------------------
	void SaveAOTexture(int i_Index);

	//--------------------------------------------------------------------
	// auto-import a single fragment's AO texture.
	//--------------------------------------------------------------------
	void AutoImportAOTexture(int i_Index);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ReportMemory(gfFileTxt& i_File);

	//--------------------------------------------------------------------
	// display modal AO property grid
	//--------------------------------------------------------------------
	void ShowAOGrid();

	//--------------------------------------------------------------------
	// Set flag for texture baking
	//--------------------------------------------------------------------
	void SetEnableBaking(bool i_bBake);

	//--------------------------------------------------------------------
	// Access to the actual g3dFragments in object
	//--------------------------------------------------------------------
	const std::vector<g3dFragment*>& GetG3dFragments() const;

	//--------------------------------------------------------------------
	//  Get a list of resources used by fragments.  
	//  The resources will be appended to the passed in list.
	//--------------------------------------------------------------------
	void GetFragmentResources( fsResourceTrackerData& io_List );

private:
	api3dObject* m_pObject;
	pick3dPickObject* m_pParent;
	prtyObject* m_pPrtyObject;
	fgmtFragmentsAOData m_AOData;
	std::vector<fgmtFragmentData> m_FragmentData;
	std::vector<g3dFragment*> m_Fragments;
	std::vector<matMaterial*> m_Materials;
	std::vector<fgmtPropertyObject*> m_FragmentUI;
	int m_LastSelectedFragmentIndex;

	std::string m_ModelName;

	// scene specific data folder where AO textures will be stored.
	fsLocator m_AOTextureLocator;
	// model material texture folder
	fsLocator m_ModelTextureLocator;

	//--------------------------------------------------------------------
	// Callbacks for when properties change, updates member data
	//--------------------------------------------------------------------
	void AOChangedOccluder(prtyProperty *i_pProperty, bool i_bDirty);
	void AOChangedSelfOcclude(prtyProperty *i_pProperty, bool i_bDirty);
	void AOChangedInherit(prtyProperty *i_pProperty, bool i_bDirty);
	void AOChangedNSamples(prtyProperty *i_pProperty, bool i_bDirty);
	void AOChangedReceive(prtyProperty *i_pProperty, bool i_bDirty);
	void AOChangedSamplingRes(prtyProperty *i_pProperty, bool i_bDirty);
	void AOChangedDepthBias(prtyProperty *i_pProperty, bool i_bDirty);
	void AOChangedTexRes(prtyProperty *i_pProperty, bool i_bDirty);
	void AOChangedBlendFactor(prtyProperty *i_pProperty, bool i_bDirty);
	void AOChangedStatic(prtyProperty *i_pProperty, bool i_bDirty);
	void AOChangedDistCutoff(prtyProperty *i_pProperty, bool i_bDirty);

	void AddUpFragmentSize();

	// baking support
	prtyBoolean m_PrtyBake;
	void ChangedBake(prtyProperty *i_pProperty, bool i_bDirty);
};
