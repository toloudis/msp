/*****************************************************************************
**  g3dEnvironmentOptiX.hpp
**
**      g3dEnvironmentOptiX is the beauty pass's environment light (the
**	launch parameters' optxEnvironmentLight, optxLightingParams.h): one per
**	traced scene, an infinitely distant sphere. A renderer keeps one.
**
**	Update (between the texture table's Begin and End) chooses it:
**	- none while the environment preference (g3dPrefs::
**	  m_bEnableEnvironment) is off;
**	- the global environment (g3dScene::GetGlobalAmbient) when it is
**	  enabled (m_bEnableSwlEnv);
**	- otherwise the environment most traced fragments are lit by (the
**	  scene BVH's GetEnvironment; if they have more than one, that is
**	  logged once: the rest wait for per-object environments).
**	Of the chosen environment it takes the specular map (the one the
**	raster reads along the reflection vector, usually the sharp radiance)
**	if there is one, else the diffuse map, with that map's colour x factor
**	and Y rotation angle (degrees, as the raster's); with no map, a constant
**	colour x factor (the specular one if its factor is above 0, else the
**	diffuse one). The map is added to the texture table in environment
**	mode (wrap in u, clamp in v, trilinear). It lights the BRDF's diffuse
**	and specular lobes as the diffuse and specular lighting preferences
**	say; camera misses show it when it is the global environment and its
**	background is on (m_bEnableSwlEnvBG). It is off if it would give no
**	light and no background, or its map can't be sampled (logged once).
**
**	Prepare (after the texture table's End, once CUDA has waited for
**	Vulkan's writes to the map) builds the sampling table on the GPU
**	(optxLighting::BuildEnvironmentTable) when the map, its contents
**	(vkdImage::Generation) or the colour changed since the last build: at
**	most 512 x 256 cells (the map's size halved until it fits), each read
**	as up to 8 x 8 samples at the level that covers the cell. The table
**	is in map coordinates, so a new angle needs no new table. If the build
**	fails, the light is sampled cosine-weighted instead.
**
**	GetVersion rises whenever what the light gives changes (on or off,
**	map, its contents, colour, angle, switches, background): accumulation
**	restarts.
**
**	Render thread, with the OptiX context current.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DENVIRONMENTOPTIX_HPP
#error g3dEnvironmentOptiX.hpp multiply included
#endif
#define GOX_G3DENVIRONMENTOPTIX_HPP

#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif
#ifndef GOX_OPTXLIGHTINGPARAMS_H
#include "GraphicsOptiX/optx/private/Programs/optxLightingParams.h"
#endif

#include <cstdint>
#include <vector>

class g3dScene;
class matTexture;

namespace gox
{
	class g3dSceneBVHOptiX;
	class g3dTextureTableOptiX;
	class optxLighting;

	class g3dEnvironmentOptiX
	{
		public:
			//	The table's largest size
			static const unsigned c_MaxTableWidth = 512;
			static const unsigned c_MaxTableHeight = 256;
			static const unsigned c_MaxTaps = 8;

			g3dEnvironmentOptiX();
			~g3dEnvironmentOptiX();
			g3dEnvironmentOptiX(const g3dEnvironmentOptiX&) = delete;
			g3dEnvironmentOptiX& operator=(const g3dEnvironmentOptiX&) = delete;

			//--------------------------------------------------------------------
			//	Update - see above. Returns whether the version rose.
			//--------------------------------------------------------------------
			bool Update(const g3dScene& i_Scene, const g3dSceneBVHOptiX& i_BVH, g3dTextureTableOptiX& i_Textures);

			//--------------------------------------------------------------------
			//	Prepare - see above. i_Textures: the texture table Update added
			//	the map to (its device table). False if a build was needed and
			//	couldn't be queued (the light then has no table).
			//--------------------------------------------------------------------
			bool Prepare(optxLighting& i_Lighting, const g3dTextureTableOptiX& i_Textures);

			//--------------------------------------------------------------------
			//	Release - frees the table; the next Prepare builds it again
			//--------------------------------------------------------------------
			void Release();

			//--------------------------------------------------------------------
			//	The record for the launch parameters (m_Flags 0: no light)
			//--------------------------------------------------------------------
			const optxEnvironmentLight& GetLight() const { return m_Light; }
			uint64_t GetVersion() const { return m_Version; }

			//--------------------------------------------------------------------
			//	For tests: the map chosen (NULL: none), table builds so far, and
			//	the table read back (optxEnvTableFloats floats; empty if none)
			//--------------------------------------------------------------------
			const matTexture* GetMap() const { return m_pMap; }
			int GetBuildCount() const { return m_nBuilds; }
			std::vector<float> ReadTable() const;

		private:
			//	What the light gives (its version rises when this changes)
			struct State
			{
				const void*		m_pImage = nullptr;
				uint64_t		m_Generation = 0;
				unsigned		m_Flags = 0;
				float			m_Color[3] = {};
				float			m_Angle = 0.0f;

				bool operator==(const State& i_Other) const;
			};
			//	What the table was built from
			struct Built
			{
				const void*		m_pImage = nullptr;
				uint64_t		m_Generation = 0;
				float			m_Color[3] = {};
				unsigned		m_Width = 0;
				unsigned		m_Height = 0;

				bool operator==(const Built& i_Other) const;
			};

			optxEnvironmentLight	m_Light;
			const matTexture*		m_pMap;
			State					m_State;
			Built					m_Built;
			bool					m_bBuilt;		// m_Table holds the table for m_Built
			cuBuffer				m_Table;
			uint64_t				m_Version;
			int						m_nBuilds;
			bool					m_bLoggedMany;
			bool					m_bLoggedMap;
	};
}
