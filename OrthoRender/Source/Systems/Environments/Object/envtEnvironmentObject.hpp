/*****************************************************************************
**  envtEnvironmentObject.hpp
**
**      A envtEnvironmentObject is base class for objects that can be positioned
**  by manipulation modes.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef ENVT_ENVIRONMENTOBJECT_HPP
#error envtEnvironmentObject.hpp multiply included
#endif
#define ENVT_ENVIRONMENTOBJECT_HPP

#ifndef ENVT_DATA_HPP
#include "Systems/Environments/Data/envtData.hpp"
#endif

#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif

#include <vector>


//============================================================================
//	forward references
//============================================================================
class evmtEnvironment;
class nameString;
class prtyFileChooserUIInfo;


//============================================================================
//============================================================================
class envtEnvironmentObject : 
	public nameObject, 
	public prtyObject,
	public pick3dPickObject
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
		virtual std::string GetPick3dName() const;

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		virtual pick3dPickObject* GetParentObject() const;
		void SetParentObject(pick3dPickObject* i_pPO);


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

	//============================================================================
	//	3D icon/geometry
	//============================================================================

		//--------------------------------------------------------------------
		// SetName
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void Add(const nameString& i_ObjectName);
		void Remove(const nameString& i_ObjectName);
		void RefreshNames();

	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ValueChanged(prtyProperty *i_pProperty, bool i_bDirty);

		envtData			m_Data;
		evmtEnvironment*	m_pEnvironment;

		pick3dPickObject*	m_pParent;

		prtyFileChooserUIInfo* m_pDiffuseFileChooser;
		prtyFileChooserUIInfo* m_pSpecularFileChooser;
};


