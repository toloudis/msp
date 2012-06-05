/*****************************************************************************
**	brshPropertyObject.cpp
**
**	the property object for the paint brush tool
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef BRSH_PROPERTY_OBJECT_HPP
#error brshPropertyObject.hpp multiply included
#endif
#define BRSH_PROPERTY_OBJECT_HPP

#ifndef RPRFPREFSDATA_HPP
#include "Support/brsh/data/brshData.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif


//----------------------------------------------------------------------------
// Forward Declarations
//----------------------------------------------------------------------------
class fsLocator;
class matTexture;
class prtyButtonUIInfo;

//============================================================================
//  Prefs object for full render prefs used by the render layers
//============================================================================
class brshPropertyObject : public prtyObject
{
public:
	//------------------------------------------------------------------------
	// Constructor/Destructor
	//------------------------------------------------------------------------
	brshPropertyObject();
	~brshPropertyObject();

	//------------------------------------------------------------------------
	// Return the texture location of this object
	//------------------------------------------------------------------------
	fsLocator& GetTextureLocator();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetSaveEnabled(bool i_bEnabled);
	bool GetSaveEnabled();

protected:
	//------------------------------------------------------------------------
	// Initialize the property object
	//------------------------------------------------------------------------
	void Init();

private:
	//------------------------------------------------------------------------
	// Assign data objects to their UI components
	//------------------------------------------------------------------------
	void RegisterProperties();

	//------------------------------------------------------------------------
	// Assign the function callbacks for each data object
	//------------------------------------------------------------------------
	void AddCallbacks();

	//------------------------------------------------------------------------
	// Callbacks
	//------------------------------------------------------------------------
	virtual void UpdatePaintMode(prtyProperty *i_pProperty, bool i_bDirty);
	virtual void UpdateBrushShape(prtyProperty *i_pProperty, bool i_bDirty);
	virtual void UpdateBrushColor(prtyProperty *i_pProperty, bool i_bDirty);
	virtual void SaveTexture(prtyProperty *i_pProperty, bool i_bDirty);

public:
	brshData m_Data;
	
private:
	fsLocator m_ShapeName;
	prtyButtonUIInfo* m_pSaveButton;
}; // end class rprfPrefsObject