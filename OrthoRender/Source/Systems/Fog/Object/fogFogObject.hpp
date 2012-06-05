/*****************************************************************************
**  fogFogObject.hpp
**
**      A fogFogObject is a derived class for a storyboard, which is a textured
**	polygon that always faces the camera.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef FOG_FOGOBJECT_HPP
#error fogFogObject.hpp multiply included
#endif
#define FOG_FOGOBJECT_HPP

#ifndef FOG_FOGDATA_HPP
#include "Systems/Fog/Data/fogFogData.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class prtyName;
class prtyEnum;
class prtyColor;
class prtyFloat;

//============================================================================
//============================================================================
class fogFogObject : public pick3dPickObject, public prtyObject, public nameObject
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
		virtual std::string GetPick3dName() const;

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
		// SetName
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);

private:
		fogFogData		m_Data;

		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void FogChanged(prtyProperty *i_pProperty, bool i_bDirty);
};

