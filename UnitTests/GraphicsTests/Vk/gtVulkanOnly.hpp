/*****************************************************************************
**  gtVulkanOnly.hpp
**
**      The material shaders GraphicsVulkan has and GraphicsDX11 doesn't
**      (Pbr: DX11 draws Pbr.fx materials as Blinn). The tests that compare
**      the Vulkan shaders and effects with DX11's leave these out by name;
**      fxShaderCopy.IdenticalToDX11 checks that every listed file exists,
**      so the list can't go stale. Suites that test Vulkan alone keep them.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace gtVulkanOnly
{
	//	Paths relative to Eff/private/Shaders, '/' separators
	inline const std::vector<std::string>& Files()
	{
		static const std::vector<std::string> c_Files =
		{
			"Materials/Pbr.hlsl",
			"Materials/Pbr.effect.json",
		};
		return c_Files;
	}

	//	i_RelPath relative to Eff/private/Shaders, '/' separators
	inline bool IsVulkanOnlyFile(const std::string& i_RelPath)
	{
		const std::vector<std::string>& files = Files();
		return std::find(files.begin(), files.end(), i_RelPath) != files.end();
	}

	//	i_Name an effect name without ".fx" ("Pbr"); an effect is Vulkan-only
	//	when its manifest is
	inline bool IsVulkanOnlyEffect(const std::string& i_Name)
	{
		for (const std::string& file : Files())
		{
			const size_t slash = file.find_last_of('/');
			const std::string base = file.substr(slash == std::string::npos ? 0 : slash + 1);
			if (base == i_Name + ".effect.json")
				return true;
		}
		return false;
	}

	//	i_ShaderName a material shader's file name ("Pbr.fx"), as
	//	matShaderMgr lists them
	inline bool IsVulkanOnlyMaterial(const std::string& i_ShaderName)
	{
		const size_t dot = i_ShaderName.rfind(".fx");
		return dot != std::string::npos && dot + 3 == i_ShaderName.size() &&
			   IsVulkanOnlyEffect(i_ShaderName.substr(0, dot));
	}
}
