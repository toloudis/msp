/*****************************************************************************
**  optxEmbeddedPrograms.cpp
**
**      See optxEmbeddedPrograms.hpp. The table itself is generated
**	(Generated/optxEmbeddedPrograms.inc) by GraphicsOptiX.vcxproj's
**	EmbedOptixPrograms target, which runs only when the CUDA toolkit and
**	the OptiX SDK are both found.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_OPTXEMBEDDEDPROGRAMS_HPP
#include "GraphicsOptiX/optx/private/optxEmbeddedPrograms.hpp"
#endif

#include <cstring>

namespace gox
{
#if defined(GOX_NO_CUDA_TOOLKIT) || defined(GOX_NO_OPTIX_SDK)
	const optxEmbeddedProgram optxEmbeddedPrograms::k_Programs[] = { { nullptr, nullptr, 0 } };
	const int optxEmbeddedPrograms::k_NumPrograms = 0;
#endif

	const optxEmbeddedProgram* optxEmbeddedPrograms::Find(const char* i_Name)
	{
		for (int i = 0; i < k_NumPrograms; ++i)
		{
			if (std::strcmp(k_Programs[i].m_Name, i_Name) == 0)
			{
				return &k_Programs[i];
			}
		}
		return NULL;
	}
}

#if !defined(GOX_NO_CUDA_TOOLKIT) && !defined(GOX_NO_OPTIX_SDK)
#include "GraphicsOptiX/optx/private/Generated/optxEmbeddedPrograms.inc"
#endif
