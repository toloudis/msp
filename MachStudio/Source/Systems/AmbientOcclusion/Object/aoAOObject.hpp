/*****************************************************************************
**  aoAOObject.hpp
**
**      A aoAOObject is a derived class for a storyboard, which is a textured
**	polygon that always faces the camera.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef AO_AOOBJECT_HPP
#error aoAOObject.hpp multiply included
#endif
#define AO_AOOBJECT_HPP

#ifndef AO_AODATA_HPP
#include "Systems/AmbientOcclusion/Data/aoAOData.hpp"
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
class gpxAmbientOcclusion;

//============================================================================
//============================================================================
class aoAOObject : public cmmSelectablePropertyObject
{
	public:
		//--------------------------------------------------------------------
		// This object takes ownership of the arguments passed in
		//--------------------------------------------------------------------
		aoAOObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~aoAOObject();


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
		const aoAOData& GetData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetData(const aoAOData &i_Data);

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
		// ClipPlaneEpsilon property access
		//--------------------------------------------------------------------
		prtyDistance& PropertyClipPlaneEpsilon();
		const prtyDistance& GetPropertyClipPlaneEpsilon() const;
		
		//--------------------------------------------------------------------
		// NoClipPlaneEpsilon property access
		//--------------------------------------------------------------------
		prtyDistance& PropertyNoClipPlaneEpsilon();
		const prtyDistance& GetPropertyNoClipPlaneEpsilon() const;
		
		//--------------------------------------------------------------------
		// AreaRatio property access
		//--------------------------------------------------------------------
		prtyFloat& PropertyAreaRatio();
		const prtyFloat& GetPropertyAreaRatio() const;
		
		//--------------------------------------------------------------------
		// BehindPlaneEpsilon property access
		//--------------------------------------------------------------------
		prtyDistance& PropertyBehindPlaneEpsilon();
		const prtyDistance& GetPropertyBehindPlaneEpsilon() const;

		//--------------------------------------------------------------------
		// SetName
		//--------------------------------------------------------------------
		//void SetName(const nameString& i_Name);

private:
		aoAOData		m_Data;
		sel3dObject*	m_pParent;
		gpxAmbientOcclusion* m_pAOProxy;
		
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void UpdateSSAO(prtyProperty *i_pProperty, bool i_bDirty);
};

