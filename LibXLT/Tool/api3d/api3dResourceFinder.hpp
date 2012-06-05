/****************************************************************************\
**	api3dResourceFinder.hpp
**
**		Implements an interface for finding resource files within a
**	directory structure.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_RESOURCEFINDER_HPP
#error api3dResourceFinder.hpp multiply included
#endif
#define API3D_RESOURCEFINDER_HPP

#ifndef FS_RESOURCEFINDER_HPP
#include "Core/fs/fsResourceFinder.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class itString;


//============================================================================
//============================================================================
class api3dResourceFinder : public fsResourceFinder
{
	public:
		//--------------------------------------------------------------------
		//	the "root" directory for Data, Models, Textures, Sounds, etc.
		//--------------------------------------------------------------------
		api3dResourceFinder(const fsLocator& i_RootDirectory);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~api3dResourceFinder() {};

		//--------------------------------------------------------------------
		//	Trying to find resource i_FileName.  If this finder
		//	knows where it is, it should return true and
		//	put the full path to the resource (including filename)
		//	into o_FoundLocator.
		//--------------------------------------------------------------------
		virtual bool FindResource(const itString& i_FileName,
								  fsLocator& o_FoundLocator) const;

		//--------------------------------------------------------------------
		// This function works like the other FindResource, but this
		//	one provides a locator of the resource that is asking
		//	for the file to be found, which may be important for
		//	some implementations.
		//--------------------------------------------------------------------
		virtual bool FindResource(const itString& i_FileName,
								  const fsLocator& i_AskingLocator,
								  fsLocator& o_FoundLocator) const;
private:
	fsLocator m_RootDirectory;
};
