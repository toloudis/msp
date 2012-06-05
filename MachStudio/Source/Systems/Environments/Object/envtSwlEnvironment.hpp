/*****************************************************************************
**  envtSwlEnvironment.hpp
**
**      envtSwlEnvironment
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef ENVT_SWLENVIRONMENT_HPP
#error envtSwlEnvironment.hpp multiply included
#endif
#define ENVT_SWLENVIRONMENT_HPP

#ifndef ENVT_DATA_HPP
#include "Systems/Environments/Data/envtData.hpp"
#endif
#ifndef CMM_SELECTABLEPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmSelectablePropertyObject.hpp"
#endif 

#include <vector>


//============================================================================
//	forward references
//============================================================================
class evmtEnvironment;
class gfFileTxt;
class nameString;
class prtyTextureFileChooserUIInfo;
class gpxGlobalAmbient;

//============================================================================
//============================================================================
class envtSwlEnvironment : public cmmSelectablePropertyObject
{
public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		envtSwlEnvironment();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~envtSwlEnvironment();

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
		// Enable property access
		//--------------------------------------------------------------------
		prtyBoolean&	PropertyEnable();
		const prtyBoolean&	GetPropertyEnable() const;

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
		//--------------------------------------------------------------------
		void ReportMemory(gfFileTxt& i_File);
	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void EnableChanged(prtyProperty *i_pProperty, bool i_bDirty);
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
		gpxGlobalAmbient*	m_pGpxEnvironment;

		sel3dObject*	m_pParent;

		prtyTextureFileChooserUIInfo* m_pDiffuseFileChooser;
		prtyTextureFileChooserUIInfo* m_pSpecularFileChooser;

		bool m_bEnabledRamp;
};


