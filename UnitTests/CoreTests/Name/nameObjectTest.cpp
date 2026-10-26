#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef NAME_MGR_HPP
#include "Core/name/nameMgr.hpp"
#endif
#ifndef NAME_NAMEINTEREST_HPP
#include "Core/name/nameNameInterest.hpp"
#endif
#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiMessageBox.hpp"
#endif

#include <gtest/gtest.h>

namespace
{
	int l_Messages = 0;

	class nameObjectTest : public testing::Test, public nameNameInterest
	{
	protected:
		void SetUp() override
		{
			l_Messages = 0;
			nameMgr::RegisterNameInterest(this);
		}
		void TearDown() override
		{
			nameMgr::UnRegisterNameInterest(this);
		}
		nameObject* GetObjectByName(const nameString&) override { return nullptr; }
		void GetNameList(nameList& io_Names) override
		{
			++m_ListQueries;
			for (const nameString& name : m_Names)
				io_Names.push_back(&name);
		}
		void GetNameString(nameUID, std::string&) override { ++m_UIDQueries; }

		std::vector<nameString> m_Names;
		int m_ListQueries = 0;
		int m_UIDQueries = 0;
	};
}

// Core's nameObject uses this Tool hook; tests record the notification
// without linking a UI toolkit or displaying a modal dialog.
int guiMessageBox::Show(const char*, const char*, int)
{
	++l_Messages;
	return guiMessageBox::e_OK;
}

TEST_F(nameObjectTest, AssignsUIDToNewName)
{
	nameObject object;
	object.SetName(nameString(std::string("New")));
	EXPECT_NE(nameString::e_InvalidUID, object.GetName().GetUID());
	EXPECT_EQ("New", object.GetName().GetString());
	EXPECT_EQ(0, m_ListQueries);
	EXPECT_EQ(0, m_UIDQueries);
}

TEST_F(nameObjectTest, SparseLargeUIDScansLiveNamesOnce)
{
	m_Names.emplace_back(std::string("Existing"), 2);
	nameObject object;
	object.SetName(nameString(std::string("New"), 1000000000));
	EXPECT_EQ("New", object.GetName().GetString());
	EXPECT_EQ(1000000000, object.GetName().GetUID());
	EXPECT_EQ(1, m_ListQueries);
	EXPECT_EQ(0, m_UIDQueries);
	EXPECT_EQ(0, l_Messages);
}

TEST_F(nameObjectTest, UnchangedRegisteredNameDoesNotSearchAgain)
{
	nameObject object;
	object.SetName(nameString(std::string("Name"), 17));
	m_ListQueries = 0;
	object.SetName(object.GetName());
	EXPECT_EQ(0, m_ListQueries);
	EXPECT_EQ(0, m_UIDQueries);
	EXPECT_EQ(17, object.GetName().GetUID());
}

TEST_F(nameObjectTest, EarlierDuplicateKeepsItsName)
{
	m_Names.emplace_back(std::string("Taken"), 3);
	nameObject object;
	object.SetName(nameString(std::string("Taken"), 20));
	EXPECT_EQ("Taken.copy", object.GetName().GetString());
	EXPECT_EQ(20, object.GetName().GetUID());
	EXPECT_EQ(1, l_Messages);
}

TEST_F(nameObjectTest, LaterOrInvalidUIDDoesNotRenameEarlierObject)
{
	m_Names.emplace_back(std::string("Taken"), 30);
	m_Names.emplace_back(std::string("Taken"), nameString::e_InvalidUID);
	nameObject object;
	object.SetName(nameString(std::string("Taken"), 20));
	EXPECT_EQ("Taken", object.GetName().GetString());
	EXPECT_EQ(0, l_Messages);
}

TEST_F(nameObjectTest, SameUIDCanChangeItsText)
{
	m_Names.emplace_back(std::string("Renamed"), 20);
	nameObject object;
	object.SetName(nameString(std::string("Old"), 20));
	object.SetName(nameString(std::string("Renamed"), 20));
	EXPECT_EQ("Renamed", object.GetName().GetString());
	EXPECT_EQ(20, object.GetName().GetUID());
	EXPECT_EQ(0, l_Messages);
}
