/********************************************************************************************\
**  fgmtPropertyObject.hpp
**
**
**  StudioGPU
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef FGMT_PROPERTYOBJECT_HPP
#error fgmtPropertyObject.hpp multiply included
#endif
#define FGMT_PROPERTYOBJECT_HPP

#ifndef CMM_SELECTABLEPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmSelectablePropertyObject.hpp"
#endif 
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

#include <vector>

class fgmtFragmentData;
class fsLocator;
class g3dFragment;
class g3dSceneNode;
class matTexture;
class maAxisBox;
class gpxFragment;
class fgmtScriptObject;
class prtyTextBoxUIInfo;

//============================================================================
//============================================================================
class fgmtPropertyObject : public cmmSelectablePropertyObject
{
public:
	//------------------------------------------------------------------------
	// Because of instancing, each fragment property object might control
	// multiple g3dFragments
	//------------------------------------------------------------------------
	fgmtPropertyObject(const std::string& i_Name,
						fgmtFragmentData& i_Data, 
						const std::vector<g3dFragment*> &i_Fragments,
						const std::vector<g3dSceneNode*> &i_SceneNodes,
						const fsLocator& i_TextureDir,
						fgmtScriptObject* i_Parent = NULL);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~fgmtPropertyObject();

	//------------------------------------------------------------------------
	// Quick access to name given in constructor
	//------------------------------------------------------------------------
	inline const std::string&  GetName() const;

//============================================================================
// sel3dObject - virtual function overrides
//============================================================================

	//--------------------------------------------------------------------
	// Set Parent pointer to use when creating selectable property objects
	//--------------------------------------------------------------------
	//void SetParent(sel3dObject* i_pParent);

	//------------------------------------------------------------------------
	// Get parent object of this object in order to define relationships
	//	between icons and their affected objects.
	//------------------------------------------------------------------------
	//virtual sel3dObject* GetParentObject() const;

	//------------------------------------------------------------------------
	// Get the name of the object for display when selected
	//------------------------------------------------------------------------
	virtual std::string GetDisplayName() const;

	//--------------------------------------------------------------------
	//	GetWorldBox returns a box which completely encloses the fragment.
	//--------------------------------------------------------------------
	maAxisBox GetWorldBox() const;

	//------------------------------------------------------------------------
	// scene specific data folder where AO textures will be stored.
	//------------------------------------------------------------------------
	void SetAOTextureLocator(const fsLocator& i_AOTextureLocator);

	//------------------------------------------------------------------------
	// model material texture folder where AO textures will be stored
	//------------------------------------------------------------------------
	void SetModelTextureLocator(const fsLocator& i_ModelTextureLocator);

	//------------------------------------------------------------------------
	// model material texture folder where Baked textures will be stored
	//------------------------------------------------------------------------
	void SetBakedTextureLocator(const fsLocator& i_BakedTextureLocator, const std::string& i_BakedTextureFormat);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanUp();

	//--------------------------------------------------------------------
	// delete and re-create AO texture with current settings
	//--------------------------------------------------------------------
	//void ResetAOTexture();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//fsLocator LocateAOTexture(bool i_bSceneSpecificFolder);

private:
	//------------------------------------------------------------------------
	//	data - this is a ref to a data object owned externally.
	//------------------------------------------------------------------------
	fgmtFragmentData& m_Data; 
	//std::vector<g3dFragment*> m_Fragments;
	std::vector<gpxFragment*> m_FragmentProxies;
	std::vector<g3dSceneNode*> m_SceneNodes;
	std::string m_Name;
	fgmtScriptObject* m_pParent;
	//sel3dObject* m_pParent;

	void UpdateFlags(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateBakeFlag(prtyProperty *i_pProperty, bool i_bDirty);

	//void UpdateAOOccluder(prtyProperty *i_pProperty, bool i_bDirty);
	//void UpdateAOData(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateAOTexture(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateGITexture(prtyProperty *i_pProperty, bool i_bDirty);
	//void UpdateAOTextureName(prtyProperty *i_pProperty, bool i_bDirty);

	//matTexture* m_pAOTexture;
	// scene specific data folder where AO textures will be stored.
	fsLocator m_AOTextureLocator;
	// model material texture folder
	fsLocator m_ModelTextureLocator;

	// baked material texture folder
	/*fsLocator m_BakedTextureLocator;
	std::string m_BakedTextureFormat;*/
	prtyText	m_BakedColorPath;
	prtyText	m_BakedNormalPath;
	shared_ptr<prtyTextBoxUIInfo> m_BakedColorPathUIInfo;
	shared_ptr<prtyTextBoxUIInfo> m_BakedNormalPathUIInfo;

	//--------------------------------------------------------------------
	// delete and re-create AO texture with current settings
	//--------------------------------------------------------------------
	//void DestroyAOTexture();
	//void CreateAOTexture();
};

//------------------------------------------------------------------------
// Quick access to name given in constructor
//------------------------------------------------------------------------
inline const std::string& fgmtPropertyObject::GetName() const
{
	return m_Name;
}
