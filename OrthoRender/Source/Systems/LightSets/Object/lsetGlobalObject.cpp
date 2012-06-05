/*****************************************************************************
**  lsetGlobalObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Object/lsetGlobalObject.hpp"

#include "Systems/LightSets/GUI/lsetDialogDataUtil.hpp"
#include "Systems/LightSets/Data/lsetDocumentChunk.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Core/prty/prtyColorRGBEditUIInfo.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lsetGlobalObject::lsetGlobalObject()
{
	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyColorRGBEditUIInfo(&(m_AmbientLight), "Ambient Light", "Ambient contribution from light set");
	AddProperty( pPUII );


	// Register callbacks to update particle generator and icons
	// when properties change
	m_AmbientLight.AddCallback(new prtyCallbackWrapper<lsetGlobalObject>(this, &lsetGlobalObject::ValueChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lsetGlobalObject::~lsetGlobalObject()
{
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string lsetGlobalObject::GetPick3dName() const
{
	return "Global Ambient";
}

//--------------------------------------------------------------------
// AmbientLight property access
//--------------------------------------------------------------------
prtyColor&	lsetGlobalObject::PropertyAmbientLight()
{
	return m_AmbientLight;
}
const prtyColor&	lsetGlobalObject::GetPropertyAmbientLight() const
{
	return m_AmbientLight;
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void lsetGlobalObject::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	ltstLightSetMgr::SetSceneAmbientLight(m_AmbientLight.GetValue());

	if (i_bDirty)
	{
		lsetDocumentChunk::ActiveDataChanged();
	}
}
