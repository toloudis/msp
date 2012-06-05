/*****************************************************************************
**  relReferenceTest.cpp
**
**      relReferenceTest is a test of envBoost.hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "relReferenceTest.hpp"
#include "relTestUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"

#include <string>
#include <vector>

//====================================================================
// Anonymous Namespace for local variables and functions
//====================================================================
namespace 
{

}

//========================================================================
//	RunTest - executes the envSTLHelpers tests
//========================================================================
void relReferenceTest::RunTest()
{
	DBG_LOG("Begin relReferenceTest::RunTest()");

	DBG_LOG("\nCreating two global reference test objects");
	GlobalClass thing1("Thing1");
	GlobalClass thing2("Thing2");

	DBG_LOG("\nPoint global object at thing1, ask for reference");
	g_pGlobalObject = &thing1;
	shared_ptr<relObjectReference> global_ref = g_pGlobalObject->CreateReferenceToSelf();

	DBG_LOG("\nResolve reference, print name");
	resolve_reference(global_ref);

	DBG_LOG("\nPoint global object at thing2");
	g_pGlobalObject = &thing2;

	DBG_LOG("\nResolve old reference, should point to thing2 now");
	resolve_reference(global_ref);

	DBG_LOG("\nEnd relReferenceTest::RunTest()");
}
