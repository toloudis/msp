/****************************************************************************\
**  matTexturePathUtil.hpp
**
**      matTexturePathUtil defines utility functions for resolving
**	single filenames and relative paths into full paths for texture files.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_TEXTUREPATHUTIL_HPP
#error matTexturePathUtil.hpp multiply included
#endif
#define MAT_TEXTUREPATHUTIL_HPP

class fsLocator;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace matTexturePathUtil
{
	//--------------------------------------------------------------------
	// Check absolute path, finding local directories that match
	// and converting relative paths to full paths.
	//--------------------------------------------------------------------
	bool ResolveFullPath(fsLocator &io_FilePath,
						  const fsLocator& i_ContainingFile,
						  bool i_bAllowSingleFilenameTextures,
						  bool i_bResolveAbsolutePaths);

}

