/*****************************************************************************
**  envSTLHelpersTest.cpp
**
**      The envSTLHelpers container utilities.
**
**      Replaces TestLibXLT/Core/Env/envSTLHelpersTest.cpp, which tested
**      MemFun/MemFunRef helpers that no longer exist.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef ENV_STLHELPERS_HPP
#include "Core/env/envSTLHelpers.hpp"
#endif

#include <gtest/gtest.h>

#include <iterator>
#include <list>
#include <utility>
#include <vector>

namespace
{
	//	Counts live instances, to check DeleteContainer/DeleteOneValue.
	struct Counted
	{
		Counted() { ++sm_Live; }
		~Counted() { --sm_Live; }
		static int sm_Live;
	};
	int Counted::sm_Live = 0;
}

TEST(envSTLHelpers, DeleteContainerDeletesAndEmpties)
{
	std::vector<Counted*> objects;
	for( int i = 0 ; i < 10 ; i++ )
		objects.push_back(new Counted);
	ASSERT_EQ(10, Counted::sm_Live);

	envSTLHelpers::DeleteContainer(objects);
	EXPECT_TRUE(objects.empty());
	EXPECT_EQ(0, Counted::sm_Live);
}

TEST(envSTLHelpers, DeleteOneValue)
{
	std::vector<Counted*> objects = { new Counted, new Counted, new Counted };
	Counted* middle = objects[1];

	EXPECT_TRUE(envSTLHelpers::DeleteOneValue(objects, middle));
	EXPECT_EQ(2u, objects.size());
	EXPECT_EQ(2, Counted::sm_Live);
	EXPECT_FALSE(envSTLHelpers::Contains(objects, middle));

	envSTLHelpers::DeleteContainer(objects);
	EXPECT_EQ(0, Counted::sm_Live);
}

TEST(envSTLHelpers, ForAllAndRForAll)
{
	std::vector<int> values = { 1, 2, 3, 4 };

	std::vector<int> forward, backward;
	envSTLHelpers::ForAll(values, [&](int i_Val) { forward.push_back(i_Val); });
	envSTLHelpers::RForAll(values, [&](int i_Val) { backward.push_back(i_Val); });

	EXPECT_EQ(std::vector<int>({ 1, 2, 3, 4 }), forward);
	EXPECT_EQ(std::vector<int>({ 4, 3, 2, 1 }), backward);
}

TEST(envSTLHelpers, ForAllForAllVisitsNestedItems)
{
	std::vector< std::vector<int> > nested = { { 1, 2 }, {}, { 3 } };
	int sum = 0;
	envSTLHelpers::ForAllForAll(nested, [&](int i_Val) { sum += i_Val; });
	EXPECT_EQ(6, sum);
}

TEST(envSTLHelpers, ForEachPairVisitsEachUnorderedPairOnce)
{
	std::vector<int> values = { 1, 2, 3, 4 };
	std::vector< std::pair<int, int> > pairs;
	envSTLHelpers::ForEachPair(values.begin(), values.end(), [&](int i_A, int i_B) { pairs.emplace_back(i_A, i_B); });

	std::vector< std::pair<int, int> > expected = { {1,2}, {1,3}, {1,4}, {2,3}, {2,4}, {3,4} };
	EXPECT_EQ(expected, pairs);

	std::vector<int> empty;
	int calls = 0;
	envSTLHelpers::ForEachPair(empty.begin(), empty.end(), [&](int, int) { ++calls; });
	EXPECT_EQ(0, calls);
}

TEST(envSTLHelpers, RemoveValues)
{
	std::list<int> values = { 1, 2, 3, 2, 4, 2 };

	EXPECT_TRUE(envSTLHelpers::RemoveOneValue(values, 2));
	EXPECT_EQ(std::list<int>({ 1, 3, 2, 4, 2 }), values);

	EXPECT_FALSE(envSTLHelpers::RemoveOneValue(values, 9));

	envSTLHelpers::RemoveAllValues(values, 2);
	EXPECT_EQ(std::list<int>({ 1, 3, 4 }), values);
	EXPECT_TRUE(envSTLHelpers::Contains(values, 3));
	EXPECT_FALSE(envSTLHelpers::Contains(values, 2));
}

TEST(envSTLHelpers, CopyIf)
{
	std::vector<int> values = { 1, 2, 3, 4, 5, 6 };
	std::vector<int> even;
	envSTLHelpers::copy_if(values.begin(), values.end(), std::back_inserter(even), [](int i_Val) { return i_Val % 2 == 0; });
	EXPECT_EQ(std::vector<int>({ 2, 4, 6 }), even);
}
