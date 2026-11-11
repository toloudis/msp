/*****************************************************************************
**  g3dBakeOptiX.hpp
**
**      g3dBakeOptiX bakes lighting into textures (g3dBake), installed by
**	GraphicsOptiXLayer::InitGraphics.
**
**	Stub: there is nothing to bake (Init returns 0 nodes).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DBAKEOPTIX_HPP
#error g3dBakeOptiX.hpp multiply included
#endif
#define GOX_G3DBAKEOPTIX_HPP

#ifndef G3D_BAKE_HPP
#include "Graphics/G3d/g3dBake.hpp"
#endif

#include <map>
#include <string>

namespace gox
{
	class g3dBakeOptiX : public g3dBakeImpl
	{
		public:
			virtual int Init(const g2dPFD& i_PFD, const g3dScene* i_pScene, const fsLocator& i_OutputPath,
							 float i_fSimTime, const camCamera& i_Camera, bool i_bIsSaveAndReplace,
							 const std::string& i_OutputFormat, int i_Res, int i_TextureReduce);
			virtual int CleanUp();
			virtual bool BakeNextNode(std::map<const g3dFragment*, std::string>& io_TextureNameMap);
			virtual void SetOutputDirectory(const fsLocator& i_OutputDir);
	};
}
