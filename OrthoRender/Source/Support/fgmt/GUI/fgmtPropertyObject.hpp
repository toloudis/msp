/********************************************************************************************\
**  fgmtPropertyObject.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef FGMT_PROPERTYOBJECT_HPP
#error fgmtPropertyObject.hpp multiply included
#endif
#define FGMT_PROPERTYOBJECT_HPP

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif 
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <vector>

class fgmtFragmentData;
class fsLocator;
class g3dFragment;
class matTexture;

//============================================================================
//============================================================================
class fgmtPropertyObject : public prtyObject, public pick3dPickObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fgmtPropertyObject(const std::string& i_Name,
						fgmtFragmentData& i_Data, 
						g3dFragment* i_pFragment,
						const fsLocator& i_TextureDir,
						pick3dPickObject* i_pParent);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~fgmtPropertyObject();

	//------------------------------------------------------------------------
	// Quick access to name given in constructor
	//------------------------------------------------------------------------
	inline const std::string&  GetName() const;

//============================================================================
// pick3dPickObject - virtual function overrides
//============================================================================

	//--------------------------------------------------------------------
	// Set Parent pointer to use when creating selectable property objects
	//--------------------------------------------------------------------
	void SetParent(pick3dPickObject* i_pParent);

	//------------------------------------------------------------------------
	// Get parent object of this object in order to define relationships
	//	between icons and their affected objects.
	//------------------------------------------------------------------------
	virtual pick3dPickObject* GetParentObject() const;

	//------------------------------------------------------------------------
	// Get the name of the object for display when selected
	//------------------------------------------------------------------------
	virtual std::string GetPick3dName() const;

	//------------------------------------------------------------------------
	// scene specific data folder where AO textures will be stored.
	//------------------------------------------------------------------------
	void SetAOTextureLocator(const fsLocator& i_AOTextureLocator);

	//------------------------------------------------------------------------
	// model material texture folder where AO textures will be stored
	//------------------------------------------------------------------------
	void SetModelTextureLocator(const fsLocator& i_ModelTextureLocator);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanUp();

	//--------------------------------------------------------------------
	// delete and re-create AO texture with current settings
	//--------------------------------------------------------------------
	void ResetAOTexture();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fsLocator LocateAOTexture(bool i_bSceneSpecificFolder);

private:
	//------------------------------------------------------------------------
	//	data - this is a ref to a data object owned externally.
	//------------------------------------------------------------------------
	fgmtFragmentData& m_Data; 
	g3dFragment* m_pFragment;
	std::string m_Name;
	pick3dPickObject* m_pParent;

	void UpdateFlags(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateAOOccluder(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateAOData(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateAOTexture(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateAOTextureName(prtyProperty *i_pProperty, bool i_bDirty);

	matTexture* m_pAOTexture;
	// scene specific data folder where AO textures will be stored.
	fsLocator m_AOTextureLocator;
	// model material texture folder
	fsLocator m_ModelTextureLocator;

	//--------------------------------------------------------------------
	// delete and re-create AO texture with current settings
	//--------------------------------------------------------------------
	void DestroyAOTexture();
	void CreateAOTexture();
};

//------------------------------------------------------------------------
// Quick access to name given in constructor
//------------------------------------------------------------------------
inline const std::string& fgmtPropertyObject::GetName() const
{
	return m_Name;
}
