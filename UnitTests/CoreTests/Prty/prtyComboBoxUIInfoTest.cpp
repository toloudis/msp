/*****************************************************************************
**  prtyComboBoxUIInfoTest.cpp
**
**      An integer property's value and the combo box item that shows it
**      (prtyComboBoxUIInfo::ValueOfItem and ItemOfValue): numbered items
**      stand for their numbers, named items for their indices.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef PRTY_COMBOBOXUIINFO_HPP
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#endif

#include <gtest/gtest.h>

#include <string>
#include <vector>

TEST(prtyComboBoxUIInfo, NamedItemsStandForTheirIndices)
{
	//	Pbr.fx's Alpha Mode (effPbrData::AlphaMode: 0 Opaque, 1 Mask, 2 Blend)
	const std::vector<std::string> items = { "Opaque", "Mask", "Blend" };
	EXPECT_EQ(0, prtyComboBoxUIInfo::ValueOfItem(items, 0));
	EXPECT_EQ(1, prtyComboBoxUIInfo::ValueOfItem(items, 1));
	EXPECT_EQ(2, prtyComboBoxUIInfo::ValueOfItem(items, 2));
	EXPECT_EQ(0, prtyComboBoxUIInfo::ItemOfValue(items, 0));
	EXPECT_EQ(2, prtyComboBoxUIInfo::ItemOfValue(items, 2));
	EXPECT_EQ(-1, prtyComboBoxUIInfo::ItemOfValue(items, 3));
	EXPECT_EQ(-1, prtyComboBoxUIInfo::ItemOfValue(items, -1));
}

TEST(prtyComboBoxUIInfo, NumberedItemsStandForTheirNumbers)
{
	const std::vector<std::string> items = { "256", "512", "1024", "-4" };
	EXPECT_EQ(256, prtyComboBoxUIInfo::ValueOfItem(items, 0));
	EXPECT_EQ(1024, prtyComboBoxUIInfo::ValueOfItem(items, 2));
	EXPECT_EQ(-4, prtyComboBoxUIInfo::ValueOfItem(items, 3));
	EXPECT_EQ(1, prtyComboBoxUIInfo::ItemOfValue(items, 512));
	EXPECT_EQ(3, prtyComboBoxUIInfo::ItemOfValue(items, -4));
	EXPECT_EQ(-1, prtyComboBoxUIInfo::ItemOfValue(items, 1)) << "an index isn't a value here";
}

TEST(prtyComboBoxUIInfo, AnyNamedItemMakesTheListNamed)
{
	//	"2 sides" isn't a whole number, so all items stand for their indices
	const std::vector<std::string> items = { "1", "2 sides", "4" };
	EXPECT_EQ(2, prtyComboBoxUIInfo::ValueOfItem(items, 2));
	EXPECT_EQ(2, prtyComboBoxUIInfo::ItemOfValue(items, 2));
}

TEST(prtyComboBoxUIInfo, OutOfRangeAndEmpty)
{
	const std::vector<std::string> items = { "Opaque", "Mask" };
	EXPECT_EQ(0, prtyComboBoxUIInfo::ValueOfItem(items, -1)) << "nothing selected";
	EXPECT_EQ(0, prtyComboBoxUIInfo::ValueOfItem(items, 2));
	const std::vector<std::string> none;
	EXPECT_EQ(0, prtyComboBoxUIInfo::ValueOfItem(none, 0));
	EXPECT_EQ(-1, prtyComboBoxUIInfo::ItemOfValue(none, 0));
}
