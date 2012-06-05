/*****************************************************************************
**  envtEnvironmentObject.hpp
**
**      A envtEnvironmentObject is base class for objects that can be positioned
**  by manipulation modes.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef ENVT_ENVIRONMENTOBJECT_HPP
#error envtEnvironmentObject.hpp multiply included
#endif
#define ENVT_ENVIRONMENTOBJECT_HPP

#ifndef ENVT_DATA_HPP
#include "Systems/Environments/Data/envtData.hpp"
#endif
#ifndef CMM_NAMEDPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmNamedPropertyObject.hpp"
#endif 
#ifdef USE_SWL_UI
#ifndef	SWL_SCRIPTOBJECT_HPP
#include "Support/swl/swlScriptObject.hpp"
#endif
#ifndef ENVT_SWLINTEREST_HPP
#include "Systems/Environments/Object/envtSwlInterest.hpp"
#endif
#endif

#include <vector>


//============================================================================
//	forward references
//============================================================================
class evmtEnvironment;
class gfFileTxt;
class nameString;
//class prtyFileChooserUIInfo;
class prtyTextureFileChooserUIInfo;

//============================================================================
//============================================================================
class envtEnvironmentObject : public cmmNamedPropertyObject
#ifdef USE_SWL_UI
							  ,public swlScriptObject	
#endif
{
public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		envtEnvironmentObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~envtEnvironmentObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetDisplayName() const;

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		//virtual sel3dObject* GetParentObject() const;
		//void SetParentObject(sel3dObject* i_pPO);


	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a environment data structure
		//--------------------------------------------------------------------
		const envtData& GetData() const;

		//--------------------------------------------------------------------
		// Set from environment data structure
		//--------------------------------------------------------------------
		void SetData(const envtData &i_Data);


	//============================================================================
	//	properties
	//============================================================================

		//--------------------------------------------------------------------
		// Name property access
		//--------------------------------------------------------------------
		prtyName&	PropertyName();
		const prtyName&	GetPropertyName() const;

		//--------------------------------------------------------------------
		// DiffuseFactor property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyDiffuseFactor();
		const prtyFloat&	GetPropertyDiffuseFactor() const;

		//--------------------------------------------------------------------
		// DiffuseAngle property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyDiffuseAngle();
		const prtyFloat&	GetPropertyDiffuseAngle() const;

		//--------------------------------------------------------------------
		// SpecularFactor property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertySpecularFactor();
		const prtyFloat&	GetPropertySpecularFactor() const;

		//--------------------------------------------------------------------
		// SpecularAngle property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertySpecularAngle();
		const prtyFloat&	GetPropertySpecularAngle() const;

		//--------------------------------------------------------------------
		// GetEnvironment() 
		//--------------------------------------------------------------------
		evmtEnvironment* GetEnvironment();

		//--------------------------------------------------------------------
		// GetEnabledRamp()
		//--------------------------------------------------------------------
		bool GetEnabledRamp();

	//============================================================================
	//	3D icon/geometry
	//============================================================================

		//--------------------------------------------------------------------
		// SetName
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void ReportMemory(gfFileTxt& i_File);
	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ValueChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void DiffuseMapValueChanged(prtyProperty *i_pProperty, bool i_bDirty);

		//--------------------------------------------------------------------
		// Ramp callbacks
		//--------------------------------------------------------------------
		void RampChangedFromData(prtyProperty *i_pProperty, bool i_bDirty);
		void RampChangedFromUI(bool i_bDirty);
		void NotifyRampUI();
		
		//----------------------------------------------------------------------------
		// Update the ramp object stored in the texture control with the new value.
		//----------------------------------------------------------------------------
		void UpdateRampData();

		//----------------------------------------------------------------------------
		// return the ramp data of the texture control
		//----------------------------------------------------------------------------
		rmpData GetRampData();

		mutable envtData	m_Data;
		evmtEnvironment*	m_pEnvironment;
#ifdef USE_SWL_UI
		envtSwlInterest*	m_pSwlInterest;
#endif

		sel3dObject*	m_pParent;

		prtyTextureFileChooserUIInfo* m_pDiffuseFileChooser;
		prtyTextureFileChooserUIInfo* m_pSpecularFileChooser;

		bool m_bEnabledRamp;
};


