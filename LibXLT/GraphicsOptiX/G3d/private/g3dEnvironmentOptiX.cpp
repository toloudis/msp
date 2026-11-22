/*****************************************************************************
**  g3dEnvironmentOptiX.cpp
**
**      See g3dEnvironmentOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DENVIRONMENTOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dEnvironmentOptiX.hpp"
#endif
#ifndef GOX_G3DSCENEBVHOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dSceneBVHOptiX.hpp"
#endif
#ifndef GOX_G3DTEXTURETABLEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dTextureTableOptiX.hpp"
#endif
#ifndef GOX_OPTXLIGHTING_HPP
#include "GraphicsOptiX/optx/optxLighting.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_RENDERSTATE_HPP
#include "Graphics/G3d/g3dRenderState.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef MA_CONSTANTS_HPP
#include "Core/Ma/maConstants.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <algorithm>
#include <cstring>

namespace gox
{
	namespace
	{
		//	Whether a colour x factor has any positive component
		bool gives_light(const maFloatRGBA& i_Color, float i_Factor)
		{
			for (int c = 0; c < 3; ++c)
			{
				if (i_Color.GetPtr()[c] * i_Factor > 0.0f)
				{
					return true;
				}
			}
			return false;
		}
	}

	bool g3dEnvironmentOptiX::State::operator==(const State& i_Other) const
	{
		return m_pImage == i_Other.m_pImage && m_Generation == i_Other.m_Generation && m_Flags == i_Other.m_Flags &&
			   std::memcmp(m_Color, i_Other.m_Color, sizeof(m_Color)) == 0 && m_Angle == i_Other.m_Angle;
	}

	bool g3dEnvironmentOptiX::Built::operator==(const Built& i_Other) const
	{
		return m_pImage == i_Other.m_pImage && m_Generation == i_Other.m_Generation &&
			   std::memcmp(m_Color, i_Other.m_Color, sizeof(m_Color)) == 0 && m_Width == i_Other.m_Width &&
			   m_Height == i_Other.m_Height;
	}

	g3dEnvironmentOptiX::g3dEnvironmentOptiX()
		: m_Light()
		, m_pMap(nullptr)
		, m_bBuilt(false)
		, m_Version(0)
		, m_nBuilds(0)
		, m_bLoggedMany(false)
		, m_bLoggedMap(false)
	{
		m_Light.m_Texture = c_optxNoTexture;
	}

	g3dEnvironmentOptiX::~g3dEnvironmentOptiX()
	{
	}

	void g3dEnvironmentOptiX::Release()
	{
		m_Table.Free();
		m_bBuilt = false;
		m_Light.m_Table = 0;
	}

	bool g3dEnvironmentOptiX::Update(const g3dScene& i_Scene, const g3dSceneBVHOptiX& i_BVH, g3dTextureTableOptiX& i_Textures)
	{
		const g3dPrefs::g3dRenderPrefs& prefs = g3dPrefs::CurrentPrefs();
		m_Light = optxEnvironmentLight();	// Prepare sets the table
		m_Light.m_Texture = c_optxNoTexture;
		m_pMap = nullptr;
		State state;

		if (prefs.m_bEnableEnvironment)
		{
			//	Which environment: the global one, else the most common
			g3dAmbientEnvState chosen;
			bool global = false;
			g3dAmbientEnvState global_state;
			i_Scene.GetGlobalAmbient(global_state);
			if (global_state.m_bEnableSwlEnv)
			{
				chosen = global_state;
				global = true;
			}
			else
			{
				chosen = i_BVH.GetEnvironment();
				if (i_BVH.GetEnvironmentCount() > 1 && !m_bLoggedMany)
				{
					DBG_LOG("GraphicsOptiX: the traced objects have " << i_BVH.GetEnvironmentCount()
							<< " environments; the beauty pass lights them all with the most common one");
					m_bLoggedMany = true;
				}
			}

			//	Which map: the specular one, else the diffuse one; with neither,
			//	the specular colour if it gives light (MachStudio's environments
			//	default to a black specular colour at factor 1), else the diffuse
			const bool specular_map = chosen.m_SpecularMap != NULL;
			const bool use_specular = specular_map || (!chosen.m_DiffuseMap && gives_light(chosen.m_SpecularColor, chosen.m_SpecularFactor));
			const matTexture* map = use_specular ? chosen.m_SpecularMap : chosen.m_DiffuseMap;
			const maFloatRGBA& color = use_specular ? chosen.m_SpecularColor : chosen.m_DiffuseColor;
			const float factor = use_specular ? chosen.m_SpecularFactor : chosen.m_DiffuseFactor;
			const float angle = use_specular ? chosen.m_SpecularAngle : chosen.m_DiffuseAngle;
			for (int c = 0; c < 3; ++c)
			{
				m_Light.m_Color[c] = std::max(0.0f, color.GetPtr()[c] * factor);
			}
			m_Light.m_Angle = angle * maConstants::c_fAngleToRad;

			unsigned flags = 0;
			const bool gives_light = m_Light.m_Color[0] > 0.0f || m_Light.m_Color[1] > 0.0f || m_Light.m_Color[2] > 0.0f;
			if (gives_light && prefs.m_bEnableDiffuseLighting)
			{
				flags |= e_optxEnvironmentDiffuse;
			}
			if (gives_light && prefs.m_bEnableSpecularLighting)
			{
				flags |= e_optxEnvironmentSpecular;
			}
			if (global && chosen.m_bEnableSwlEnvBG)
			{
				flags |= e_optxEnvironmentBackground;
			}
			bool usable = flags != 0;
			if (usable && map)
			{
				m_Light.m_Texture = i_Textures.Add(map, cuExternalImageCache::e_Environment);
				gvk::vkdImage* image = gvk::g3dVkTextureUtil::GetImage(map);
				if (m_Light.m_Texture == c_optxNoTexture || !image)
				{
					if (!m_bLoggedMap)
					{
						DBG_LOG("GraphicsOptiX: the environment's map can't be sampled; the beauty pass has no environment light");
						m_bLoggedMap = true;
					}
					usable = false;
					m_Light.m_Texture = c_optxNoTexture;
				}
				else
				{
					m_pMap = map;
					state.m_pImage = image;
					state.m_Generation = image->Generation();
				}
			}
			if (usable)
			{
				m_Light.m_Flags = flags | e_optxEnvironmentOn;
			}
		}
		if (!m_Light.m_Flags)
		{
			m_Light = optxEnvironmentLight();
			m_Light.m_Texture = c_optxNoTexture;
			m_pMap = nullptr;
			state = State();
		}
		else
		{
			state.m_Flags = m_Light.m_Flags;
			std::memcpy(state.m_Color, m_Light.m_Color, sizeof(state.m_Color));
			state.m_Angle = m_Light.m_Angle;
		}
		if (state == m_State)
		{
			return false;
		}
		m_State = state;
		++m_Version;
		return true;
	}

	bool g3dEnvironmentOptiX::Prepare(optxLighting& i_Lighting, const g3dTextureTableOptiX& i_Textures)
	{
		m_Light.m_Table = 0;
		const std::vector<optxTexture>& textures = i_Textures.GetTable();
		if (!(m_Light.m_Flags & (e_optxEnvironmentDiffuse | e_optxEnvironmentSpecular)) || m_Light.m_Texture == c_optxNoTexture ||
			m_Light.m_Texture >= textures.size())
		{
			return true;	// no table needed: no light, or the colour alone
		}

		//	The table's size: the map's, halved until it fits; its cells read
		//	as up to c_MaxTaps x c_MaxTaps samples at the level that covers them
		const optxTexture& map = textures[m_Light.m_Texture];
		unsigned level = 0;
		while ((map.m_Width >> level) > c_MaxTableWidth || (map.m_Height >> level) > c_MaxTableHeight)
		{
			++level;
		}
		const unsigned width = std::max(1u, map.m_Width >> level), height = std::max(1u, map.m_Height >> level);
		unsigned tap_shift = 0;
		while ((1u << (tap_shift + 1)) <= c_MaxTaps && tap_shift + 1 <= level)
		{
			++tap_shift;
		}
		m_Light.m_TableWidth = width;
		m_Light.m_TableHeight = height;
		m_Light.m_TableLevel = float(level);
		m_Light.m_Taps = 1u << tap_shift;
		m_Light.m_TapLevel = float(level - tap_shift);

		Built built;
		built.m_pImage = m_State.m_pImage;
		built.m_Generation = m_State.m_Generation;
		std::memcpy(built.m_Color, m_Light.m_Color, sizeof(built.m_Color));
		built.m_Width = width;
		built.m_Height = height;
		if (m_bBuilt && built == m_Built)
		{
			m_Light.m_Table = m_Table.Get();
			return true;
		}

		m_bBuilt = false;
		const size_t bytes = size_t(optxEnvTableFloats(width, height)) * sizeof(float);
		if (!m_Table.Reserve(bytes))
		{
			return false;
		}
		optxLightingParams params = {};
		params.m_Textures = i_Textures.GetTextures();
		params.m_Environment = m_Light;
		params.m_Environment.m_Table = m_Table.Get();
		if (!i_Lighting.BuildEnvironmentTable(params))
		{
			return false;
		}
		m_Built = built;
		m_bBuilt = true;
		++m_nBuilds;
		m_Light.m_Table = m_Table.Get();
		return true;
	}

	std::vector<float> g3dEnvironmentOptiX::ReadTable() const
	{
		std::vector<float> table;
		if (!m_bBuilt || !m_Light.m_Table)
		{
			return table;
		}
		table.resize(optxEnvTableFloats(m_Light.m_TableWidth, m_Light.m_TableHeight));
		if (!m_Table.Download(table.data(), table.size() * sizeof(float)))
		{
			table.clear();
		}
		return table;
	}
}
