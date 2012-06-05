/*****************************************************************************
**  fogFogObject.hpp
**
**      A fogFogObject is a derived class for a storyboard, which is a textured
**	polygon that always faces the camera.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef FOG_FOGOBJECT_HPP
#error fogFogObject.hpp multiply included
#endif
#define FOG_FOGOBJECT_HPP

#ifndef FOG_FOGDATA_HPP
#include "Systems/Fog/Data/fogFogData.hpp"
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
class gpxFog;

//============================================================================
//============================================================================
class fogFogObject : public cmmSelectablePropertyObject
{
	public:
		//--------------------------------------------------------------------
		// This object takes ownership of the arguments passed in
		//--------------------------------------------------------------------
		fogFogObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~fogFogObject();


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
		const fogFogData& GetData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetData(const fogFogData &i_Data);

	//============================================================================
	//	properties
	//============================================================================

		//--------------------------------------------------------------------
		// Mode property access
		//--------------------------------------------------------------------
		prtyEnum&	PropertyMode();
		const prtyEnum&	GetPropertyMode() const;

		//--------------------------------------------------------------------
		// Color property access
		//--------------------------------------------------------------------
		prtyColor&	PropertyColor();
		const prtyColor&	GetPropertyColor() const;

		//--------------------------------------------------------------------
		// Density property access
		//--------------------------------------------------------------------
		prtyFloat& PropertyDensity();
		const prtyFloat& GetPropertyDensity() const;

		//--------------------------------------------------------------------
		// Start property access
		//--------------------------------------------------------------------
		prtyFloat& PropertyStart();
		const prtyFloat& GetPropertyStart() const;

		//--------------------------------------------------------------------
		// End property access
		//--------------------------------------------------------------------
		prtyFloat& PropertyEnd();
		const prtyFloat& GetPropertyEnd() const;

		//--------------------------------------------------------------------
		// AltitudeStart property access
		//--------------------------------------------------------------------
		prtyFloat& PropertyAltitudeStart();
		const prtyFloat& GetPropertyAltitudeStart() const;

		//--------------------------------------------------------------------
		// AltitudeEnd property access
		//--------------------------------------------------------------------
		prtyFloat& PropertyAltitudeEnd();
		const prtyFloat& GetPropertyAltitudeEnd() const;

		//--------------------------------------------------------------------
		// AltitudeDensity property access
		//--------------------------------------------------------------------
		prtyFloat& PropertyAltitudeDensity();
		const prtyFloat& GetPropertyAltitudeDensity() const;

		//--------------------------------------------------------------------
		// bEnable property access
		//--------------------------------------------------------------------
		prtyBoolean& PropertybEnable();
		const prtyBoolean& GetPropertybEnable() const;

		//--------------------------------------------------------------------
		// bWorldOrientation property access
		//--------------------------------------------------------------------
		prtyBoolean& PropertybWorldOrientation ();
		const prtyBoolean& GetPropertybWorldOrientation () const;

		//--------------------------------------------------------------------
		// SetName
		//--------------------------------------------------------------------
		//void SetName(const nameString& i_Name);

private:
		fogFogData		m_Data;
		gpxFog*			m_pFogProxy;
		sel3dObject*	m_pParent;
		float			m_MaxDensity;
		float			m_MaxHeightDensity;

		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void FogChanged(prtyProperty *i_pProperty, bool i_bDirty);
};

