/****************************************************************************\
**	mtrTextureFinder.hpp
**
**		mtrTextureFinder.hpp provides an implementation of
**	fsResourceFinder that will use the full directory structure
**	under the material library in order to find a texture.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MTR_TEXTUREFINDER_HPP
#error mtrTextureFinder.hpp multiply included
#endif
#define MTR_TEXTUREFINDER_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef FS_RESOURCEFINDER_HPP
#include "Core/fs/fsResourceFinder.hpp"
#endif


//============================================================================
//============================================================================
class mtrTextureFinder : public fsResourceFinder
{
	public:
		//----------------------------------------------------------------
		// Constructor provides default finder to use first
		// before searching material library.
		//----------------------------------------------------------------
		mtrTextureFinder();
		explicit mtrTextureFinder(const fsResourceFinder& i_Finder);
		explicit mtrTextureFinder(const mtrTextureFinder& i_Finder);

		//----------------------------------------------------------------
		//	Trying to find resource i_FileName.  If this finder
		//	knows where it is, it should return true and
		//	put the full path to the resource (including filename)
		//	into o_FoundLocator.
		//----------------------------------------------------------------
		virtual bool FindResource(const itString& i_FileName, 
								  fsLocator& o_FoundLocator) const;

		//----------------------------------------------------------------
		// This function works like the other FindResource, but this
		//	one provides a locator of the resource that is asking
		//	for the file to be found, which may be important for
		//	some implementations.
		//----------------------------------------------------------------
		virtual bool FindResource(const itString& i_FileName, 
								  const fsLocator& i_AskingLocator,
								  fsLocator& o_FoundLocator) const;

	private:
		const fsResourceFinder* m_pFinder;
};
