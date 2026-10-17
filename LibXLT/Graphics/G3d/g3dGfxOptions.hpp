/****************************************************************************\
**  g3dGfxOptions.hpp
**
**      g3dGfxOptions holds the graphics start-up options an application
**	reads from its command line before graphics are initialized.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef G3D_GFXOPTIONS_HPP
#error g3dGfxOptions.hpp multiply included
#endif
#define G3D_GFXOPTIONS_HPP

#include <string>

//============================================================================
//============================================================================
struct g3dGfxOptions
{
	std::string m_ProfileFile;	// -gfxProfile <file.csv>; empty: profiling off
};
