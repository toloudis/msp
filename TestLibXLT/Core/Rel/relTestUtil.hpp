/*****************************************************************************
**  relTestUtil.hpp
**
**      relTestUtil provides classes for the other test modes
**
**	Extra Large Technology
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef REL_TESTUTIL_HPP
#error relTestUtil.hpp multiply included
#endif
#define REL_TESTUTIL_HPP

#ifndef REL_OBJECT_HPP
#include "Core/rel/relObject.hpp"
#endif 

//===============================================================
// Global object pointer to use with global reference below
//===============================================================
extern relObject *g_pGlobalObject;

//===============================================================
// Returns a global reference
//===============================================================
class GlobalReference : public relObjectReference
{
public:
	//--------------------------------------------------------------------
	//	GetObject - return a pointer to an object that is usable
	//	for a short period of time. 
	//--------------------------------------------------------------------
	virtual relObject* GetObject() const
	{
		return g_pGlobalObject;
	}
};

//===============================================================
// Class with a string so we can tell instances apart.
//===============================================================
class TestClass : public relObject
{
	public:
		//------------------------------------------------------------
		//------------------------------------------------------------
		TestClass(const char* i_Name)
			: m_String(i_Name)
		{
		}
		std::string m_String;
};

//===============================================================
// This class is defined to use the global reference to
// refer to itself for testing purposes.
//===============================================================
class GlobalClass : public TestClass
{
	public:
		//------------------------------------------------------------
		//------------------------------------------------------------
		GlobalClass(const char* i_Name)
			: TestClass(i_Name) {}

		//------------------------------------------------------------
		//	CreateReferenceToSelf - create a reference that refers to the
		//	given object. This reference should be able to refer to 
		//  future instances of this object persistent through deletion
		//	and recreation through undo.
		//------------------------------------------------------------
		virtual shared_ptr<relObjectReference> CreateReferenceToSelf()
		{
			shared_ptr<relObjectReference> global_ref(new GlobalReference());
			return global_ref;
		}
};

//===============================================================
//===============================================================
extern void resolve_reference(shared_ptr<relObjectReference> i_GlobalReference);