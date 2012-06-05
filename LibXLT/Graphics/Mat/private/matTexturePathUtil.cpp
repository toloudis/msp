/*****************************************************************************
**	matTexturePathUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mat/matTexturePathUtil.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsLocator.hpp"

//============================================================================
//============================================================================
namespace matTexturePathUtil
{
	namespace
	{
		const bool bDebugTextureResolving = false;

		//--------------------------------------------------------------------
		// Use a basic search from the directory of the containing file
		// to try to find the texture.
		//--------------------------------------------------------------------
		bool find_texture(const itString &i_Filename,
						  const fsLocator& i_ContainingFile,
						  fsLocator &o_FilePath)
		{
			// Need some sort of path to do the search
			if (i_ContainingFile.GetNumNames() < 2)
				return false;

			fsLocator tex_dir(i_ContainingFile);
			tex_dir.Pop();

			fsLocator fullpath = tex_dir;
			fullpath.Push(i_Filename);

			// If we fail to find the texture later, this is going to be our best guess.
			o_FilePath = fullpath; 
			if (fsFileUtil::FileExists(fullpath))
				return true;

			// Try some other variations, but only set them into the o_FilePath if they are found

			// First try ./Textures
			tex_dir.Push("Textures");
			fullpath = tex_dir;
			fullpath.Push(i_Filename);
			if (fsFileUtil::FileExists(fullpath))
			{
				o_FilePath = fullpath; 
				return true;
			}
			tex_dir.Pop();	// restore to "." directory

			// Finally, try ../Textures
			tex_dir.Pop();	// go up one directory
			tex_dir.Push("Textures");
			fullpath = tex_dir;
			fullpath.Push(i_Filename);
			if (fsFileUtil::FileExists(fullpath))
			{
				o_FilePath = fullpath; 
				return true;
			}

			// o_FilePath was set above to be the current directory of the containing file,
			// let that path be returned here as the starting point for the "Locate" dialog.
			return false;
		}
	}

	//--------------------------------------------------------------------
	// Check absolute path, finding local directories that match
	// and converting relative paths to full paths.
	//--------------------------------------------------------------------
	bool ResolveFullPath(fsLocator &io_FilePath,
						  const fsLocator& i_ContainingFile,
						  bool i_bAllowSingleFilenameTextures,
						  bool i_bResolveAbsolutePaths)
	{
		bool bChangedLocator = false;
		fsLocator tex_loc = io_FilePath;

		if (bDebugTextureResolving)
		{
			DBG_LOG("ResolveFullPath, original: " << io_FilePath);
		}

		// Flag controls whether single texture filenames need to
		// be promoted to full paths.
		if (!i_bAllowSingleFilenameTextures)
		{
			if (tex_loc.GetNumNames() == 1)
			{
				itString filename = tex_loc.GetLastName();
				find_texture(filename, i_ContainingFile, tex_loc);
				io_FilePath = tex_loc;
				bChangedLocator = true;
			}
		}

		// Also need to consider relative paths here
		if (tex_loc.GetNumNames() > 1)
		{
			if (tex_loc.GetName(0) == itString("."))
			{
				fsLocator local_path = i_ContainingFile;
				local_path.Pop();
				tex_loc.RemoveBefore(1); // take off the "."
				local_path.Push(tex_loc);
				io_FilePath = local_path;
				bChangedLocator = true;
				
			}
			else if (tex_loc.GetName(0) == itString(".."))
			{
				fsLocator local_path = i_ContainingFile;
				local_path.Pop();
				
				// For every ".." move up a directory in the
				// local directory path
				while ((tex_loc.GetNumNames() > 1) &&
					   (local_path.GetNumNames() > 1) )
				{
					if (tex_loc.GetName(0) == itString(".."))
					{
						tex_loc.RemoveBefore(1);
						local_path.Pop();
					}
					else break;
				}
				local_path.Push(tex_loc);
				io_FilePath = local_path;
				bChangedLocator = true;
			}
		}


		// Sometimes we don't want to resolve the paths, either because
		// the material information is going to be overriden, or we
		// really want to know what the original references were.
		if (i_bResolveAbsolutePaths)
		{
			// Now that we have a fullpath, give it to
			// the AbsolutePathMgr so that the fullpath can be mapped
			// locally, or the user can be prompted.
			tex_loc = io_FilePath;
			if (tex_loc.GetNumNames() > 1)
			{
				if (fsAbsolutePathMgr::ResolvePath(tex_loc, "Textures"))
				{
					io_FilePath = tex_loc;
					bChangedLocator = true; // maybe check for real differences here?
				} 
			}
		}

		if (bDebugTextureResolving)
		{
			DBG_LOG("ResolveFullPath, altered: " << io_FilePath);
		}

		return bChangedLocator;
	}

} // end of namespace

