/*****************************************************************************
**  g3dLightTableOptiX.cpp
**
**      See g3dLightTableOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DLIGHTTABLEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dLightTableOptiX.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef G3D_DIRECTIONALLIGHT_HPP
#include "Graphics/G3d/g3dDirectionalLight.hpp"
#endif
#ifndef G3D_LIGHT_HPP
#include "Graphics/G3d/g3dLight.hpp"
#endif
#ifndef G3D_POINTLIGHT_HPP
#include "Graphics/G3d/g3dPointLight.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
#endif
#ifndef MA_CONSTANTS_HPP
#include "Core/Ma/maConstants.hpp"
#endif

#include <algorithm>
#include <cmath>
#include <cstring>

namespace gox
{
	namespace
	{
		void set3(float o_V[3], float x, float y, float z)
		{
			o_V[0] = x;
			o_V[1] = y;
			o_V[2] = z;
		}

		void set_direction(float o_V[3], const maVector3d& i_Direction)
		{
			maVector3d d = i_Direction;
			if (!d.Normalize())
			{
				d.Set(0, 0, 1);
			}
			set3(o_V, d.m_X, d.m_Y, d.m_Z);
		}

		template <class T>
		bool same(const std::vector<T>& a, const std::vector<T>& b)
		{
			return a.size() == b.size() && (a.empty() || std::memcmp(a.data(), b.data(), a.size() * sizeof(T)) == 0);
		}
	}

	optxLight g3dLightTableOptiX::Describe(const g3dLight& i_Light, bool i_bHeadlight)
	{
		optxLight light;
		std::memset(&light, 0, sizeof(light));
		light.m_Falloff[0] = 1.0f;
		light.m_Direction[2] = 1.0f;

		//	effShaderBaseVk's get_light_info: colour times intensity factor
		if (i_Light.IsDiffuseEnabled() && g3dPrefs::CurrentPrefs().m_bEnableDiffuseLighting)
		{
			const maFloatRGBA& color = i_Light.GetIntensity();
			const float factor = i_Light.GetIntensityFactor();
			set3(light.m_Color, color.GetRed() * factor, color.GetGreen() * factor, color.GetBlue() * factor);
		}

		const bool shadows = i_Light.GetShadowSource() && !i_bHeadlight;
		if (const g3dProjectedLight* projected = dynamic_cast<const g3dProjectedLight*>(&i_Light))
		{
			const bool directional = projected->GetIsDirectional();
			light.m_Type = directional ? e_optxLightProjectedDirectional : e_optxLightProjected;
			const maPoint3d& position = projected->GetPosition();
			set3(light.m_Position, position.m_X, position.m_Y, position.m_Z);
			set_direction(light.m_Direction, projected->GetDirection());
			light.m_Falloff[0] = projected->GetFalloff0();
			light.m_Falloff[1] = projected->GetFalloff1();
			light.m_Falloff[2] = projected->GetFalloff2();
			light.m_Falloff[3] = projected->GetFalloff3();
			light.m_FalloffStart = projected->GetFalloffStart();
			//	effShaderBaseVk::SetupSingleLight's near and far
			light.m_Far = directional ? projected->GetRange() : projected->GetScale() + projected->GetRange();
			light.m_InnerAngle = projected->GetInnerAngle() * maConstants::c_fAngleToRad;
			light.m_OuterAngle = projected->GetAngle() * maConstants::c_fAngleToRad;
			light.m_Aspect = projected->GetAspect();
			const maMatrix4x4& matrix = projected->GetTotalMatrix();
			for (int r = 0; r < 4; ++r)
			{
				for (int c = 0; c < 4; ++c)
				{
					light.m_Matrix[r * 4 + c] = matrix(r, c);
				}
			}
			light.m_Radius = std::max(0.0f, projected->GetLightSize() * 0.5f);
			if (directional && light.m_Radius > 0.0f)
			{
				const float tangent = light.m_Radius / std::max(projected->GetScale(), 1e-6f);
				light.m_CosCone = 1.0f / std::sqrt(1.0f + tangent * tangent);
			}
		}
		else if (const g3dPointLight* point = dynamic_cast<const g3dPointLight*>(&i_Light))
		{
			light.m_Type = e_optxLightPoint;
			const maPoint3d& position = point->GetPosition();
			set3(light.m_Position, position.m_X, position.m_Y, position.m_Z);
			light.m_Falloff[0] = point->GetFalloff0();
			light.m_Falloff[1] = point->GetFalloff1();
			light.m_Falloff[2] = point->GetFalloff2();
			light.m_Falloff[3] = point->GetFalloff3();
			light.m_FalloffStart = point->GetFalloffStart();
		}
		else if (const g3dDirectionalLight* directional = dynamic_cast<const g3dDirectionalLight*>(&i_Light))
		{
			light.m_Type = e_optxLightDirectional;
			set_direction(light.m_Direction, directional->GetDirection());
		}
		light.m_Flags = shadows ? unsigned(e_optxLightShadows) : 0u;
		return light;
	}

	g3dLightTableOptiX::g3dLightTableOptiX()
		: m_bUploaded(false)
		, m_Version(0)
	{
	}

	g3dLightTableOptiX::~g3dLightTableOptiX()
	{
	}

	bool g3dLightTableOptiX::Update(const std::vector<LightSet>& i_Sets)
	{
		const g3dPrefs::g3dRenderPrefs& prefs = g3dPrefs::CurrentPrefs();
		gvk::g3dLightMgrVk* manager = gvk::g3dLightMgrVk::Implementation();
		const bool headlight = manager && manager->IsHeadlightEnabled();

		std::vector<optxLight> table;
		std::vector<const g3dLight*> sources;
		if (manager)
		{
			for (const g3dLight* light : manager->GetLights())
			{
				if (!light || !light->IsEnabled())
				{
					continue;
				}
				const bool projected = dynamic_cast<const g3dProjectedLight*>(light) != NULL;
				if (projected ? !prefs.m_bProjLightsOn : !prefs.m_bPtLightsOn)
				{
					continue;
				}
				table.push_back(Describe(*light, headlight));
				sources.push_back(light);
			}
		}

		std::vector<optxLightSet> sets;
		std::vector<unsigned int> indices;
		for (const LightSet& set : i_Sets)
		{
			optxLightSet entry;
			entry.m_First = static_cast<unsigned int>(indices.size());
			for (size_t i = 0; i < sources.size(); ++i)
			{
				if (headlight || std::find(set.begin(), set.end(), sources[i]) != set.end())
				{
					indices.push_back(static_cast<unsigned int>(i));
				}
			}
			entry.m_Count = static_cast<unsigned int>(indices.size()) - entry.m_First;
			sets.push_back(entry);
		}

		if (m_bUploaded && same(table, m_Table) && same(sets, m_SetTable) && same(indices, m_IndexTable))
		{
			return false;
		}
		optxContext::MakeCurrent();
		m_bUploaded = m_Lights.Upload(table.data(), table.size() * sizeof(optxLight)) &&
					  m_Sets.Upload(sets.data(), sets.size() * sizeof(optxLightSet)) &&
					  m_Indices.Upload(indices.data(), indices.size() * sizeof(unsigned int));
		m_Table.swap(table);
		m_SetTable.swap(sets);
		m_IndexTable.swap(indices);
		if (!m_bUploaded)
		{
			m_Table.clear();	// nothing lit rather than stale lights
		}
		++m_Version;
		return true;
	}

	void g3dLightTableOptiX::Release()
	{
		m_Lights.Free();
		m_Sets.Free();
		m_Indices.Free();
		m_Table.clear();
		m_SetTable.clear();
		m_IndexTable.clear();
		m_bUploaded = false;
	}
}
