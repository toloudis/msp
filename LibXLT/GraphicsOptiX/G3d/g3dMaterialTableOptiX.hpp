/*****************************************************************************
**  g3dMaterialTableOptiX.hpp
**
**      g3dMaterialTableOptiX is the material table the device programs
**	read (optxMaterialTypes.h's optxMaterial records): one record per
**	material of a scene BVH (g3dSceneBVHOptiX::GetMaterials, which build
**	inputs name by index). A renderer keeps one and calls Update before
**	each launch, between its texture table's Begin and End.
**
**	Update converts each material (effTraceMaterial::Of, first layer) and
**	adds its textures to the texture table (sampled wrap, trilinear). The
**	records are uploaded only when they differ from the last ones; then
**	the version rises (accumulation restarts). So a material edit
**	(a colour, a texture swapped) restarts; a texture's contents changing
**	is the texture table's version.
**
**	Render thread, with the OptiX context current.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DMATERIALTABLEOPTIX_HPP
#error g3dMaterialTableOptiX.hpp multiply included
#endif
#define GOX_G3DMATERIALTABLEOPTIX_HPP

#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif
#ifndef GOX_OPTXMATERIALTYPES_H
#include "GraphicsOptiX/optx/private/Programs/optxMaterialTypes.h"
#endif

#include <cstdint>
#include <vector>

class matMaterial;
struct effTraceMaterial;

namespace gox
{
	class g3dTextureTableOptiX;

	class g3dMaterialTableOptiX
	{
		public:
			//--------------------------------------------------------------------
			//	Describe - a converted material as the device programs see it,
			//	its textures added to i_Textures
			//--------------------------------------------------------------------
			static optxMaterial Describe(const effTraceMaterial& i_Material, g3dTextureTableOptiX& i_Textures);

			g3dMaterialTableOptiX();
			~g3dMaterialTableOptiX();
			g3dMaterialTableOptiX(const g3dMaterialTableOptiX&) = delete;
			g3dMaterialTableOptiX& operator=(const g3dMaterialTableOptiX&) = delete;

			bool Update(const std::vector<const matMaterial*>& i_Materials, g3dTextureTableOptiX& i_Textures);
			void Release();

			CUdeviceptr GetMaterials() const { return m_Materials.Get(); }
			unsigned GetCount() const { return static_cast<unsigned>(m_Table.size()); }
			uint64_t GetVersion() const { return m_Version; }
			const std::vector<optxMaterial>& GetTable() const { return m_Table; }

		private:
			std::vector<optxMaterial>	m_Table;
			bool						m_bUploaded;
			cuBuffer					m_Materials;
			uint64_t					m_Version;
	};
}
