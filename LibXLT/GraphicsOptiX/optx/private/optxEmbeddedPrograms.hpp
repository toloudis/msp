/*****************************************************************************
**  optxEmbeddedPrograms.hpp
**
**      GraphicsOptiX's device programs, compiled to OptiX IR by nvcc at
**	build time (EmbedProgramsOptiX.ps1, from the .cu files in
**	optx/private/Programs) and embedded. Each is named after its .cu file. A build without the
**	CUDA toolkit or the OptiX SDK has none.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXEMBEDDEDPROGRAMS_HPP
#error optxEmbeddedPrograms.hpp multiply included
#endif
#define GOX_OPTXEMBEDDEDPROGRAMS_HPP

#include <cstddef>

namespace gox
{
	struct optxEmbeddedProgram
	{
		const char*				m_Name;
		const unsigned char*	m_pIR;		// OptiX IR
		size_t					m_Size;		// bytes
	};

	class optxEmbeddedPrograms
	{
		public:
			//--------------------------------------------------------------------
			//	Find - the program named i_Name, or NULL
			//--------------------------------------------------------------------
			static const optxEmbeddedProgram* Find(const char* i_Name);

			//	k_NumPrograms entries (one placeholder entry when there are none)
			static const optxEmbeddedProgram	k_Programs[];
			static const int					k_NumPrograms;
	};
}
