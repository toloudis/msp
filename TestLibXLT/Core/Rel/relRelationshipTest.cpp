/*****************************************************************************
**  relRelationshipTest.cpp
**
**      relRelationshipTest is a test of envBoost.hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "relRelationshipTest.hpp"
#include "relTestUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/rel/relRelationshipMultiple.hpp"
#include "Core/rel/relRelationshipSingle.hpp"

#include <string>
#include <vector>

//====================================================================
// Anonymous Namespace for local variables and functions
//====================================================================
namespace 
{
	//===============================================================
	// This class contains a vector of its children
	// and needs to define a multiple relationship for that.
	//===============================================================
	class MultipleClass : public TestClass
	{
		public:
			//------------------------------------------------------------
			//------------------------------------------------------------
			MultipleClass(const char* i_Name)
				: TestClass(i_Name) 
			{
				// Define the relationship between this object and its children
				m_ChildRelationship.reset(new relRelationshipMultiple<TestClass>("Vector", *this, m_Children));
				this->AddRelationship(m_ChildRelationship);
			}

			//------------------------------------------------------------
			//------------------------------------------------------------
			void AddObject(TestClass* i_pObject)
			{
				m_Children.push_back(i_pObject);
				i_pObject->SetParentRelationship( m_ChildRelationship );
			}

			std::vector<TestClass*> m_Children;
			shared_ptr<relRelationship> m_ChildRelationship;
	};

	void create_relationship(relObject &io_Object1, relObject &io_Object2)
	{
		// Create relationship between two objects
		shared_ptr<relRelationshipSingle> parent_child(new relRelationshipSingle("Child", io_Object1, io_Object2));

		// Only the parent object owns the relationship
		io_Object1.AddRelationship( parent_child );

		// The child relationship is through a weak_ptr
		io_Object2.SetParentRelationship( parent_child );
	}
}

//========================================================================
//	RunTest - executes the envSTLHelpers tests
//========================================================================
void relRelationshipTest::RunTest()
{
	DBG_LOG("Begin relRelationshipTest::RunTest()");

	DBG_LOG("\nCreating two parent global reference test objects");
	GlobalClass parent1("Parent1"), parent2("Parent2");

	DBG_LOG("\nCreating two multiple relationship test objects");
	MultipleClass multiple1("Multiple1"), multiple2("Multiple2");

	DBG_LOG("\nCreating parenting relationships");
	create_relationship(parent1, multiple1);
	create_relationship(parent2, multiple2);

	DBG_LOG("\nCreating two child test objects (not global references) for each multiple");
	TestClass child1a("Child1a"), child1b("Child1b");
	TestClass child2a("Child1a"), child2b("Child2b");

	DBG_LOG("\nAdding children to multiple objects");
	multiple1.AddObject( &child1a );
	multiple1.AddObject( &child1b );
	multiple2.AddObject( &child2a );
	multiple2.AddObject( &child2b );

	DBG_LOG("\nPoint global object at parent1, ask for reference from one of the leaf child objects");
	g_pGlobalObject = &parent1;
	shared_ptr<relObjectReference> global_ref = child1b.CreateReferenceToSelf();

	DBG_LOG("\nResolve reference, print name");
	resolve_reference(global_ref);

	DBG_LOG("\nPoint global object at parent2");
	g_pGlobalObject = &parent2;

	DBG_LOG("\nResolve old child reference, should point to parent2's grandchild child2b now");
	resolve_reference(global_ref);

	DBG_LOG("\nEnd relRelationshipTest::RunTest()");
}
