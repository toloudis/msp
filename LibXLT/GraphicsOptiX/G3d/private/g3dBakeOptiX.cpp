/*****************************************************************************
**  g3dBakeOptiX.cpp
**
**      See g3dBakeOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DBAKEOPTIX_HPP
#include "GraphicsOptiX/G3d/private/g3dBakeOptiX.hpp"
#endif

namespace gox
{
	int g3dBakeOptiX::Init(const g2dPFD& /*i_PFD*/, const g3dScene* /*i_pScene*/, const fsLocator& /*i_OutputPath*/,
						   float /*i_fSimTime*/, const camCamera& /*i_Camera*/, bool /*i_bIsSaveAndReplace*/,
						   const std::string& /*i_OutputFormat*/, int /*i_Res*/, int /*i_TextureReduce*/)
	{
		return 0;
	}

	int g3dBakeOptiX::CleanUp()
	{
		return 0;
	}

	bool g3dBakeOptiX::BakeNextNode(std::map<const g3dFragment*, std::string>& /*io_TextureNameMap*/)
	{
		return false;
	}

	void g3dBakeOptiX::SetOutputDirectory(const fsLocator& /*i_OutputDir*/)
	{
	}
}
