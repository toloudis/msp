/*****************************************************************************
**  relTestUtil.cpp
**
**
**	Extra Large Technology
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "relTestUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"

//===============================================================
// Global object pointer to use with global reference below
//===============================================================
relObject *g_pGlobalObject = NULL;



//===============================================================
//===============================================================
void resolve_reference(shared_ptr<relObjectReference> i_GlobalReference)
{
	relObject *pObject = i_GlobalReference->GetObject();
	if (pObject)
	{
		TestClass *pTestObj = dynamic_cast<TestClass*>(pObject);
		if (pTestObj)
			DBG_LOG("Reference to: " << pTestObj->m_String);
		else
			DBG_LOG("Reference is not TestClass");
	}
	else
		DBG_LOG("Reference is NULL");
}
