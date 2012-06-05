/*****************************************************************************
**	xtraScriptObject.hpp
**
**	This class provides the ability to add custom properties and channels
**	to an object at run-time.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef XTRA_SCRIPTOBJECT_HPP
#error xtraScriptObject.hpp multiply included
#endif
#define XTRA_SCRIPTOBJECT_HPP

#ifndef TMLN_SCRIPTOBJECT_HPP
#include "Support/tmln/tmlnScriptObject.hpp"
#endif 
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <vector>

//============================================================================
//	forward references
//============================================================================
class prtyObject;
class prtyProperty;
class prtyPropertyUIInfo;
class xtraPropertyData;


//============================================================================
//============================================================================
class xtraScriptObject : public tmlnScriptObject
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	xtraScriptObject();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~xtraScriptObject();

	//--------------------------------------------------------------------
	// Set pointer to the object that should receive the custom
	//	properties.
	//--------------------------------------------------------------------
	void SetExtraPropertyObject(prtyObject *i_pObject);

	//--------------------------------------------------------------------
	// Returns true if this object already has a property with the 
	//	given name. Custom properties have to have unique names.
	//--------------------------------------------------------------------
	bool HasPropertyWithName(const std::string& i_AttributeName);

	//--------------------------------------------------------------------
	// Get index for custom property with given name.
	//--------------------------------------------------------------------
	int GetIndexForName(const std::string& i_AttributeName);

	//--------------------------------------------------------------------
	//	Adds new property to object according to given data
	//--------------------------------------------------------------------
	bool AddCustomProperty(const xtraPropertyData &i_Data);

	//--------------------------------------------------------------------
	// Delete custom property with given index
	//--------------------------------------------------------------------
	void DeleteCustomProperty(const std::string& i_PropertyName, 
							  bool i_bDeletePropertyDrivers);
	void DeleteCustomProperty(int i_Index, 
							  bool i_bDeletePropertyDrivers);

	//--------------------------------------------------------------------
	// Get vector of custom properties in order to store info to file
	//--------------------------------------------------------------------
	void GetCustomPropertyData(std::vector<shared_ptr<xtraPropertyData>> &o_Data) const;

	//--------------------------------------------------------------------
	// Create custom properties to match data list
	//--------------------------------------------------------------------
	void SetCustomPropertyData(const std::vector<shared_ptr<xtraPropertyData>> &i_Data);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int GetNumCustomProperties() const;

	//--------------------------------------------------------------------
	// Access to channel - this may return NULL if the property
	// cannot be animated.
	//--------------------------------------------------------------------
	const tmlnChannel* GetCustomChannel(int i_Index) const;
	tmlnChannel* GetCustomChannel(int i_Index);

private:
	prtyObject* m_pObject;
	std::vector< shared_ptr<xtraPropertyData> > m_Data;
	std::vector< shared_ptr<prtyPropertyUIInfo> > m_UIInfos;
	std::vector<tmlnChannel*> m_Channels;

};
