/*****************************************************************************
**  iscnSceneChoiceTest.cpp
**
**      iscnSceneChoice: the Import dialog's object tree, its labels, and
**      a chosen object bringing its ancestor groups.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef ISCN_SCENECHOICE_HPP
#include "ImportExport/iscn/iscnSceneChoice.hpp"
#endif
#ifndef ISCN_SCENEDESC_HPP
#include "ImportExport/iscn/iscnSceneDesc.hpp"
#endif

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace
{
	enum { e_Root, e_Car, e_Wheel, e_Lamp, e_Cam, e_Count };

	iscnObjectDesc Object(iscnObjectDesc::Kind i_Kind, const char* i_Name, int i_Parent)
	{
		iscnObjectDesc object;
		object.m_Kind = i_Kind;
		object.m_Name = i_Name;
		object.m_Parent = i_Parent;
		return object;
	}

	iscnClipDesc Clip(const char* i_Name)
	{
		iscnClipDesc clip;
		clip.m_Identity = i_Name;
		clip.m_Name = i_Name;
		return clip;
	}

	// Root group > Car group > Wheel mesh (keyed, clips Drive and Idle); Lamp at the top; Cam under Root
	iscnSceneDesc Scene()
	{
		iscnSceneDesc scene;
		scene.m_Objects.push_back(Object(iscnObjectDesc::e_Group, "Root", -1));
		scene.m_Objects.push_back(Object(iscnObjectDesc::e_Group, "Car", e_Root));
		iscnObjectDesc wheel = Object(iscnObjectDesc::e_Mesh, "Wheel", e_Car);
		wheel.m_Keys.push_back(iscnKeyTrack());
		wheel.m_Clips.push_back(Clip("Drive"));
		wheel.m_Clips.push_back(Clip("Idle"));
		scene.m_Objects.push_back(wheel);
		scene.m_Objects.push_back(Object(iscnObjectDesc::e_PointLight, "Lamp", -1));
		scene.m_Objects.push_back(Object(iscnObjectDesc::e_Camera, "Cam", e_Root));
		return scene;
	}

	std::vector<bool> Only(std::initializer_list<int> i_Indices)
	{
		std::vector<bool> chosen(e_Count, false);
		for (int i : i_Indices)
			chosen[i] = true;
		return chosen;
	}
}

TEST(iscnSceneChoice, KindLabels)
{
	EXPECT_EQ("Group", iscnSceneChoice::KindLabel(iscnObjectDesc::e_Group));
	EXPECT_EQ("Mesh", iscnSceneChoice::KindLabel(iscnObjectDesc::e_Mesh));
	EXPECT_EQ("Point Light", iscnSceneChoice::KindLabel(iscnObjectDesc::e_PointLight));
	EXPECT_EQ("Spot Light", iscnSceneChoice::KindLabel(iscnObjectDesc::e_SpotLight));
	EXPECT_EQ("Directional Light", iscnSceneChoice::KindLabel(iscnObjectDesc::e_DirectionalLight));
	EXPECT_EQ("Camera", iscnSceneChoice::KindLabel(iscnObjectDesc::e_Camera));
}

TEST(iscnSceneChoice, ItemLabel)
{
	const iscnSceneDesc scene = Scene();
	EXPECT_EQ("Root (Group)", iscnSceneChoice::ItemLabel(scene.m_Objects[e_Root]));
	EXPECT_EQ("Lamp (Point Light)", iscnSceneChoice::ItemLabel(scene.m_Objects[e_Lamp]));
	EXPECT_EQ("Wheel (Mesh; keyed; clips: Drive, Idle)", iscnSceneChoice::ItemLabel(scene.m_Objects[e_Wheel]));

	iscnObjectDesc clips_only = Object(iscnObjectDesc::e_Mesh, "X", -1);
	clips_only.m_Clips.push_back(Clip("Drive"));
	EXPECT_EQ("X (Mesh; clips: Drive)", iscnSceneChoice::ItemLabel(clips_only));
}

TEST(iscnSceneChoice, Children)
{
	const iscnSceneDesc scene = Scene();
	EXPECT_EQ((std::vector<int>{e_Root, e_Lamp}), iscnSceneChoice::Children(scene, -1));
	EXPECT_EQ((std::vector<int>{e_Car, e_Cam}), iscnSceneChoice::Children(scene, e_Root));
	EXPECT_EQ((std::vector<int>{e_Wheel}), iscnSceneChoice::Children(scene, e_Car));
	EXPECT_TRUE(iscnSceneChoice::Children(scene, e_Wheel).empty());
}

TEST(iscnSceneChoice, WithAncestorsAddsGroups)
{
	EXPECT_EQ(Only({e_Root, e_Car, e_Wheel}), iscnSceneChoice::WithAncestors(Scene(), Only({e_Wheel})));
}

TEST(iscnSceneChoice, WithAncestorsKeepsChoice)
{
	const iscnSceneDesc scene = Scene();
	EXPECT_EQ(Only({}), iscnSceneChoice::WithAncestors(scene, Only({})));
	const std::vector<bool> all(e_Count, true);
	EXPECT_EQ(all, iscnSceneChoice::WithAncestors(scene, all));
	EXPECT_EQ(Only({e_Lamp}), iscnSceneChoice::WithAncestors(scene, Only({e_Lamp})));
}

TEST(iscnSceneChoice, WithAncestorsShortInput)
{
	const std::vector<bool> result = iscnSceneChoice::WithAncestors(Scene(), std::vector<bool>{false, false, true});
	EXPECT_EQ(Only({e_Root, e_Car, e_Wheel}), result);
	EXPECT_EQ((size_t)e_Count, result.size());
}
