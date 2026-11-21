/*****************************************************************************
**  g3dLightTableOptiX.hpp
**
**      g3dLightTableOptiX is the light table the lighting programs read
**	(optxLightTypes.h): an optxLight per light, and the light sets the
**	scene BVH's instances name, as indices into them. A renderer keeps one
**	and calls Update before each launch.
**
**	Update takes GraphicsVulkan's lights (g3dLightMgrVk::GetLights: the
**	scene's, or the headlight alone while it is on) that are enabled and
**	allowed by the preferences (m_bProjLightsOn for projected lights,
**	m_bPtLightsOn for the others, as the raster's lit pass), and describes
**	each as the raster lighting sees it (see optxLightTypes.h):
**	- g3dPointLight (and g3dSpotLight, which the raster also lights as a
**	  point): position, falloff;
**	- g3dDirectionalLight: direction;
**	- g3dProjectedLight: a spot or directional projected light (by
**	  GetIsDirectional), with position, direction, falloff, the world-to-
**	  texture matrix and its z limit (the raster's NearFar.y), inner and
**	  outer angles and aspect; an area light of radius GetLightSize / 2 (a
**	  directional one: a cone of directions whose half angle has tangent
**	  radius / GetScale).
**	Any light traces shadows when GetShadowSource, except the headlight;
**	GetIsSinglePassLight is the rasterizers' and is ignored.
**	Its colour is the intensity colour times the intensity factor; it
**	lights diffuse surfaces (e_optxLightDiffuse) when its diffuse light and
**	the preference for diffuse lighting are on, and gives highlights
**	(e_optxLightSpecular) when its specular light and the preference for
**	specular lighting are on.
**
**	A projected light's texture (g3dProjectedLight::GetTexture: its image,
**	or its ramp's texture) is added to the renderer's texture table in
**	projector mode (border, bilinear, mip 0); its contents changing is the
**	texture table's version.
**
**	A light set (the BVH's GetLightSets: the lights each instance's render
**	states name) becomes the indices of those of its lights in the table.
**	While the headlight is on, every set holds it.
**
**	The table, sets and indices are uploaded only when they differ from the
**	last ones; then the version rises (accumulation restarts).
**
**	Render thread; Update makes the OptiX context current.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DLIGHTTABLEOPTIX_HPP
#error g3dLightTableOptiX.hpp multiply included
#endif
#define GOX_G3DLIGHTTABLEOPTIX_HPP

#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif
#ifndef GOX_OPTXLIGHTTYPES_H
#include "GraphicsOptiX/optx/private/Programs/optxLightTypes.h"
#endif

#include <cstdint>
#include <vector>

class g3dLight;

namespace gox
{
	class g3dTextureTableOptiX;

	class g3dLightTableOptiX
	{
		public:
			typedef std::vector<const g3dLight*> LightSet;

			//--------------------------------------------------------------------
			//	Describe - a light as the lighting programs see it (see above).
			//	i_bHeadlight: it is the headlight (no shadows). m_Texture is
			//	c_optxNoTexture (Update sets it).
			//--------------------------------------------------------------------
			static optxLight Describe(const g3dLight& i_Light, bool i_bHeadlight);

			g3dLightTableOptiX();
			~g3dLightTableOptiX();
			g3dLightTableOptiX(const g3dLightTableOptiX&) = delete;
			g3dLightTableOptiX& operator=(const g3dLightTableOptiX&) = delete;

			//--------------------------------------------------------------------
			//	Update - see above. Returns whether the table changed.
			//--------------------------------------------------------------------
			bool Update(const std::vector<LightSet>& i_Sets, g3dTextureTableOptiX& i_Textures);

			//--------------------------------------------------------------------
			//	Release - frees everything on the device; the next Update
			//	uploads again
			//--------------------------------------------------------------------
			void Release();

			//--------------------------------------------------------------------
			//	The device arrays (optxLight, optxLightSet, unsigned int)
			//--------------------------------------------------------------------
			CUdeviceptr GetLights() const { return m_Lights.Get(); }
			CUdeviceptr GetSets() const { return m_Sets.Get(); }
			CUdeviceptr GetIndices() const { return m_Indices.Get(); }
			unsigned GetLightCount() const { return static_cast<unsigned>(m_Table.size()); }

			//--------------------------------------------------------------------
			//	GetVersion - raised each time Update changes the table
			//--------------------------------------------------------------------
			uint64_t GetVersion() const { return m_Version; }

			//--------------------------------------------------------------------
			//	For tests: the last table
			//--------------------------------------------------------------------
			const std::vector<optxLight>& GetTable() const { return m_Table; }

		private:
			std::vector<optxLight>		m_Table;
			std::vector<optxLightSet>	m_SetTable;
			std::vector<unsigned int>	m_IndexTable;
			bool						m_bUploaded;
			cuBuffer					m_Lights;
			cuBuffer					m_Sets;
			cuBuffer					m_Indices;
			uint64_t					m_Version;
	};
}
