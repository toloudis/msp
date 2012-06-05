/*****************************************************************************
**  giGIObject.hpp
**
**      A giGIObject is a derived class for a storyboard, which is a textured
**	polygon that always faces the camera.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GI_GIOBJECT_HPP
#error giGIObject.hpp multiply included
#endif
#define GI_GIOBJECT_HPP

#ifndef GI_GIDATA_HPP
#include "Systems/GlobalIllumination/Data/giGIData.hpp"
#endif
#ifndef CMM_SELECTABLEPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmSelectablePropertyObject.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class prtyName;
class prtyEnum;
class prtyColor;
class prtyFloat;
class prtyInt32;
class gpxGlobalIllumination;

//============================================================================
//============================================================================
class giGIObject : public cmmSelectablePropertyObject
{
	public:
		//--------------------------------------------------------------------
		// This object takes ownership of the arguments passed in
		//--------------------------------------------------------------------
		giGIObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~giGIObject();


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
		// Get values as a data structure
		//--------------------------------------------------------------------
		const giGIData& GetData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetData(const giGIData &i_Data);

	//============================================================================
	//	properties
	//============================================================================

		//--------------------------------------------------------------------
		// Radius property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyRadius();
		const prtyFloat&	GetPropertyRadius() const;

		//--------------------------------------------------------------------
		// RadiusFar property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyRadiusFar();
		const prtyFloat&	GetPropertyRadiusFar() const;

		//--------------------------------------------------------------------
		// AngleBias property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyAngleBias();
		const prtyFloat&	GetPropertyAngleBias() const;

		//--------------------------------------------------------------------
		// Attenuation property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyAttenuation();
		const prtyFloat&	GetPropertyAttenuation() const;

		//--------------------------------------------------------------------
		// Contrast property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyContrast();
		const prtyFloat&	GetPropertyContrast() const;

		//--------------------------------------------------------------------
		// BlurWidth property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyBlurWidth();
		const prtyFloat&	GetPropertyBlurWidth() const;

		//--------------------------------------------------------------------
		// BlurSharpness property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyBlurSharpness();
		const prtyFloat&	GetPropertyBlurSharpness() const;

		//--------------------------------------------------------------------
		// Color property access
		//--------------------------------------------------------------------
		prtyColor&	PropertyColor();
		const prtyColor&	GetPropertyColor() const;

		//--------------------------------------------------------------------
		// Overscan Pixels property access
		//--------------------------------------------------------------------
		prtyInt32&	PropertyOverscanPixels();
		const prtyInt32&	GetPropertyOverscanPixels() const;

		//--------------------------------------------------------------------
		// SetName
		//--------------------------------------------------------------------
		//void SetName(const nameString& i_Name);

private:
		giGIData		m_Data;
		sel3dObject*	m_pParent;
		gpxGlobalIllumination* m_pGIProxy;
		
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void UpdateSSGI(prtyProperty *i_pProperty, bool i_bDirty);

		//--------------------------------------------------------------------
		// Callbacks for when RSM GI properties change, updates member data
		//--------------------------------------------------------------------
		//void UpdateRSMGI(prtyProperty *i_pProperty, bool i_bDirty);

		//--------------------------------------------------------------------
		// Callbacks for when LPV GI properties change, updates member data
		//--------------------------------------------------------------------
		void UpdateLPVGI(prtyProperty *i_pProperty, bool i_bDirty);
};

