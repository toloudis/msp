/********************************************************************************************\
**  swlPropertyObject.hpp
**
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#ifdef SWL_PROPERTYOBJECT_HPP
#error swlPropertyObject.hpp multiply included
#endif
#define SWL_PROPERTYOBJECT_HPP

#ifndef CMM_SELECTABLEPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmSelectablePropertyObject.hpp"
#endif

#ifndef SWL_INTEREST_HPP
#include "Support/swl/swlInterest.hpp"
#endif

#include <vector>

//============================================================================
//============================================================================
class swlPropertyObject : public cmmSelectablePropertyObject
{
public:
	//------------------------------------------------------------------------
	// Constructor
	//------------------------------------------------------------------------
	swlPropertyObject(const std::string& i_Name, swlData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~swlPropertyObject();

	//------------------------------------------------------------------------
	// Quick access to name given in constructor
	//------------------------------------------------------------------------
	inline const std::string&  GetName() const;

	//------------------------------------------------------------------------
	// Get the name of the object for display when selected
	//------------------------------------------------------------------------
	virtual std::string GetDisplayName() const;

	//--------------------------------------------------------------------
	//	Register interest
	//--------------------------------------------------------------------
	void RegisterInterest(swlInterest* i_Interest);

private:
	//------------------------------------------------------------------------
	//	data - this is a ref to a data object owned externally.
	//------------------------------------------------------------------------
	swlData& m_Data; 
	std::string m_Name;
	std::vector<swlInterest*> m_Interest;

	void UpdateFlags(prtyProperty *i_pProperty, bool i_bDirty);
};

//------------------------------------------------------------------------
// Quick access to name given in constructor
//------------------------------------------------------------------------
inline const std::string& swlPropertyObject::GetName() const
{
	return m_Name;
}
