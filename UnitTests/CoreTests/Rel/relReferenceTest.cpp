/*****************************************************************************
**  relReferenceTest.cpp
**
**      Object references resolve through the parent relationships, so a
**      reference keeps pointing at "the same place" when the objects
**      behind it are replaced.
**
**      Ported from TestLibXLT/Core/Rel (relReferenceTest,
**      relRelationshipTest and relTestUtil), which only logged the names.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef REL_OBJECT_HPP
#include "Core/rel/relObject.hpp"
#endif
#ifndef REL_RELATIONSHIPMULTIPLE_HPP
#include "Core/rel/relRelationshipMultiple.hpp"
#endif
#ifndef REL_RELATIONSHIPSINGLE_HPP
#include "Core/rel/relRelationshipSingle.hpp"
#endif

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace
{

// Relationships are found by handle, which is compared by pointer
const relHandle c_ChildHandle = "Child";
const relHandle c_VectorHandle = "Vector";

relObject* g_pGlobalObject = nullptr;

//	A reference to whatever g_pGlobalObject currently points at, like a
//	document's root object that is recreated on load.
//
class GlobalReference : public relObjectReference
{
	public:
		relObject* GetObject() const override { return g_pGlobalObject; }
};

class TestClass : public relObject
{
	public:
		explicit TestClass(const char* i_Name) : m_Name(i_Name) {}
		std::string m_Name;
};

class GlobalClass : public TestClass
{
	public:
		explicit GlobalClass(const char* i_Name) : TestClass(i_Name) {}

		shared_ptr<relObjectReference> CreateReferenceToSelf() override
		{
			return shared_ptr<relObjectReference>(new GlobalReference());
		}
};

//	MultipleClass holds a vector of children through a multiple
//	relationship.
//
class MultipleClass : public TestClass
{
	public:
		explicit MultipleClass(const char* i_Name)
			: TestClass(i_Name)
		{
			m_ChildRelationship.reset(new relRelationshipMultiple<TestClass>(c_VectorHandle, *this, m_Children));
			AddRelationship(m_ChildRelationship);
		}

		void AddObject(TestClass* i_pObject)
		{
			m_Children.push_back(i_pObject);
			i_pObject->SetParentRelationship(m_ChildRelationship);
		}

		std::vector<TestClass*> m_Children;
		shared_ptr<relRelationship> m_ChildRelationship;
};

//	MakeParentChild gives io_Parent a single relationship to io_Child.
//	Only the parent owns the relationship; the child holds it weakly.
//
void MakeParentChild(relObject& io_Parent, relObject& io_Child)
{
	shared_ptr<relRelationshipSingle> parent_child(new relRelationshipSingle(c_ChildHandle, io_Parent, io_Child));
	io_Parent.AddRelationship(parent_child);
	io_Child.SetParentRelationship(parent_child);
}

std::string Resolve(const shared_ptr<relObjectReference>& i_Reference)
{
	TestClass* object = dynamic_cast<TestClass*>(i_Reference->GetObject());
	return object ? object->m_Name : std::string("(null)");
}

class relReference : public testing::Test
{
	protected:
		void TearDown() override { g_pGlobalObject = nullptr; }
};

}

TEST_F(relReference, ObjectWithoutParentRefersToItself)
{
	TestClass orphan("Orphan");
	shared_ptr<relObjectReference> ref = orphan.CreateReferenceToSelf();
	EXPECT_EQ("Orphan", Resolve(ref));
	EXPECT_EQ(nullptr, orphan.GetParentObject());
}

TEST_F(relReference, GlobalReferenceFollowsTheGlobal)
{
	GlobalClass thing1("Thing1");
	GlobalClass thing2("Thing2");

	g_pGlobalObject = &thing1;
	shared_ptr<relObjectReference> ref = g_pGlobalObject->CreateReferenceToSelf();
	EXPECT_EQ("Thing1", Resolve(ref));

	g_pGlobalObject = &thing2;
	EXPECT_EQ("Thing2", Resolve(ref));
}

//	A reference to a grandchild goes global -> single relationship ->
//	multiple relationship index, so switching the global to another tree
//	of the same shape resolves to the matching grandchild there.
//
TEST_F(relReference, ChildReferenceResolvesThroughRelationships)
{
	GlobalClass parent1("Parent1"), parent2("Parent2");
	MultipleClass multiple1("Multiple1"), multiple2("Multiple2");
	MakeParentChild(parent1, multiple1);
	MakeParentChild(parent2, multiple2);

	TestClass child1a("Child1a"), child1b("Child1b");
	TestClass child2a("Child2a"), child2b("Child2b");
	multiple1.AddObject(&child1a);
	multiple1.AddObject(&child1b);
	multiple2.AddObject(&child2a);
	multiple2.AddObject(&child2b);

	EXPECT_EQ(&multiple1, child1b.GetParentObject());
	EXPECT_EQ(&parent1, multiple1.GetParentObject());

	g_pGlobalObject = &parent1;
	shared_ptr<relObjectReference> ref = child1b.CreateReferenceToSelf();
	EXPECT_EQ("Child1b", Resolve(ref));

	g_pGlobalObject = &parent2;
	EXPECT_EQ("Child2b", Resolve(ref));
}
