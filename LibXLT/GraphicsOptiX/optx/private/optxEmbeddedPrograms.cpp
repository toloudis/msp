/*****************************************************************************
**  optxEmbeddedPrograms.cpp
**
**      See optxEmbeddedPrograms.hpp. The table itself is generated
**	(Generated/optxEmbeddedPrograms.inc) by GraphicsOptiX.vcxproj's
**	EmbedOptixPrograms target.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_OPTXEMBEDDEDPROGRAMS_HPP
#include "GraphicsOptiX/optx/private/optxEmbeddedPrograms.hpp"
#endif

#include <cstring>

namespace gox
{
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

#include "GraphicsOptiX/optx/private/Generated/optxEmbeddedPrograms.inc"
