/****************************************************************************\
**  iscnSceneChoice.cpp
**
**      see iscnSceneChoice.hpp
**
**	Extra Large Technology
\****************************************************************************/
#ifndef ISCN_SCENECHOICE_HPP
#include "ImportExport/iscn/iscnSceneChoice.hpp"
#endif

#include <algorithm>

namespace iscnSceneChoice
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::string KindLabel(iscnObjectDesc::Kind i_Kind)
	{
		switch (i_Kind)
		{
			case iscnObjectDesc::e_Group:				return "Group";
			case iscnObjectDesc::e_Mesh:				return "Mesh";
			case iscnObjectDesc::e_PointLight:			return "Point Light";
			case iscnObjectDesc::e_SpotLight:			return "Spot Light";
			case iscnObjectDesc::e_DirectionalLight:	return "Directional Light";
			case iscnObjectDesc::e_Camera:				return "Camera";
		}
		return std::string();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::string ItemLabel(const iscnObjectDesc& i_Object)
	{
		std::string label = i_Object.m_Name + " (" + KindLabel(i_Object.m_Kind);
		if (!i_Object.m_Keys.empty())
			label += "; keyed";
		for (size_t i = 0; i < i_Object.m_Clips.size(); ++i)
			label += (i == 0 ? "; clips: " : ", ") + i_Object.m_Clips[i].m_Name;
		return label + ")";
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::vector<int> Children(const iscnSceneDesc& i_Scene, int i_Parent)
	{
		std::vector<int> children;
		for (size_t i = 0; i < i_Scene.m_Objects.size(); ++i)
		{
			if (i_Scene.m_Objects[i].m_Parent == i_Parent)
				children.push_back((int)i);
		}
		return children;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::vector<bool> WithAncestors(const iscnSceneDesc& i_Scene, const std::vector<bool>& i_Chosen)
	{
		const size_t count = i_Scene.m_Objects.size();
		std::vector<bool> chosen(i_Chosen.begin(), i_Chosen.begin() + std::min(i_Chosen.size(), count));
		chosen.resize(count, false);

		// parents come before their children, so walking back up reaches
		// each child before its parent
		for (size_t i = count; i-- > 0; )
		{
			const int parent = i_Scene.m_Objects[i].m_Parent;
			if (chosen[i] && parent >= 0 && parent < (int)count)
				chosen[parent] = true;
		}
		return chosen;
	}
}
